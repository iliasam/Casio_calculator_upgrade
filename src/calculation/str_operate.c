#include "str_operate.h"
#include "text_functions.h"
//#include <iostream>

//legacy code


void add_to_str(uint8_t *source,uint8_t *dest,uint8_t lng,uint8_t pos)
{
  text_insert_string((char*)dest, (char*)source, pos, lng);
}  


void cut_from_str(uint8_t *source,uint8_t pos,uint8_t lng)
{
  text_delete_symbols((char*)source,  pos, lng);
}  

void cut_chr_from_str(uint8_t *source,uint8_t pos)
{
  text_delete_symbols((char*)source,  pos, 1);
} 


void replace_by_char(uint8_t *source,uint8_t pos,uint8_t lng,uint8_t chr)
{
//удаляет из строки часть символов и заменяет их символом
cut_from_str(source,pos,lng);
add_to_str(&chr,source,1,pos);
} 