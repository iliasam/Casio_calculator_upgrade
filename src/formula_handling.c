#include "formula_handling.h"
#include "keys_handling.h"
#include "text_functions.h"
#include <stddef.h>
#include <string.h>
#include "main.h"

extern ModeStateType            mode_state;
extern FormulaInputStateType    formula_input_state;

char formula_text[FORMULA_MAX_LENGTH];
uint16_t formula_current_length = 0;
uint16_t formula_cursor_position = 0;

//Clean current formula
void formula_clear(void)
{
  memset(formula_text, 0, sizeof(formula_text));
  formula_current_length = 0;
  formula_cursor_position = 0;
}


//processing pressed key
//Try to add new text to formula by key_code
int8_t formula_add_new_keys_symbol(uint16_t key_code)
{
  const char* text = get_text_from_key_code(key_code);
  if (text == NULL)
    return -1;//nothing to add
  
  uint8_t new_text_length = strlen(text);//length of new text to add
  
  /*
  if ((formula_current_length + new_text_length) > (FORMULA_MAX_LENGTH-1)) //Это не очень правильно - нужно учитывать преобразование строк
    return -2;//formula is too long to add new text
  */
  return formula_add_text((char*)text);
}

//Try to add new text to formula
int8_t formula_add_text(char* text)
{
  if (text == NULL)
    return -1;//nothing to add
    
  uint8_t new_text_length = strlen(text);//length of new text to add
  if ((formula_current_length + new_text_length) > (FORMULA_MAX_LENGTH-1))
    return -2;//formula is too long to add new text
  
  if (formula_cursor_position >= (FORMULA_MAX_LENGTH-1))
    return -3;//cursor in wrong place - impossible to add text
  
  text_insert_string(formula_text, text, formula_cursor_position, new_text_length);
  formula_cursor_position+= formula_calculate_cursor_jump(text);
  
  formula_current_length+= new_text_length;
  
  
  return 1;//text was added
}

//Try to move input cursor to some symbols
//Эту функцию нужно переделать, чтобы она перепрыгивала sin()
void formula_move_cursor_left(void)
{
  if (formula_cursor_position > 0) formula_cursor_position--;
}

//Эту функцию нужно переделать, чтобы она перепрыгивала sin()
void formula_move_cursor_right(void)
{
  if (formula_cursor_position < (formula_current_length)) formula_cursor_position++;
}

//Delete symbol after cursor or before it
//Эту функцию нужно переделать, чтобы она перепрыгивала sin()
void formula_delete_symbol(void)
{
  if (formula_current_length > 0)
  {
    if (formula_text[formula_cursor_position] > 0)//if there is a symbol present under cursor
    {
      text_delete_symbols(formula_text, formula_cursor_position, 1);
      formula_current_length--;
    }
    else
    {
      //no symbols after cursor
      formula_cursor_position--;
      text_delete_symbols(formula_text, formula_cursor_position, 1);
      formula_current_length--;
    }
  }
}

//Calculate cursor shift for given text
//For "sin()" shift is 4
uint16_t formula_calculate_cursor_jump(char* text)
{
  if (text == NULL)
    return 0;
  
  uint8_t text_length = strlen(text);
  if (text_length < 2)
    return text_length;
  
  //serach for "("
  uint8_t i;
  for (i=0; i < text_length; i++)
  {
    if (text[i] == '(')
      return i+1;
  }
  return text_length;// full length of text
}

