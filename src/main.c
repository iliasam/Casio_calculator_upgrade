#include "stm32f10x.h"
#include "config_periph.h"
#include "ST7565R.h"
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "math.h"
#include "stm32f10x_pwr.h"
#include "keys_functions.h"
#include "delay_us_timer.h"


void Delay_ms(uint32_t ms);
volatile double test;

extern volatile uint16_t tmp_cnt;

int main()
{
  char str[32];
  uint16_t cnt = 0;
  
  init_all_hardware();
  initialize_keys_gpio();
  dwt_init();
  
  lcd_init_pins();//инициализация выводв LCD
  Delay_ms(100);
  
  lcd_init();			//	инициализация LCD
  Delay_ms(100);
  lcd_clear();		//	очистка дисплея

  lcd_gotoxy(0,0);
  lcd_puts("TEST1");
  lcd_gotoxy(0,1);
  lcd_puts("TEST2");
  
  lcd_gotoxy(0,2);
  lcd_puts("TEST3");
  lcd_gotoxy(0,3);
  lcd_puts("TEST4");
  lcd_gotoxy(0,4);
  lcd_puts("TEST5");
  lcd_gotoxy(0,5);
  lcd_puts("TEST6");
  lcd_gotoxy(0,6);
  lcd_puts("TEST7");
  lcd_gotoxy(0,7);
  //lcd_puts("123456789012345");
  test = sin(2.0);
  sprintf(str,"sin: %.10g", test);
  lcd_puts(str);
   
   
  Delay_ms(1000);	//	секундная задержка
  
  while(1)			//	бесконечный цикл
  {
    //delay_ms(1000);	//	секундная задержка
    lcd_gotoxy(0,1);
    sprintf(str,"CNT: %d\n", cnt);
    lcd_puts(str);
    
    lcd_gotoxy(0,2);
    double tmp = measure_battery_voltage();
    sprintf(str,"BAT: %.2g", tmp);
    lcd_puts(str);
    
    lcd_gotoxy(0,3);
    sprintf(str,"test: %d", tmp_cnt);
    lcd_puts(str);
    
    uint16_t key_state = keys_polling();
    lcd_gotoxy(0,4);
    sprintf(str,"key: %d    ", key_state);
    lcd_puts(str);
    
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