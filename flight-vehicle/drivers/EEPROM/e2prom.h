#ifndef __e2prom_H
#define __e2prom_H


/***************************************************/
#define save_gyro_x           0x30
#define save_gyro_y           0x34
#define save_gyro_z           0x38
 
#define save_acc_x            0x3c
#define save_acc_y            0x40
#define save_acc_z            0x44

#define save_5883_x           0x48
#define save_5883_y           0x4c
#define save_5883_z           0x50

#define Shell_Pitch_P         0x60
#define Shell_Pitch_I         0x62
#define Shell_Pitch_D         0x64

#define Shell_Roll_P          0x68
#define Shell_Roll_I          0x6a
#define Shell_Roll_D          0x6c

#define Shell_Yaw_P           0x6e
#define Shell_Yaw_I           0x70
#define Shell_Yaw_D           0x72

#define Core_Pitch_P          0x74
#define Core_Pitch_I          0x76
#define Core_Pitch_D          0x78

#define Core_Roll_P           0x7a
#define Core_Roll_I           0x7c
#define Core_Roll_D           0x7e

#define Core_Yaw_P            0x80
#define Core_Yaw_I            0x82
#define Core_Yaw_D            0x84

#define save_5883_max_x       0x8A
#define save_5883_max_y       0x8B
#define save_5883_max_z       0x8C
#define save_5883_min_x       0x8D
#define save_5883_min_y       0x8E
#define save_5883_min_z       0x8F

#define Hight_Rate_P          0x92
#define Hight_Rate_I          0x94
#define Hight_Rate_D          0x96

#define Hight_P       	      0x98
#define Hight_I        		  0x9A
#define Hight_D        		  0x9C

#define CCD_P         		  0xA2
#define CCD_I         		  0xA4
#define CCD_D         		  0xA6

#define ctrl4_P          	  0xA8
#define ctrl4_I         	  0xAA
#define ctrl4_D         	  0xAC

#define Forward_time_P            0xB2
#define Forward_time_I         	  0xB4
#define Forward_time_D         	  0xB6

#define Left_time_P          	  0xB8
#define Left_time_I         	  0xBA
#define Left_time_D         	  0xBC

#define Exp_Forward          	  0x22
#define Exp_Left         	  0x24
#define Exp_Back         	  0x26



void e2prom_init(void);
void save_Acc_Gyro_offest(void);
void read_Acc_Gyro_offest(void);

#endif


