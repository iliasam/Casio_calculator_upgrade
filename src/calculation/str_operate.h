#include "stdint.h"

void add_to_str(uint8_t *source,uint8_t *dest,uint8_t lng,uint8_t pos);
void cut_from_str(uint8_t *source,uint8_t pos,uint8_t lng);
void cut_chr_from_str(uint8_t *source,uint8_t pos);
void replace_by_char(uint8_t *source,uint8_t pos,uint8_t lng,uint8_t chr);
uint8_t cheek_and_replace_substring(char *work_str, char *substring, char replace_symbol);
uint8_t cheek_and_replace_symbol(char *work_str, char symbol, char* replace_str);





