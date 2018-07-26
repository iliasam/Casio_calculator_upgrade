#include "stm32f10x.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "config_periph.h"
#include "delay_us_timer.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_dbgmcu.h"
#include "misc.h"

extern uint16_t APD_temperature_raw;

void init_all_hardware(void)
{
  RCC_ClocksTypeDef RCC_Clocks;
  
  init_sys_clock();
  rtc_init();
  init_adc_single_measure();
  RCC_GetClocksFreq (&RCC_Clocks);
  if (SysTick_Config (RCC_Clocks.SYSCLK_Frequency/1000)) { /* Setup SysTick for 1 msec interrupts */;while (1);}
  
  dwt_init();
  init_gpio();
  
  DBGMCU_Config(DBGMCU_STOP | DBGMCU_SLEEP, ENABLE);
}

void init_gpio(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
   
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
  
  //Отключаем JTAG. Переключения выводов порта JTAG в обычный режим ввода-вывода
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin   = BACKLIGHT_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_Init(BACKLIGHT_PORT, &GPIO_InitStructure);
  GPIO_ResetBits(BACKLIGHT_PORT, BACKLIGHT_PIN);
  GPIO_SetBits(BACKLIGHT_PORT, BACKLIGHT_PIN);
  
  GPIO_InitStructure.GPIO_Pin   = BATT_ADC_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
  GPIO_Init(BATT_ADC_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin   = BATT_GND_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
  GPIO_Init(BATT_GND_PORT, &GPIO_InitStructure);
}


void init_sys_clock(void)
{ 
  //Настраиваем систему тактирования
  RCC_DeInit(); /* RCC system reset(for debug purpose) */

  FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);/* Enable Prefetch Buffer */
  FLASH_SetLatency(FLASH_Latency_2);/* Flash 2 wait state */
  
  //Configure for LSI + PLL
  RCC_HCLKConfig(RCC_SYSCLK_Div1);/* HCLK = SYSCLK */
  RCC_PCLK2Config(RCC_HCLK_Div1);/* PCLK2 = HCLK */
  RCC_PCLK1Config(RCC_HCLK_Div2);
  //RCC_PREDIV1Config(RCC_PREDIV1_Source_HSE, RCC_PREDIV1_Div1);
  RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_16);/* PLLCLK = 8MHz/2 * 16 = 64 MHz <<<<<<<<<<<<<<<<<<<<<<<<<<*/
  RCC_PLLCmd(ENABLE);/* Enable PLL */
  while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET){};/* Wait till PLL is ready */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);/* Select PLL as system clock source */
  while (RCC_GetSYSCLKSource() != 0x08){}/* Wait till PLL is used as system clock source */
}


//init ADC for single software triggered measure
void init_adc_single_measure(void)
{
  ADC_InitTypeDef  ADC_InitStructure;
  
  RCC_ADCCLKConfig(RCC_PCLK2_Div2);//12 mhz
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1|RCC_APB2Periph_AFIO,ENABLE);
  
  ADC_DeInit(ADC1);
  ADC_StructInit(&ADC_InitStructure);

  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  ADC_DMACmd(ADC1, DISABLE);  
  ADC_Cmd(ADC1, ENABLE);
  
  ADC_ResetCalibration(ADC1);
  while(ADC_GetResetCalibrationStatus(ADC1)){}; 
  ADC_StartCalibration(ADC1);   
  while(ADC_GetCalibrationStatus(ADC1)){};
}


uint16_t readADC1(uint8_t channel)
{
  ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_41Cycles5);
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
  return ADC_GetConversionValue(ADC1);
}


//do single measurements
void do_single_adc_measurements(void)
{
  init_adc_single_measure();
  //APD_temperature_raw = readADC1(ADC_TEMP_CHANNEL);
}

float measure_battery_voltage(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin   = BATT_GND_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_Init(BATT_GND_PORT, &GPIO_InitStructure);
  GPIO_ResetBits(BATT_GND_PORT, BATT_GND_PIN);//pull  low
  
  float result = (float)readADC1(ADC_Channel_7) / 620.0f;
  
  GPIO_InitStructure.GPIO_Pin   = BATT_GND_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
  GPIO_Init(BATT_GND_PORT, &GPIO_InitStructure);
  
  return result;
}

//RTC используетсядля того, чтобы выводить контроллер из сна
void rtc_init(void)
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
  PWR_BackupAccessCmd(ENABLE);
  RCC_LSICmd(ENABLE);
  
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);  
  RCC_RTCCLKCmd(ENABLE); 
  RTC_WaitForSynchro();
  RTC_WaitForLastTask();
  RTC_SetPrescaler(40-1);//40khz / 40 = 1000 hz -> 1 ms
  RTC_WaitForLastTask();
  
  RTC_ITConfig(RTC_IT_ALR, ENABLE);
  RTC_WaitForLastTask();
  
  RTC_ClearFlag(RTC_FLAG_ALR);
  while(RTC_GetFlagStatus(RTC_FLAG_SEC) == RESET);
}

//функция ухода в спящий режим
void go_to_sleep_mode(void)
{
  RTC_SetAlarm(RTC_GetCounter() + 100);//100 ms
  RTC_WaitForLastTask();
  
  RTC_ITConfig(RTC_IT_ALR, ENABLE);
  
  EXTI_Configuration(ENABLE);
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel                    = RTCAlarm_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority         = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd                 = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  RTC_ClearFlag(RTC_IT_ALR);
  RTC_ClearITPendingBit(RTC_IT_ALR);
  EXTI_ClearITPendingBit(EXTI_Line17);//EXTI_Line17 - RTC alarm

  PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);
  //here program is running again
  
  EXTI_Configuration(DISABLE);
  init_sys_clock();
}

void EXTI_Configuration(FunctionalState state)
{
  EXTI_InitTypeDef EXTI_InitStructure;

  EXTI_ClearITPendingBit(EXTI_Line17);
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Line = EXTI_Line17;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = state;
  EXTI_Init(&EXTI_InitStructure);
}

