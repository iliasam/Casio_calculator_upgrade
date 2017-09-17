#include "keys_handling.h"
#include "formula_handling.h"
#include "main.h"
#include <stddef.h>

extern ModeStateType mode_state;
extern FormulaInputStateType formula_input_state;

void callback_key_ac_formula(uint16_t key_code);
void callback_key_left_formula(uint16_t key_code);
void callback_key_right_formula(uint16_t key_code);
void callback_key_del_formula(uint16_t key_code);

void callback_key_alpha_formula(uint16_t key_code);
void callback_key_shift_formula(uint16_t key_code);

void callback_key_exe_formula(uint16_t key_code);

//Structure to hold information about keys and corresponding text
//Some texts ("pi") will be replaced to their symbols during text input
KeyTextType keys_text_array[] =
{
  {1, "0", "Z", NULL},
  {2, ".", "[", NULL},
  {3, "\xB7", "]", "\xB6"},//\xB7 - exp is 10^x, \xB6 - pi
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
  {31, NULL,  "A",  NULL},
  {32, NULL,  "B",  NULL},
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
//It hold pointers to functiona that must be called when button is pressed
KeyFunctionalType keys_functional_array[] = 
{
  {25, callback_key_del_formula,   NULL},//DEL
  {47, callback_key_right_formula, NULL},//Right
  {48, callback_key_left_formula,  NULL},//Left
  {52, NULL, NULL},//Down
  {54, NULL, NULL},//Up
  {55, callback_key_shift_formula, NULL},//Shift
  {56, callback_key_alpha_formula, NULL},//Alpha
  {60, NULL, NULL},//MODE
  {62, callback_key_exe_formula, NULL},//EXE
  {64, callback_key_ac_formula, NULL},//AC
};

#define KEYS_TEXT_ARRAY_COUNT (sizeof(keys_text_array) / sizeof(KeyTextType))
#define KEYS_FUNCT_ARRAY_COUNT (sizeof(keys_functional_array) / sizeof(KeyFunctionalType))

void process_key_state(uint16_t new_key_state)
{
  static uint16_t prev_key_state = 0;
  
  if ((new_key_state != 0) && (new_key_state != prev_key_state))
  {
    prev_key_state = new_key_state;
    
    if (process_functional_keys(new_key_state) > 0)
      return;//button was processed
    
    if (mode_state == FORMULA_INPUT)
    {
      if (formula_add_new_keys_symbol(new_key_state) > 0)//test was added
      {
        if (formula_input_state != INPUT_MODE_BASIC)
          formula_input_state = INPUT_MODE_BASIC;//switch to basic input mode after adding text
      }
      return;
    }
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
    }
  }
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
      if ((mode_state == MENU_MODE) && (keys_functional_array[i].formula_callback  != NULL))
      {
        keys_functional_array[i].menu_callback(key_code);
        return 1;
      }
    }
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

void callback_key_exe_formula(uint16_t key_code)
{
  solve_formula();
}