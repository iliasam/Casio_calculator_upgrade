//Routines that draw something at LCD must be placed here
//Data path is: display_handling.c (generate text) -> lcd_worker.c (text draw) -> ST7565R.c (send image to LCD)

#include "display_handling.h"
#include "formula_handling.h"
#include "stm32f10x_rtc.h"
#include "text_functions.h"
#include "lcd_worker.h"
#include "menu_handling.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "main.h"

extern ModeStateType            mode_state;
extern FormulaInputStateType    formula_input_state;

extern char formula_text[FORMULA_MAX_LENGTH];
extern uint16_t formula_cursor_position;
extern uint16_t formula_current_length;

extern CalcAnswerType calc_result;

AnswerType answer_type_mode = ANSWER_TYPE_NORMAL; //norm/sci/eng

uint8_t display_input_font = FONT_SIZE_8;

char tmp_formula_buf[50];//holds temporary formula part

extern uint8_t button_pressed_lcd_flag;//flag to tell lcd handler that button was pressed. Cleared by lcd handling functions
uint8_t button_pressed_lcd_local_flag = 0;

uint8_t generate_answer(char* ans_string, double value);

//#############################################################################

void display_handling_update_button_pressed(void);
void leave_menu_selector1(void);

//*****************************************************************************

void display_draw_input_handler(void)
{
  display_handling_update_button_pressed();
  
  if (mode_state == FORMULA_INPUT)
  {
    //char str[32];
    //lcd_draw_string(formula_text, 0, 0*display_input_font, display_input_font, 0);
    //draw_formula_input_cursor();
    draw_cur_oneline_formula();
    draw_answer_in_line(calc_result, 1);
  }
}

//Draw current formula that take single line at LCD (rolling string)
void draw_cur_oneline_formula(void)
{
  uint16_t max_formula_lng = (LCD_RIGHT_OFFSET - LCD_LEFT_OFFSET) / get_font_width(display_input_font) + 1;//maximum formula width in symbols
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
  else if (formula_input_state == INPUT_MODE_HEX)
    local_cursor = CURSOR_INPUT_HEX;
  else if (formula_input_state == INPUT_MODE_BIN)
    local_cursor = CURSOR_INPUT_BIN;
  
  draw_blinking_cursor(local_cursor, x, y, display_input_font);
}

//x, y - cursor position in pixels
void draw_blinking_cursor(CursorType cursor, uint16_t x, uint16_t y, uint8_t font_size)
{
  static uint32_t prev_time = 0;
  static uint8_t cursor_enabled = 1;//used for blinking
  uint32_t cur_time = RTC_GetCounter();
  
  if (button_pressed_lcd_local_flag == 1)//Cursor must be turned on when key is pressed
  {
    cursor_enabled = 1;
    prev_time = cur_time;
  }
  else if ((cur_time - prev_time) > CURSOR_BLINK_PERIOD)//used for blinking
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
        lcd_draw_char('<', x, y, font_size, 0);
        break;
      }
      case CURSOR_INPUT_ALPHA: 
      {
        lcd_draw_char('a', x, y, font_size, LCD_INVERTED_FLAG);
        break;
      }
      case CURSOR_INPUT_SHIFT: 
      {
        lcd_draw_char('s', x, y, font_size, LCD_INVERTED_FLAG);
        break;
      }
      case CURSOR_INPUT_HEX: 
      {
        lcd_draw_char('h', x, y, font_size, LCD_INVERTED_FLAG);
        break;
      }
      case CURSOR_INPUT_BIN: 
      {
        lcd_draw_char('b', x, y, font_size, LCD_INVERTED_FLAG);
        break;
      }
      default: break;
    }
  }
}

