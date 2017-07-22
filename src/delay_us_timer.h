#ifndef __DELAY_US_TIMER
#define __DELAY_US_TIMER

#include "stdint.h"

#define F_CPU 48000000

void dwt_init(void);
static inline uint32_t dwt_dt(uint32_t t0, uint32_t t1);
void dwt_delay(uint32_t us);
uint32_t get_dwt_value(void);

#endif

