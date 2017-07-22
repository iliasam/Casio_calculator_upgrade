#ifndef _MAIN_H
#define _MAIN_H

#define SYMB_MICRO_CODE        0xB5 //181 dec - micro
#define SYMB_PI_CODE           0xB6 //182 dec - pi
#define SYMB_EXP10_CODE        0xB7 //183 dec - exp (10x)
#define SYMB_SQUARE_CODE       0xB2 //178 dec - ^2
#define SYMB_SQRT_CODE         0xB4 //180 dec - sqrt()

void solve_formula(void);

typedef enum 
{
  FORMULA_INPUT = 0,
  MENU_MODE,
}ModeStateType;

typedef enum 
{
  INPUT_MODE_BASIC = 0,//main characters input (0-9, +,-, and others)
  INPUT_MODE_ALPHA,     //letters characters input (A-Z)
  INPUT_MODE_SHIFT,     //additional characters
}FormulaInputStateType;

#endif
