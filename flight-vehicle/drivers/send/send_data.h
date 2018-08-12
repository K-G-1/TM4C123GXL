#ifndef __send_data_H
#define __send_data_H	 

#include "Algorithm_math.h"
#define BYTE0(dwTemp)       (*(char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))
	

extern float roll_old,pitch_old;
void sand_2402_data(s16 a_off_x,s16 a_off_y,s16 a_off_z,s16 g_off_x,s16 g_off_y,s16 g_off_z);
void sand_ACC_GYRO_data(void);
void sand_IMU_data(void);
void sand_RC_data(void);
void sand_Motor_data(void);
void sand_PID_shell_data(void);
void sand_PID_core_data(void);


void sand_F1_origin(void);
void sand_F2_origin(void);
void Data_Receive_Prepare(u8 data);
void Data_Receive_Anl(u8 *data_buf,u8 num);

#endif




