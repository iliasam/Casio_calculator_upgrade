#ifndef _STR_MATH_H
#define _STR_MATH_H

#include "stdio.h"
#include "stdint.h"

typedef enum
{
  CACL_ERR_NO = 0,
  CACL_ERR_BRACKETS,
  CACL_ERR_ZERO_DIV,
  CACL_ERR_NO_ARGUMENT,//нет агрумента функции?
  CACL_ERR_BAD_FORMULA2,//лишние символы в подстроке?
  CACL_ERR_BAD_FORMULA3,//первый символ в строке - не числовой? (4)
  CACL_ERR_LOG,//ln(-1)
  CACL_ERR_ROOT,//sqrt(-1)
  CACL_ERR_TRIGON,//asin(x), where x<-1 or x>1
} CalcErrorType;

typedef struct
{
  double        Answer;
  CalcErrorType Error;
} CalcAnswerType;

void fill_work_buffer(uint8_t *txt,uint8_t length);

CalcAnswerType solve(uint8_t *txt,uint8_t length);
void find_numbers(void);
void bracket_anlyse(void);
void fill_sub_buffer(uint8_t *txt,uint8_t length);

void add_new_number_to_buffer(double new_value);
void replace_symbols_in_work_buffer(void);

uint8_t is_num_sumbol(uint8_t chr);
uint8_t is_mem_sumbol(uint8_t chr);
uint8_t char_is_number(uint8_t chr);

uint8_t is_5_level_func(uint8_t chr);
uint8_t is_4_level_func(uint8_t chr);
uint8_t is_3_level_func(uint8_t chr);
uint8_t is_2_level_func(uint8_t chr);
uint8_t is_1_level_func(uint8_t chr);

uint8_t is_x_level_func(uint8_t chr,uint8_t x);

uint8_t return_function_level(uint8_t funct_code);

void solve_sub_buffer(void);
void solve_work_buffer(void);

void solve_plus(uint8_t pos);
void solve_minus(uint8_t pos);
void solve_multiply(uint8_t pos);
void solve_div(uint8_t pos);
void solve_pow(uint8_t pos);
void solve_dbl(uint8_t pos);//x^2

void solve_sin(uint8_t pos);
void solve_cos(uint8_t pos);
void solve_tg(uint8_t pos);
void solve_asin(uint8_t pos);
void solve_acos(uint8_t pos);
void solve_atan(uint8_t pos);

void solve_sqrt(uint8_t pos);

void solve_ln(uint8_t pos);
void solve_log(uint8_t pos);
void solve_exp(uint8_t pos);


void solve_func(uint8_t pos);

void replace_functions(void);
void replace_metric_symbols(void);

#endif