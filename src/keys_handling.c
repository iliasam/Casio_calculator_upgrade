#include "keys_handling.h"
#include "formula_handling.h"
#include "display_handling.h"
#include "menu_handling.h"
#include "main.h"
#include <stddef.h>
#include <ctype.h>

extern ModeStateType mode_state;
extern FormulaInputStateType formula_input_state;

void callback_key_ac_formula(uint16_t key_code);
void callback_key_ac_menu(uint16_t key_code);

void callback_key_left_formula(uint16_t key_code);
void callback_key_right_formula(uint16_t key_code);
void callback_key_del_formula(uint16_t key_code);

void callback_key_down_menu(uint16_t key_code);
void callback_key_up_menu(uint16_t key_code);
void callback_key_left_menu(uint16_t key_code);
void callback_key_right_menu(uint16_t key_code);

void callback_key_alpha_formula(uint16_t key_code);
void callback_key_shift_formula(uint16_t key_code);
void callback_key_dots_formula(uint16_t key_code);
void callback_key_save_cell_menu(uint16_t key_code);

void callback_key_exe_formula(uint16_t key_code);
void callback_key_exe_menu(uint16_t key_code);

void callback_key_answer_menu(uint16_t key_code);

uint8_t button_pressed_lcd_flag = 0;//flag to tell lcd handler that button was pressed. Cleared by lcd handling functions

//Structure to hold information about keys and corresponding text
KeyTextType keys_text_array[] =
{
  {1, "0", "Z", NULL},
  {2, ".", NULL, NULL},
  {3, "\xB7", "\xB6", "\xB6"},//\xB7 - exp is 10^x, \xB6 - pi
  {4, "1", "U", NULL},
  {5, "2", "V", NULL},
  {6, "3", "W", NULL},
  {7, "+", "X", NULL},
  {8, "-", "Y", NULL},
  {10, "4", "P", NULL},
  {11, "5", "Q", NULL},
  {12, "6", "R", NULL},
  {13, "7", "M", NULL},
  {14, "8", "N", NULL},
  {15, "9", "O", NULL},
  {16, "*", "S", NULL},//mulityply
  {17, "/", "T", NULL},//divide
  {20, NULL, "H", NULL},//save
  {21, "(", "I", NULL},
  {27, ")", "J", NULL},
  {28, "\xB8", "G", NULL},//\xB8 - minus symbol
  {31, NULL,  "A",  NULL},
  {32, "\xB0",  "B",  NULL},//\xB0 - deg symbol
  {33, NULL,  "C",  NULL},
  {34, "^", "K", NULL},
  {35, NULL, "L", NULL},//root
  {36, "sin()", "D", "asin()"},
  {29, "cos()", "E", "acos()"},
  {44, "tg()",  "F",  "atg()"},
  {45, "\xB2",  "n",  NULL},//\xB2 - ^2
  
  {38, NULL,  "m",  NULL},
  
  {43, "log()",  "p",  NULL},
  {42, "ln()",   "f",  "exp()"},
  
  {39, "\xB4()", "\xB5", NULL},//\xB4 - sqrt, \xB5 - micro
  {61, "Ans", NULL, NULL},
};

//Structure to hold information about functional keys
//It hold pointers to the functions that must be called when button is pressed
KeyFunctionalType keys_functional_array[] = 
{
  {20, callback_key_save_cell_menu,   NULL},//DEL
  {25, callback_key_del_formula,   NULL},//DEL
  {37, callback_key_dots_formula,   NULL},//two dots
  {38, callback_key_answer_menu,   NULL},//ENG
  {47, callback_key_right_formula, callback_key_right_menu},//Right
  {48, callback_key_left_formula,  callback_key_left_menu},//Left
  {52, NULL, callback_key_down_menu},//Down
  {54, NULL, callback_key_up_menu},//Up
  {55, callback_key_shift_formula, NULL},//Shift
  {56, callback_key_alpha_formula, NULL},//Alpha
  {60, NULL, NULL},//MODE
  {62, callback_key_exe_formula,   callback_key_exe_menu},//EXE
  {64, callback_key_ac_formula,    callback_key_ac_menu},//AC
};

#define KEYS_TEXT_ARRAY_COUNT (sizeof(keys_text_array) / sizeof(KeyTextType))
#define KEYS_FUNCT_ARRAY_COUNT (sizeof(keys_functional_array) / sizeof(KeyFunctionalType))

//Processing new button pressed
void process_key_state(uint16_t new_key_state)
{
  static uint16_t prev_key_state = 0;
  
  if ((new_key_state != 0) && (new_key_state != prev_key_state))
  {
    prev_key_state = new_key_state;
    button_pressed_lcd_flag = 1;
    
    if (mode_state == FORMULA_INPUT)
    {
      if (formula_add_new_keys_symbol(new_key_state) > 0)//text was added
      {
        if ((formula_input_state == INPUT_MODE_ALPHA) ||
            (formula_input_state == INPUT_MODE_SHIFT))
          formula_input_state = INPUT_MODE_BASIC;//switch to basic input mode after adding text
        return;
      }
    }

    if (process_functional_keys(new_key_state) > 0)
      return;//button was processed
    

  }
  prev_key_state = new_key_state;
  
}

