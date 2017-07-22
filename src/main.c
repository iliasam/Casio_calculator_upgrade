#include "stm32f10x.h"
#include "config_periph.h"
#include "ST7565R.h"
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "stm32f10x_pwr.h"
#include "keys_functions.h"
#include "keys_handling.h"
#include "delay_us_timer.h"
#include "lcd_worker.h"
#include "text_functions.h"
#include "display_handling.h"
#include "formula_handling.h"
#include "str_math.h"

void Delay_ms(uint32_t ms);
volatile double test;

extern volatile uint16_t tmp_cnt;

ModeStateType mode_state = FORMULA_INPUT;
FormulaInputStateType formula_input_state = INPUT_MODE_BASIC;
//FormulaInputStateType formula_input_state = INPUT_MODE_SHIFT;

extern char formula_text[FORMULA_MAX_LENGTH];
extern uint16_t formula_current_length;

int main()
{
  char str[32];
  memset(str, 0, 32);
  uint16_t cnt = 0;
  
  init_all_hardware();
  initialize_keys_gpio();
  dwt_init();
  
  lcd_init_pins();//инициализация выводв LCD
  Delay_ms(100);
  
  lcd_init();			//	инициализация LCD
  Delay_ms(100);
  
  lcd_full_clear();
  /*
  lcd_draw_string_cur("TEST1", FONT_SIZE_8, LCD_NEW_LINE_FLAG);
  lcd_draw_string_cur("TEST2", FONT_SIZE_8, LCD_NEW_LINE_FLAG);
  lcd_draw_string_cur("TEST3", FONT_SIZE_6, LCD_NEW_LINE_FLAG);
  lcd_draw_string_cur("TEST4", FONT_SIZE_11, LCD_NEW_LINE_FLAG);
  
  test = sin(2.0);
  sprintf(str,"sin: %.10g", test);
  lcd_draw_string_cur(str, FONT_SIZE_8, LCD_NEW_LINE_FLAG);
*/

  while(1)			//	бесконечный цикл
  {
    lcd_clear_framebuffer();
    
    display_draw_input_handler();
    //lcd_draw_char(182, 0, 0, FONT_SIZE_6, 0);
    
    //delay_ms(1000);	//	секундная задержка
    sprintf(str,"CNT: %d\n", cnt);
    lcd_draw_string(str, 0, FONT_SIZE_8*4, FONT_SIZE_8, 0);
    
    double tmp = measure_battery_voltage();
    sprintf(str,"BAT: %.2g", tmp);
    lcd_draw_string(str, 0, FONT_SIZE_8*5, FONT_SIZE_8, 0);
    
    sprintf(str,"test: %d", tmp_cnt);
    lcd_draw_string(str, 0, FONT_SIZE_8*6, FONT_SIZE_8, 0);
    
    uint16_t key_state = keys_polling();
    process_key_state(key_state);
    lcd_gotoxy(0,4);
    sprintf(str,"key: %d    ", key_state);
    lcd_draw_string(str, 0, FONT_SIZE_8*7, FONT_SIZE_8, 0);
    lcd_update();
    
   // go_to_sleep_mode();
    
    cnt++;
  }
}


int putchar(int c)
 {
   // Write a character to the USART

  //Loop until the end of transmission   
  return c;    
}


void Delay_ms(uint32_t ms)
{
  volatile uint32_t nCount;
  RCC_ClocksTypeDef RCC_Clocks;
  RCC_GetClocksFreq (&RCC_Clocks);
  nCount=(RCC_Clocks.HCLK_Frequency/10000)*ms;
  for (; nCount!=0; nCount--);
}

void solve_formula(void)
{
  solve((uint8_t*)formula_text, formula_current_length);
}