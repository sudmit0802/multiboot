#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

extern volatile uint8_t tty_feedback;

void tty_init(void);
void tty_setcolor(uint32_t color);

void draw_vga_character(uint8_t c, int x, int y, int fg, int bg, bool bgon);

void tty_putchar(char c);
void tty_write(const char *data, size_t size);
void tty_putstring(const char *data);
void tty_putstring_color(const char *data, uint32_t text_color);
void tty_putuint(int i);
void tty_putint(int i);
void tty_puthex(uint32_t i);
void tty_backspace();

void tty_print(char *format, va_list args);
void tty_printf(char *text, ...);
void tty_scroll();
void tty_update_cursor(size_t row, size_t col);

#endif // _KERNEL_TTY_H