#ifndef _CONFIG_PEIRIPH_H
#define _CONFIG_PEIRIPH_H



#define BACKLIGHT_PIN           GPIO_Pin_10//tim2_ch3
#define BACKLIGHT_PORT          GPIOB

#define BATT_ADC_PIN            GPIO_Pin_7
#define BATT_ADC_PORT           GPIOA

#define BATT_GND_PIN            GPIO_Pin_6
#define BATT_GND_PORT           GPIOA

void init_all_hardware(void);
void init_gpio(void);
void init_sys_clock(void);

void init_adc_single_measure(void);

double measure_battery_voltage(void);
void rtc_init(void);
void go_to_sleep_mode(void);
void EXTI_Configuration(FunctionalState state);



#endif
