#ifndef _MAIN_H
#define _MAIN_H

#include <stdint.h>

void solve_formula(void);

typedef enum 
{
  FORMULA_INPUT = 0, //Input formula
  SELECTOR1_MENU_MODE, //Select norm/sci/eng
  SAVE_MEM_MENU_MODE, //Save answer to memory
  ANSWER_DISPLAY_MENU_MODE, //Draw extended answer info
} ModeStateType;

typedef enum 
{
  INPUT_MODE_BASIC = 0,//main characters input (0-9, +,-, and others)
  INPUT_MODE_ALPHA,     //letters characters input (A-Z)
  INPUT_MODE_SHIFT,     //additional characters
  INPUT_MODE_HEX,       //HEX input
  INPUT_MODE_BIN,       //BIN input
}FormulaInputStateType;

extern float battery_voltage;
extern uint16_t current_key_state;

#endif
