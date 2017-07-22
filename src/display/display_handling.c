//Routines that draw something at LCD must be placed here

#include "display_handling.h"
#include "formula_handling.h"
#include "stm32f10x_rtc.h"
#include "lcd_worker.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "main.h"

extern ModeStateType            mode_state;
extern FormulaInputStateType    formula_input_state;


extern char formula_text[FORMULA_MAX_LENGTH];
extern uint16_t formula_cursor_position;
extern uint16_t formula_current_length;

extern double answer;//tmp

uint8_t display_input_font = FONT_SIZE_8;

char tmp_formula_buf[50];//holds temporary formula part


void display_draw_input_handler(void)
{
  if (mode_state == FORMULA_INPUT)
  {
    char str[32];
    //lcd_draw_string(formula_text, 0, 0*display_input_font, display_input_font, 0);
    //draw_formula_input_cursor();
    draw_cur_oneline_formula();
    
    sprintf(str,"> %.10g", answer);
    lcd_draw_string(str, 0, 2*display_input_font, display_input_font, 0);
  }
}

//Draw current formula that take single line at LCD (rolling string)
void draw_cur_oneline_formula(void)
{
  uint16_t max_formula_lng = (LCD_RIGHT_OFFSET - LCD_LEFT_OFFSET) / get_font_width(display_input_font);//maximum formula width in symbols
  uint16_t cur_cursor_x = 0;//position in pixels
  
  if (formula_current_length < max_formula_lng)//formula is short
  {
    cur_cursor_x = formula_cursor_position * get_font_width(display_input_font);
    lcd_draw_string(formula_text, 0, 0*display_input_font, display_input_font, 0);
    draw_formula_input_cursor(cur_cursor_x, 0*display_input_font);
  }
  else
  {
    //formula longer than LCD line
    if (formula_cursor_position < max_formula_lng)
    {
      //cursor is at the beginning of formula - draw only beginning of formula
      memcpy(tmp_formula_buf, &formula_text[0], (max_formula_lng - 1));//copy text from begin of formula_text to tmp_formula_buf
      tmp_formula_buf[max_formula_lng] = 0;
      lcd_draw_string(tmp_formula_buf, 0, 0*display_input_font, display_input_font, 0);
      cur_cursor_x = formula_cursor_position * get_font_width(display_input_font);
      draw_formula_input_cursor(cur_cursor_x, 0*display_input_font);
    }
    else
    {
      //cursor will be shown at the right of LCD
      int16_t start_pos = (int16_t)formula_cursor_position - (int16_t)max_formula_lng + 1;//data must be taken from this position
      if (start_pos < 0)
        start_pos = 0;
      //dest, source
      memcpy(tmp_formula_buf, &formula_text[start_pos], (max_formula_lng - 1));//copy text from formula_text to tmp_formula_buf
      tmp_formula_buf[max_formula_lng] = 0;
  
      lcd_draw_string(tmp_formula_buf, 0, 0*display_input_font, display_input_font, 0);
      cur_cursor_x = (max_formula_lng - 1) * get_font_width(display_input_font);//end of LCD string
      draw_formula_input_cursor(cur_cursor_x, 0*display_input_font);
    }
  }
}

//Draw cursor that is used during formula input
//x, y - cursor position in pixels
void draw_formula_input_cursor(uint16_t x, uint16_t y)
{
  CursorType local_cursor = CURSOR_INPUT_BASIC;
  
  if (formula_input_state == INPUT_MODE_BASIC)
    local_cursor = CURSOR_INPUT_BASIC;
  else if (formula_input_state == INPUT_MODE_ALPHA)
    local_cursor = CURSOR_INPUT_ALPHA;
  else if (formula_input_state == INPUT_MODE_SHIFT)
    local_cursor = CURSOR_INPUT_SHIFT;
  
  draw_blinking_cursor(local_cursor, x, y, display_input_font);
}

//x, y - cursor position in pixels
void draw_blinking_cursor(CursorType cursor, uint16_t x, uint16_t y, uint8_t font_size)
{
  static uint32_t prev_time = 0;
  static uint8_t cursor_enabled = 1;//used for blinking
  uint32_t cur_time = RTC_GetCounter();
  
  if ((cur_time - prev_time) > 500)//used for blinking
  {
    prev_time = cur_time;
    cursor_enabled^= 1;
  }
  
  if (cursor_enabled == 1)
  {
    switch (cursor)
    {
      case CURSOR_INPUT_BASIC: 
      {
        lcd_draw_char(60, x, y, font_size, 0);
        break;
      }
      case CURSOR_INPUT_ALPHA: 
      {
        lcd_draw_char(97, x, y, font_size, LCD_INVERTED_FLAG);//97 - a
        break;
      }
      case CURSOR_INPUT_SHIFT: 
      {
        lcd_draw_char(115, x, y, font_size, LCD_INVERTED_FLAG);//115 - s
        break;
      }
      default: break;
    }
  }
}
