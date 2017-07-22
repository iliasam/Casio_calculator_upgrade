#ifndef __KEYS_HANDLING_H
#define __KEYS_HANDLING_H
#include "stdint.h"

//key_code is send for universal functions (some keys can have single callback)
typedef void (*key_callback_t)(uint16_t key_code);

//Structure to hold information about keys and corresponding text
typedef struct
{
  uint16_t      code;
  const char    *basic_text;//This text added to formula when key pressed
  const char    *alpha_text;//This text added to formula when key pressed in ALPHA mode
  const char    *shift_text;//This text added to formula when key pressed in SHIFT mode
} KeyTextType;

//Structure to hold information about functional keys  (ALPHA, SHIFT, DEL, AC and so on)
typedef struct
{
  uint16_t              code;//key code
  key_callback_t        formula_callback;//called in formula mode
  key_callback_t        menu_callback;//called in menu mode

} KeyFunctionalType;

int8_t process_functional_keys(uint16_t key_code);
void process_key_state(uint16_t new_key_state);
const char* get_text_from_key_code(uint16_t key_code);

#endif
