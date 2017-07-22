#ifndef __LCD_WORKER_H
#define __LCD_WORKER_H

#include "stdint.h"
#include "string.h"
#include "ST7565R.h"
#include "fonts.h"

#define LCD_WIDTH       128
#define LCD_HEIGHT      64

#define LCD_LEFT_OFFSET         10
#define LCD_RIGHT_OFFSET        (LCD_WIDTH - 10)

#define FONT_SIZE_6             6//3*5
#define FONT_SIZE_6_WIDTH       4//3*5

#define FONT_SIZE_8             8//5*7
#define FONT_SIZE_8_WIDTH       6//5*7

#define FONT_SIZE_11            12//7*11
#define FONT_SIZE_11_WIDTH      8//7*11

#define LCD_NEW_LINE_FLAG       1//jump to new line
#define LCD_INVERTED_FLAG       2//inverted draw

void lcd_full_clear(void);
void lcd_clear_framebuffer(void);
void lcd_update(void);
void lcd_set_pixel(uint16_t x, uint16_t y);
void lcd_reset_pixel(uint16_t x, uint16_t y);

void lcd_draw_char(uint8_t chr, uint16_t x, uint16_t y, uint8_t font_size, uint8_t flags);
void lcd_set_cursor_pos(uint16_t x, uint16_t y);
uint16_t lcd_draw_string(char *s, uint16_t x, uint16_t y, uint8_t font_size, uint8_t flags);
uint16_t lcd_draw_string_cur(char *s, uint8_t font_size, uint8_t flags);
uint16_t get_font_width(uint8_t font);


#endif

