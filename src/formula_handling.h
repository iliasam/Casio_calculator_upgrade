#ifndef __FORMULA_HANDLING_H
#define __FORMULA_HANDLING_H
#include "stdint.h"

#define FORMULA_MAX_LENGTH 50 //Max formula length is symbols

void formula_clear(void);
int8_t formula_add_text(char* text);
int8_t formula_add_new_keys_symbol(uint16_t key_code);
void formula_delete_symbol(void);

uint16_t formula_calculate_cursor_jump(char* text);

void formula_move_cursor_left(void);
void formula_move_cursor_right(void);



#endif
