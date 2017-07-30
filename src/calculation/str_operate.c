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

//Cheek if "substring" is present at beginning of "work_str".
//If if is true, replace "substring"  by "replace_symbol"
//Return - number of symbols that was emoved from string
uint8_t cheek_and_replace_substring(char *work_str, char *substring, char replace_symbol)
{
  uint8_t substring_size = strlen(substring);
  
  if (strncmp(work_str, substring, substring_size) == 0)
  {
    replace_by_char((uint8_t*)work_str,0,substring_size,replace_symbol);//TODO: remove uint8_t
    return substring_size - 1;
  }
  return 0;
}