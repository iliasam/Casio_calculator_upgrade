#ifndef __DISPLAY_HANDLING_H
#define __DISPLAY_HANDLING_H
#include "stdint.h"
#include "str_math.h"


typedef enum
{
  CURSOR_INPUT_BASIC = 0,//<
  CURSOR_INPUT_ALPHA,//"A"
  CURSOR_INPUT_SHIFT,//"S"
}CursorType;

typedef enum
{
  ANSWER_TYPE_NORMAL = 0,
  ANSWER_TYPE_ENGINEERING,
  ANSWER_TYPE_SCINCE,
}AnswerType;

typedef enum
{
  MENU_SELECTOR1_ANSWER_MODE = 0,
  MENU_SELECTOR1_DUMMY,
} MenuSelector1ModeType;

typedef struct
{
  char*                         menu_caption;
  uint8_t                       number_of_items;
  uint8_t                       selected_item;
  MenuSelector1ModeType         menu_type;
  char*                         item_names[10];
} MenuSelector1Type;

void display_draw_input_handler(void);
void display_draw_menu_handler(void);


void draw_cur_oneline_formula(void);
void draw_formula_input_cursor(uint16_t x, uint16_t y);
void draw_blinking_cursor(CursorType cursor, uint16_t x, uint16_t y, uint8_t font_size);
void draw_answer_in_line(CalcAnswerType result, uint16_t line);

void menu_move_cursor_down(void);
void menu_move_cursor_up(void);
void menu_leave(void);

void draw_selector1_menu(void);


void prepare_menu_selector1_for_answer_mode(void);

#endif
