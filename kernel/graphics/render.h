#ifndef RENDER_H
#define RENDER_H

#include <stdint.h>
#include <kernel/graphics/vesafb.h>

typedef rgba_color (*Renderer_t)[600];

void render_fill(Renderer_t renderer, int start_x, int start_y, int length_across, int length_down, uint32_t color);
void render_pixel(Renderer_t renderer, int x, int y, uint32_t color);
void render_filled_circle(Renderer_t renderer, int16_t x, int16_t y, int16_t rad, uint8_t r, uint8_t g, uint8_t b);
void render_vga_character(Renderer_t renderer, uint8_t c, int x, int y, int fg, int bg, bool bgon);
void render_text_string(Renderer_t renderer, const char *text, int x, int y, int fg, int bg, bool bgon);
void render_horizontal_line(Renderer_t renderer, int x, int y,int length, uint32_t color);
void render_vertical_line(Renderer_t renderer, int x, int y, int length, uint32_t color);
void render_square(Renderer_t renderer, int x, int y, int width, int height, uint32_t color);
void render_head(Renderer_t renderer, int16_t x, int16_t y, int16_t rad);
void draw_rendered(Renderer_t renderer);


#endif