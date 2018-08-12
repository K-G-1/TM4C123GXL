#ifndef __timer_config_H
#define __timer_config_H

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

void systick_init(void);
void Timer_1_A_init(void);


void Timer1AIntHandler(void);

#endif

