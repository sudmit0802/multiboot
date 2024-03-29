#include <kernel/graphics/vesafb.h>
#include <kernel/mm/virt_memory.h>
#include <kernel/mm/kheap.h>
#include <kernel/tty.h>

#include <kernel/libk/string.h>
#include <kernel/libk/math.h>

uint8_t *framebuffer_addr;
uint32_t framebuffer_pitch;
uint32_t framebuffer_bpp;
uint32_t framebuffer_width;
uint32_t framebuffer_height;
uint32_t framebuffer_size;

uint8_t *back_framebuffer_addr;

void init_vbe(multiboot_info_t *mboot) {


    svga_mode_info_t *svga_mode = (svga_mode_info_t*) mboot->vbe_mode_info;
    framebuffer_addr = (uint8_t *)svga_mode->physbase; //vmm_temp_map_page(svga_mode->physbase);
    framebuffer_pitch = svga_mode->pitch;
    framebuffer_bpp = svga_mode->bpp;
    framebuffer_width = svga_mode->screen_width;
    framebuffer_height = svga_mode->screen_height;
    //framebuffer_size = framebuffer_width * framebuffer_height * (framebuffer_bpp / 8);
    framebuffer_size = framebuffer_height * framebuffer_pitch;

    uint8_t *frame, *virt;
    for (frame = framebuffer_addr, virt = framebuffer_addr;
         frame < (framebuffer_addr + framebuffer_size);
         frame += PAGE_SIZE, virt += PAGE_SIZE) {
        vmm_map_page(frame, virt);
    }

    create_back_framebuffer();
}

void create_back_framebuffer() {
    
    back_framebuffer_addr = kmalloc(framebuffer_size);
    tty_printf("back_framebuffer_addr = %x\n", back_framebuffer_addr);
    memset(back_framebuffer_addr, 0, framebuffer_size); 
}

void set_pixel(int x, int y, uint32_t color) {
    if (x < 0 || y < 0 || 
        x >= (int) VESA_WIDTH || 
        y >= (int) VESA_HEIGHT) {
        return;
    }

    unsigned where = x * (framebuffer_bpp / 8) + y * framebuffer_pitch;

    framebuffer_addr[where] = color;
    framebuffer_addr[where + 1] = (color >> 8);
    framebuffer_addr[where + 2] = (color >> 16);

}

// TODO use colors in uint32_t format
void rgba_blend(uint8_t result[4], uint8_t fg[4], uint8_t bg[4])
{
    unsigned int alpha = fg[3] + 1;
    unsigned int inv_alpha = 256 - fg[3];
    result[0] = (uint8_t)((alpha * fg[0] + inv_alpha * bg[0]) >> 8);
    result[1] = (uint8_t)((alpha * fg[1] + inv_alpha * bg[1]) >> 8);
    result[2] = (uint8_t)((alpha * fg[2] + inv_alpha * bg[2]) >> 8);
    result[3] = 0xff;
}

// TODO make color uint32_t here
void set_pixel_alpha(int x, int y, rgba_color color) {
    if (x < 0 || y < 0 || 
        x >= (int) VESA_WIDTH || 
        y >= (int) VESA_HEIGHT) {
        return;
    }

    unsigned where = x * (framebuffer_bpp / 8) + y * framebuffer_pitch;

    if (color.a != 255) {
        if (color.a != 0) {

            uint8_t bg[4] = {framebuffer_addr[where], framebuffer_addr[where + 1], framebuffer_addr[where + 2], 255};
            uint8_t fg[4] = {(uint8_t)color.b, (uint8_t)color.g, (uint8_t)color.r, (uint8_t)color.a};
            uint8_t res[4];

            rgba_blend(res, fg, bg);

            framebuffer_addr[where] = res[0]; 
            framebuffer_addr[where + 1] = res[1]; 
            framebuffer_addr[where + 2] = res[2];
            
        } else { // if absolutely transparent dont draw anything
            return;
        }
    } else { // if non transparent just draw rgb
    
        framebuffer_addr[where] = color.b;
        framebuffer_addr[where + 1] = color.g;
        framebuffer_addr[where + 2] = color.r;
    }
}