//Draw answer in single line
//Нужно доделать - отображение, или выбран инжинерный режим
void draw_answer_in_line(CalcAnswerType result, uint16_t line)
{
  char str[32];
  
  if (result.Error == CACL_ERR_NO)//no errors
  {
    uint16_t max_text_lng = (LCD_RIGHT_OFFSET - LCD_LEFT_OFFSET) / get_font_width(display_input_font) + 1;//maximum text width in symbols
    
    uint8_t length = generate_answer(str, calc_result.Answer);
    if (length > max_text_lng)
      return;//unknown error
    
    uint8_t add_length = max_text_lng - length;//number of symbols that must be added
    uint8_t i;
    for (i = 0; i < add_length; i++)
    {
      text_insert_string(str, " ", 0, 1);//add space char at the beginning of string
    }
    
    lcd_draw_string(str, 0, line*display_input_font, display_input_font, 0);
  }
  else
  {
    switch (calc_result.Error)
    {
      case CACL_ERR_BRACKETS:
        lcd_draw_string("BRAKETS ERROR", 0, line*display_input_font, display_input_font, LCD_INVERTED_FLAG);
        break;
      
      case CACL_ERR_ZERO_DIV:
        lcd_draw_string("DIV BY 0 ERROR", 0, line*display_input_font, display_input_font, LCD_INVERTED_FLAG);
        break;
        
      case CACL_ERR_NO_ARGUMENT:
        lcd_draw_string("NO ARG ERROR", 0, line*display_input_font, display_input_font, LCD_INVERTED_FLAG);
        break;

      case CACL_ERR_LOG:
        lcd_draw_string("LN(x<0) ERROR", 0, line*display_input_font, display_input_font, LCD_INVERTED_FLAG);
        break;
       
      case CACL_ERR_ROOT:
        lcd_draw_string("ROOT ERROR", 0, line*display_input_font, display_input_font, LCD_INVERTED_FLAG);
        break;
        
      case CACL_ERR_TRIGON:
        lcd_draw_string("TRIG ERROR", 0, line*display_input_font, display_input_font, LCD_INVERTED_FLAG);
        break;
        
      default:
      {
        sprintf(str,"ERROR: %d", (uint8_t)calc_result.Error);
        lcd_draw_string(str, 0, line*display_input_font, display_input_font, LCD_INVERTED_FLAG);
      }
    }

  }
}

//Write "value" to "dest_str" string
// "    123.456"
void draw_value_at_right(uint8_t* dest_str, double value, uint8_t str_length)
{
  char str[32];
  uint8_t length = sprintf(str,"%.10g", value);
  
  if (length > str_length)
      return;//too long
  
  uint8_t tmp_start = str_length - length; 
  for (uint8_t i = 0; i < length; i++)
    dest_str[tmp_start + i] = str[i];

}

uint8_t generate_answer(char* ans_string, double value)
{
  uint8_t length = 0;
  
  switch (answer_type_mode)
  {
    case ANSWER_TYPE_NORMAL:
    {
      length = sprintf(ans_string,"%.10g<", value);
      break;
    }
    case ANSWER_TYPE_SCIENCE:
    {
      length = sprintf(ans_string,"%.10e<", value);
      break;
    }
    case ANSWER_TYPE_ENGINEERING:
    {
      char symbol = ' ';
      double div = 1.0;
      
      if (fabs(value) < 1e12)
      {
        symbol = 'G';//giga
        div = 1e9;
      }
      if (fabs(value) < 1e9)
      {
        symbol = 'M';//mega
        div = 1e6;
      }
      if (fabs(value) < 1e6)
      {
        symbol = 'K';//kilo
        div = 1e3;
      }
      if (fabs(value) < 1e3)
      {
        symbol = ' ';//empty
        div = 1.0;
      }
      if (fabs(value) < 1.0)
      {
        symbol = 'm';//mili
        div = 1e-3;
      }
      if (fabs(value) < 1e-3)
      {
        symbol = 'u';//micro
        div = 1e-6;
      }
      if (fabs(value) < 1e-6)
      {
        symbol = 'n';//nano
        div = 1e-9;
      }
      if (fabs(value) < 1e-9)
      {
        symbol = 'p';//pico
        div = 1e-12;
      }
      
      length = sprintf(ans_string,"%.10g%c<", (value / div), symbol);
      break;
    }
    default: break;
  }
  return length;
}

void draw_status_line(void)
{
  char str[32];
  draw_black_line(STATUS_LINE_Y_POS);
  sprintf(str,"BAT: %.2f | KEY: %d | ", battery_voltage, current_key_state);
  lcd_draw_string(str, 0, (STATUS_LINE_Y_POS+2), FONT_SIZE_6, 0);
}

void display_handling_update_button_pressed(void)
{
  if (button_pressed_lcd_flag == 1)
  {
    button_pressed_lcd_flag = 0;
    button_pressed_lcd_local_flag = 1;
  }
  else
    button_pressed_lcd_local_flag = 0;
}


