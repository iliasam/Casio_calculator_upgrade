#ifndef __DISPLAY_HANDLING_H
#define __DISPLAY_HANDLING_H
#include "stdint.h"


typedef enum
{
  CURSOR_INPUT_BASIC = 0,//<
  CURSOR_INPUT_ALPHA,//"A"
  CURSOR_INPUT_SHIFT,//"S"
}CursorType;

void display_draw_input_handler(void);

void draw_cur_oneline_formula(void);
void draw_formula_input_cursor(uint16_t x, uint16_t y);
void draw_blinking_cursor(CursorType cursor, uint16_t x, uint16_t y, uint8_t font_size);

#endif
