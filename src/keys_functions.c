#include "keys_functions.h"
#include "stm32f10x_gpio.h"
#include "delay_us_timer.h"

//COMx
uint16_t row_pins[] = 
{
    GPIO_Pin_11,//com1
    GPIO_Pin_12,//com2
    GPIO_Pin_3, //com3
    GPIO_Pin_15,//com4
    GPIO_Pin_9,//com5
    GPIO_Pin_8,//com6
    GPIO_Pin_1,//com7
};

GPIO_TypeDef* row_gpio[] = 
{
    GPIOB,//com1
    GPIOB,//com2
    GPIOA,//com3
    GPIOA,//com4
    GPIOB,//com5
    GPIOB,//com6
    GPIOA,//com7
};

//KEY_LINEx
uint16_t column_pins[] = 
{
    GPIO_Pin_15,//key1
    GPIO_Pin_9, //key2
    GPIO_Pin_7, //key3
    GPIO_Pin_14,//key4
    GPIO_Pin_13,//key5
    GPIO_Pin_2, //key6
    GPIO_Pin_14,//key7
    GPIO_Pin_15,//key8
    GPIO_Pin_6, //key9
};

GPIO_TypeDef* column_gpio[] = 
{
    GPIOB,//key1
    GPIOA,//key2
    GPIOB,//key3
    GPIOB,//key4
    GPIOB,//key5
    GPIOB,//key6
    GPIOC,//key7
    GPIOC,//key8
    GPIOB,//key9
};

#define KEYS_ROWS_COUNT (sizeof(row_pins) / sizeof(uint16_t))
#define KEYS_COLUMNS_COUNT (sizeof(column_pins) / sizeof(uint16_t))

void initialize_keys_gpio(void)
{
  uint8_t i;
  
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  
  //Init column pins as inputs with pull-up
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
  
  for (i=0; i<KEYS_COLUMNS_COUNT; i++)
  {
    GPIO_InitStructure.GPIO_Pin = column_pins[i];
    GPIO_Init(column_gpio[i], &GPIO_InitStructure);
  }
  
  //Init row pins as open-drain outputs
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;
  for (i=0; i<KEYS_ROWS_COUNT; i++)
  {
    GPIO_InitStructure.GPIO_Pin = row_pins[i];
    GPIO_Init(row_gpio[i], &GPIO_InitStructure);
    GPIO_SetBits(row_gpio[i], row_pins[i]);//disable open-drain pull down
  }
}

uint16_t keys_polling(void)
{
  uint8_t row_pos = 0;
  
  for (row_pos = 0; row_pos < KEYS_ROWS_COUNT; row_pos++)
  {
    row_gpio[row_pos]->ODR&=  ~row_pins[row_pos];//pull-down line
    dwt_delay(10);
    uint8_t key_state = check_column_keys();
    row_gpio[row_pos]->ODR|=  row_pins[row_pos];//disable open-drain pull down
    dwt_delay(10);
    if (key_state > 0)
      return (row_pos*KEYS_COLUMNS_COUNT + key_state);
  }
  return 0;
}

//Check if some key in column is pressed
//Return position number of this kwy
uint8_t check_column_keys(void)
{
  uint8_t col_pos = 0;
  
  for (col_pos=0; col_pos < KEYS_COLUMNS_COUNT; col_pos++)
  {
    if ((column_gpio[col_pos]->IDR & column_pins[col_pos]) == 0)
      return (col_pos + 1);
  }
  
  return 0;
}
