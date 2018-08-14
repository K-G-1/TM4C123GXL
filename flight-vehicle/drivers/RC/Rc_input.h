#ifndef _RC_INPUT_H_
#define _RC_INPUT_H_
#include <stdint.h>

typedef struct int16_rcget{
				int16_t ROLL;
				int16_t PITCH;
				int16_t THROTTLE;
				int16_t YAW;
              int16_t pitch_offset;
              int16_t roll_offset;
              int16_t yaw_offset;
				int16_t AUX1;
				int16_t AUX2;
				int16_t AUX3;
				int16_t AUX4;
}T_RC_DATA;

extern T_RC_DATA Rc_Data;//1000~2000
extern uint32_t Rc_Pwm_In[8];
extern uint8_t fly_mode;
extern uint8_t ARMED;



void RC_init(void);
void time_cap_init(void);
void RC_Data_Refine(void);
#endif



