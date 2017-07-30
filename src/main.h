#ifndef _MAIN_H
#define _MAIN_H

#define SYMB_MICRO_CODE        0xB5 //181 dec - micro
#define SYMB_PI_CODE           0xB6 //182 dec - pi
#define SYMB_EXP10_CODE        0xB7 //183 dec - exp (10x)
#define SYMB_SQUARE_CODE       0xB2 //178 dec - ^2
#define SYMB_SQRT_CODE         0xB4 //180 dec - sqrt()

//When formula solving found some functions like "sin(..)", chars "sin" will be replaced to single char with code SYMB_SIN_CODE
#define SYMB_SIN_CODE          128 //sin()
#define SYMB_COS_CODE          129 //cos()
#define SYMB_TAN_CODE          130 //tg()
#define SYMB_LN_CODE           131 //ln()
#define SYMB_LOG_CODE          132 //log10()
#define SYMB_EXP_CODE          133 //exp()
#define SYMB_ASIN_CODE         134 //asin()
#define SYMB_ACOS_CODE         135 //acos()
#define SYMB_ATAN_CODE         136 //atan()

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