//Used during formula input - take "key_code" and return text that must be added to formula
//In fact in some cases (pi, exp) this text must be converted to single char before adding to real formula
const char* get_text_from_key_code(uint16_t key_code)
{
  uint8_t i;
  
  for (i = 0; i< KEYS_TEXT_ARRAY_COUNT; i++)
  {
    if (keys_text_array[i].code == key_code)
    {
      if (formula_input_state == INPUT_MODE_BASIC)
        return keys_text_array[i].basic_text;
      if (formula_input_state == INPUT_MODE_ALPHA)
        return keys_text_array[i].alpha_text;
      if (formula_input_state == INPUT_MODE_SHIFT)
        return keys_text_array[i].shift_text;
      if (formula_input_state == INPUT_MODE_HEX)
      {
        if (keys_text_array[i].basic_text != NULL &&
            isdigit(keys_text_array[i].basic_text[0]))
          return keys_text_array[i].basic_text;
        else if (keys_text_array[i].alpha_text != NULL &&
                 ((uint8_t)keys_text_array[i].alpha_text[0] >= 'A' &&
                  (uint8_t)keys_text_array[i].alpha_text[0] <= 'F'))
          return keys_text_array[i].alpha_text;
        else
        {
          formula_input_state = INPUT_MODE_BASIC;
          return keys_text_array[i].basic_text;
        }
      }
      else if (formula_input_state == INPUT_MODE_BIN)
      {
        if ((keys_text_array[i].basic_text != NULL) &&
            ( (keys_text_array[i].basic_text[0] == '0') ||
              (keys_text_array[i].basic_text[0] == '1')) )
            return keys_text_array[i].basic_text;
          else
            formula_input_state = INPUT_MODE_BASIC;
          return keys_text_array[i].basic_text;
      }
      
    }//if keys_text_array ...
  }//end of for
  return NULL;
}

//Process pressing functional keys (ALPHA, SHIFT, DEL, AC and so on)
//Return 1 if key was found in table
int8_t process_functional_keys(uint16_t key_code)
{
  uint8_t i;
  for (i = 0; i< KEYS_FUNCT_ARRAY_COUNT; i++)
  {
    if (keys_functional_array[i].code == key_code)
    {
      //try to call callback
      if ((mode_state == FORMULA_INPUT) && (keys_functional_array[i].formula_callback  != NULL))
      {
        keys_functional_array[i].formula_callback(key_code);
        return 1;
      }
      if ((mode_state != FORMULA_INPUT) && (keys_functional_array[i].menu_callback  != NULL))
      {
        keys_functional_array[i].menu_callback(key_code);
        return 1;
      }
      return -1;//no corresponding key found
    }//end of for
  }
  return -1;//no corresponding key found
}

//CALLBACKS
void callback_key_ac_formula(uint16_t key_code)
{
  formula_clear();
  formula_input_state = INPUT_MODE_BASIC;
}

void callback_key_left_formula(uint16_t key_code)
{
  formula_move_cursor_left();
}

void callback_key_right_formula(uint16_t key_code)
{
  formula_move_cursor_right();
}

void callback_key_del_formula(uint16_t key_code)
{
  formula_delete_symbol();
}

//Two dots key
void callback_key_dots_formula(uint16_t key_code)
{
  if (formula_input_state == INPUT_MODE_BASIC)
    formula_input_state = INPUT_MODE_HEX;
  else if (formula_input_state == INPUT_MODE_HEX)
    formula_input_state = INPUT_MODE_BIN;
  else
    formula_input_state = INPUT_MODE_BASIC;
  
  formula_add_system_perfex(formula_input_state);
}

void callback_key_alpha_formula(uint16_t key_code)
{
  if (formula_input_state != INPUT_MODE_ALPHA)
    formula_input_state = INPUT_MODE_ALPHA;
  else
    formula_input_state = INPUT_MODE_BASIC;
}

void callback_key_shift_formula(uint16_t key_code)
{
  if (formula_input_state != INPUT_MODE_SHIFT)
    formula_input_state = INPUT_MODE_SHIFT;
  else
    formula_input_state = INPUT_MODE_BASIC;
}

//"EXE" button
void callback_key_exe_formula(uint16_t key_code)
{
  formula_input_state = INPUT_MODE_BASIC;
  solve_formula();
}

void callback_key_down_menu(uint16_t key_code)
{
  menu_move_cursor_down();
}

void callback_key_up_menu(uint16_t key_code)
{
  menu_move_cursor_up();
}

void callback_key_left_menu(uint16_t key_code)
{
  menu_move_cursor_up();//under development
}

void callback_key_right_menu(uint16_t key_code)
{
  menu_move_cursor_down();//under development
}

//"ENG" button - enter to menu with selection of answer type
void callback_key_answer_menu(uint16_t key_code)
{
  mode_state = SELECTOR1_MENU_MODE;
  prepare_menu_selector1_for_answer_mode();
}

//"->" button - enter to saving answer menu
void callback_key_save_cell_menu(uint16_t key_code)
{
  mode_state = SAVE_MEM_MENU_MODE;
}

void callback_key_ac_menu(uint16_t key_code)
{
  menu_exit();
}

void callback_key_exe_menu(uint16_t key_code)
{
  menu_leave();
}