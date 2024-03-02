#include <stddef.h>
#include <stdbool.h>

#include <kernel/graphics/render.h>
#include <kernel/graphics/vesafb.h>

#include <kernel/libk/string.h>
#include <kernel/libk/math.h>

void render_fill(Renderer_t renderer, int start_x, int start_y, int length_across, int length_down, uint32_t color) {
    int i, j;
    for (i = 0; i < length_down; i++) {
        for (j = 0; j < length_across; j++) {
            render_pixel(renderer, start_x + j, start_y + i, color);
        }
    }
}

void render_pixel(Renderer_t renderer, int x, int y, uint32_t color) {
        if (x < 0 || y < 0 || 
        x >= (int) VESA_WIDTH || 
        y >= (int) VESA_HEIGHT) {
        return;
    }
    //rgba_color res_color = ;
    //res_color.a=255;
    renderer[x][y] = rgba_reverse(color);
}

void render_filled_circle(Renderer_t renderer, int16_t x, int16_t y, int16_t rad, uint8_t r, uint8_t g, uint8_t b)
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
                uint32_t res_color = rgb(r,g,b);
                render_pixel(renderer, x + i, y + j, res_color);
                render_pixel(renderer, x + i, y + diameter - j, res_color); // Reflect horizontally
                render_pixel(renderer, x + diameter - i, y + j, res_color); // Reflect vertically
                render_pixel(renderer, x + diameter - i, y + diameter - j, res_color); // Reflect horizontally and vertically
            }
        }
    }
}

void render_head(Renderer_t renderer, int16_t x, int16_t y, int16_t rad)
{

    int16_t centerY = y + rad;

    render_filled_circle(renderer, x+rad/2, centerY-rad/4-rad/6, rad/6, 0,0,0);
    render_filled_circle(renderer, x+rad+rad/4, centerY-rad/4-rad/6, rad/6, 0,0,0);
    render_filled_circle(renderer, x+rad/2+rad/4, centerY+rad/6+rad/12, rad/3, 0,0,0);
}

void render_vga_character(Renderer_t renderer, uint8_t c, int x, int y, int fg, int bg, bool bgon) {
    int cx, cy;
    int mask[8] = { 128, 64, 32, 16, 8, 4, 2, 1 };
    unsigned char *glyph = (uint8_t*) vgafnt + (int) c * 16;

    for (cy = 0; cy < 16; cy++) {
        for (cx = 0; cx < 8; cx++) {
            if (glyph[cy] & mask[cx]) {
                render_pixel(renderer, x + cx, y + cy, fg);
            } else if (bgon == true) {
                render_pixel(renderer, x + cx, y + cy, bg);
            }
        }
    }
}

void render_text_string(Renderer_t renderer, const char *text, int x, int y, int fg, int bg, bool bgon) {
    int len = strlen(text);
    for (int i = 0; i < len; i++) {
        if (x + 8 <= 1024) {
            render_vga_character(renderer, text[i], x, y, fg, bg, bgon);
            x += 8;
        } else {
            break;
        }
    }
}

void render_horizontal_line(Renderer_t renderer, int x, int y,int length, uint32_t color) {
    int i;
    for (i = 0; i <= length; i++) {
        render_pixel(renderer, x, y, color);
        x++;
    }
}

void render_vertical_line(Renderer_t renderer, int x, int y, int length, uint32_t color) {
    int i;
    for (i = 0; i <= length; i++) {
        render_pixel(renderer, x, y, color);
        y++;
    }
}

void render_square(Renderer_t renderer, int x, int y, int width, int height, uint32_t color) {
    render_horizontal_line(renderer, x, y, width, color);
    render_vertical_line(renderer, x, y, height, color);
    render_horizontal_line(renderer, x, y + height, width, color);
    render_vertical_line(renderer, x + width, y, height, color);
}

void draw_rendered(Renderer_t renderer)
{
    for(size_t i = 0; i<VESA_WIDTH; ++i)
    {
        for(size_t j=0;j<VESA_HEIGHT;++j)
        {
            set_pixel_alpha(i, j, renderer[i][j]);
        }
    }
}

