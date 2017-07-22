//Special functions to prosess text strings

#include "text_functions.h"

//Insert "add_string" with length "length" to "base_string" to "pos"
//First symbol of "add_string" is placed to "pos"
//text_insert_string("ABCDE", "xy", 2, 2) -> "ABxyCDE"
void text_insert_string(char* base_string, char* add_string, uint16_t pos, uint16_t length)
{
  int16_t i;
  
  if ((base_string == NULL) || (add_string == NULL) || (length == 0))
    return;//nothing to add
  
  uint16_t base_string_lengh = strlen(base_string);
  uint16_t new_string_lenght = base_string_lengh + length;
  
  base_string[new_string_lenght] = 0;//null ending
  
  //shift right part of string to "length" symbols
  for (i = (base_string_lengh - 1); i >= pos; i--)
  {
    base_string[i+length] = base_string[i];
  }
  //copy new string to base_string
  memcpy(&base_string[pos],add_string,length);
}

//Delete symbols*"length" from "base_string" from "pos" position
//This function simply shifts right part "base_string" to left at "length" symbols
void text_delete_symbols(char* base_string,  uint16_t pos, uint16_t length)
{
  uint16_t base_string_lengh = strlen(base_string);
  if (length >= base_string_lengh)//bad situaltion
  {
    base_string[0] = 0;
    return;
  }
  //dest, source
  memcpy(&base_string[pos],&base_string[pos + length], base_string_lengh-pos-1);
  base_string[base_string_lengh - length] = 0;//end of string
}


