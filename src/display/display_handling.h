#ifndef __DISPLAY_HANDLING_H
#define __DISPLAY_HANDLING_H
#include "stdint.h"
#include "str_math.h"

#define CURSOR_BLINK_PERIOD     500 //[ms]

#define STATUS_LINE_Y_POS       56

#define DRAW_ANSWER_BRAKET      1//draw "<" at the end of the answer
#define DRAW_ANSWER_LEFT        2//draw answer at the left

typedef enum
{
  CURSOR_INPUT_BASIC = 0,//<
  CURSOR_INPUT_ALPHA,//"A"
  CURSOR_INPUT_SHIFT,//"S"
  CURSOR_INPUT_HEX,//"H"
  CURSOR_INPUT_BIN,//"b"
}CursorType;

typedef enum
{
  ANSWER_TYPE_NORMAL = 0,
  ANSWER_TYPE_SCIENCE,
  ANSWER_TYPE_ENGINEERING,
  ANSWER_TYPE_HEX,
} AnswerType;


void display_draw_input_handler(void);

void draw_cur_oneline_formula(void);
void draw_formula_input_cursor(uint16_t x, uint16_t y);
void draw_blinking_cursor(CursorType cursor, uint16_t x, uint16_t y, uint8_t font_size);
void draw_answer_in_line(CalcAnswerType result, uint16_t line);
void draw_status_line(void);
void draw_value_at_right(uint8_t* dest_str, double value, uint8_t str_length);
uint8_t generate_answer_by_type(char* ans_string, AnswerType ans_type, double value, uint8_t params);
void draw_good_answer_in_line(CalcAnswerType result, uint16_t line, AnswerType ans_type, uint8_t params);

#endif
