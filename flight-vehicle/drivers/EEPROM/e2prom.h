#ifndef __e2prom_H
#define __e2prom_H


/***************************************************/
#define save_gyro_x           0x10
#define save_gyro_y           0x14
#define save_gyro_z           0x18
 
#define save_acc_x            0x1c
#define save_acc_y            0x20
#define save_acc_z            0x24

#define save_5883_x           0x28
#define save_5883_y           0x2c
#define save_5883_z           0x30

#define Shell_Pitch_P         0x40
#define Shell_Pitch_I         0x44
#define Shell_Pitch_D         0x48

#define Shell_Roll_P          0x4c
#define Shell_Roll_I          0x50
#define Shell_Roll_D          0x54

#define Shell_Yaw_P           0x58
#define Shell_Yaw_I           0x5c
#define Shell_Yaw_D           0x60

#define Core_Pitch_P          0x64
#define Core_Pitch_I          0x68
#define Core_Pitch_D          0x6c

#define Core_Roll_P           0x70
#define Core_Roll_I           0x74
#define Core_Roll_D           0x78

#define Core_Yaw_P            0x7c
#define Core_Yaw_I            0x80
#define Core_Yaw_D            0x84

#define save_5883_max_x       0x8C
#define save_5883_max_y       0x90
#define save_5883_max_z       0x94
#define save_5883_min_x       0x9c
#define save_5883_min_y       0xA0
#define save_5883_min_z       0xA4

#define Hight_Rate_P          0xA8
#define Hight_Rate_I          0xAC
#define Hight_Rate_D          0xB0

#define Hight_Position_P      0xB4
#define Hight_Position_I      0xB8
#define Hight_Position_D      0xBC

#define CCD_P         		  0xC0
#define CCD_I         		  0xC4
#define CCD_D         		  0xC8

#define ctrl4_P          	  0xCC
#define ctrl4_I         	  0xD0
#define ctrl4_D         	  0xD4

#define Forward_time_P        0xD8
#define Forward_time_I        0xDC
#define Forward_time_D        0xE0

#define Left_time_P           0xE4
#define Left_time_I           0xE8
#define Left_time_D           0xEC

#define Exp_Forward           0xF0
#define Exp_Left         	  0xF4
#define Exp_Back         	  0xF8



void e2prom_init(void);
void save_Acc_Gyro_offest(void);
void read_Acc_Gyro_offest(void);


void Save_PID_shell(void);
void Save_PID_core(void);
void Save_PID_hight(void);

void Read_PID_shell(void);
void Read_PID_core(void);
void Read_PID_hight(void);
    
#endif


