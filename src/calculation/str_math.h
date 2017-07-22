#include "stdio.h"
#include "stdint.h"

void fill_work_buffer(uint8_t *txt,uint8_t length);

uint8_t solve(uint8_t *txt,uint8_t length);
void find_numbers(void);
void bracket_anlyse(void);
void fill_sub_buffer(uint8_t *txt,uint8_t length);

uint8_t is_num_sumbol(uint8_t chr);
uint8_t is_mem_sumbol(uint8_t chr);

uint8_t is_5_level_func(uint8_t chr);
uint8_t is_4_level_func(uint8_t chr);
uint8_t is_3_level_func(uint8_t chr);
uint8_t is_2_level_func(uint8_t chr);
uint8_t is_1_level_func(uint8_t chr);

uint8_t is_x_level_func(uint8_t chr,uint8_t x);

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
void solve_sqrt(uint8_t pos);

void solve_ln(uint8_t pos);
void solve_exp(uint8_t pos);


void solve_func(uint8_t pos);

void replace_functions(void);