// Return color of the pixel
uint32_t get_pixel(int x, int y) {
    if (x < 0 || y < 0 || 
        x >= (int) VESA_WIDTH || 
        y >= (int) VESA_HEIGHT) {
        return 0;
    }

    unsigned where = x * (framebuffer_bpp / 8) + y * framebuffer_pitch;

    uint32_t color = 0;
    color = framebuffer_addr[where + 0] + 
            (framebuffer_addr[where + 1] << 8) + 
            (framebuffer_addr[where + 2] << 16);
    return color;
}

uint32_t rgb(uint8_t r, uint8_t g, uint8_t b) {
    return 0xFF000000 + (r * 0x10000) + (g * 0x100) + (b * 0x1);
}

uint32_t rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return (a * 0x1000000) + (r * 0x10000) + (g * 0x100) + (b * 0x1);
}

rgba_color rgba_reverse(uint32_t color) {
    rgba_color result;
    
    result.a = (color >> 24) & 0xFF; // Извлекаем старший байт для альфа-канала
    result.r = (color >> 16) & 0xFF; // Следующий байт для красного
    result.g = (color >> 8) & 0xFF;  // Затем для зелёного
    result.b = color & 0xFF;         // И последний байт для синего
    
    return result;
}

void draw_horizontal_line(int x, int y,int length, uint32_t color) {
    int i;
    for (i = 0; i <= length; i++) {
        set_pixel(x, y, color);
        x++;
    }
}

void draw_vertical_line(int x, int y, int length, uint32_t color) {
    int i;
    for (i = 0; i <= length; i++) {
        set_pixel(x, y, color);
        y++;
    }
}

void draw_square(int x, int y, int width, int height, uint32_t color) {
    draw_horizontal_line(x, y, width, color);
    draw_vertical_line(x, y, height, color);
    draw_horizontal_line(x, y + height, width, color);
    draw_vertical_line(x + width, y, height, color);
}

void draw_filled_circle(int16_t x, int16_t y, int16_t rad, uint8_t r, uint8_t g, uint8_t b)
{
    int16_t diameter = rad * 2;

    // Calculate the center of the circle
    int16_t centerX = x + rad;
    int16_t centerY = y + rad;

    // Iterate over the bounding box of the circle
    for (int16_t i = 0; i < diameter; ++i)
    {
        for (int16_t j = 0; j < diameter; ++j)
        {
            // Calculate the distance from the center of the circle to the current pixel
            int16_t distanceX = abs(centerX - (x + i));
            int16_t distanceY = abs(centerY - (y + j));
            double distance = sqrt(distanceX * distanceX + distanceY * distanceY);

            // Check if the current pixel is inside the circle
            if (distance <= rad)
            {
                // Set the pixel color
                set_pixel(x + i, y + j, (r << 16) | (g << 8) | b);
                set_pixel(x + i, y + diameter - j, (r << 16) | (g << 8) | b); // Reflect horizontally
                set_pixel(x + diameter - i, y + j, (r << 16) | (g << 8) | b); // Reflect vertically
                set_pixel(x + diameter - i, y + diameter - j, (r << 16) | (g << 8) | b); // Reflect horizontally and vertically
            }
        }
    }
}

void draw_fill(int start_x, int start_y, int length_across, int length_down, uint32_t color) {
    int i, j;
    for (i = 0; i < length_down; i++) {
        for (j = 0; j < length_across; j++) {
            set_pixel(start_x + j, start_y + i, color);
        }
    }
}


void draw_vga_character(uint8_t c, int x, int y, int fg, int bg, bool bgon) {
    int cx, cy;
    int mask[8] = { 128, 64, 32, 16, 8, 4, 2, 1 };
    unsigned char *glyph = (uint8_t*) vgafnt + (int) c * 16;

    for (cy = 0; cy < 16; cy++) {
        for (cx = 0; cx < 8; cx++) {
            if (glyph[cy] & mask[cx]) {
                set_pixel(x + cx, y + cy, fg);
            } else if (bgon == true) {
                set_pixel(x + cx, y + cy, bg);
            }
        }
    }
}

void draw_text_string(const char *text, int x, int y, int fg, int bg, bool bgon) {
    int len = strlen(text);
    for (int i = 0; i < len; i++) {
        if (x + 8 <= 1024) {
            draw_vga_character(text[i], x, y, fg, bg, bgon);
            x += 8;
        } else {
            break;
        }
    }
}


