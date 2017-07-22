#ifndef _KEYS_FUNCTIONS_H
#define _KEYS_FUNCTIONS_H
#include "stdint.h"

#define KEY_AC_PIN      GPIO_Pin_0
#define KEY_AC_GPIO     GPIOA

void initialize_keys_gpio(void);
uint8_t check_column_keys(void);
uint16_t keys_polling(void);

#endif
