#ifndef __TEXT_FUNCTIONS_H
#define __TEXT_FUNCTIONS_H
#include "stdint.h"
#include "string.h"

void text_insert_string(char* base_string, char* add_string, uint16_t pos, uint16_t length);
void text_delete_symbols(char* base_string,  uint16_t pos, uint16_t length);

#endif
