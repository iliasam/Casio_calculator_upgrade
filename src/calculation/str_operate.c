#include "str_operate.h"
#include "text_functions.h"
//#include <iostream>

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

// Remove some symbols from string and rplace them by symbol
// source - source string
// pos - start of substring to be removed
// lng - length of substring
// chr - character that replaces substring
void replace_by_char(uint8_t *source, uint8_t pos, uint8_t lng, uint8_t chr)
{
  cut_from_str(source,pos,lng);
  add_to_str(&chr,source,1,pos);
}

//check if "work_str" contains "substring" at it's beginning.
//If if is true, replace "substring"  by "replace_symbol"
//Return - number of symbols that was removed from string
uint8_t check_and_replace_substring(char *work_str, char *substring, char replace_symbol)
{
  uint8_t substring_size = strlen(substring);
  
  if (strncmp(work_str, substring, substring_size) == 0)
  {
    replace_by_char((uint8_t*)work_str,0,substring_size,replace_symbol);//TODO: remove uint8_t
    return substring_size - 1;
  }
  return 0;
}

//check if "work_str" contains "symbol" at it's beginning.
//If if is true, replace "symbol"  by "replace_str" string
//Return - number of symbols that was added to string
uint8_t check_and_replace_symbol(char *work_str, char symbol, char* replace_str)
{
  if (work_str[0] == symbol)
  {
    uint8_t substring_size = strlen(replace_str);//size of added string
    text_delete_symbols((char*)work_str,  0, 1);//delete char
    text_insert_string((char*)work_str, (char*)replace_str, 0, substring_size);
    return substring_size - 1;
  }
  return 0;
}