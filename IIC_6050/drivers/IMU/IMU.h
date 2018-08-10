#ifndef __IMU_H
#define	__IMU_H



#define RtA 		57.324841f		//  180/3.1415  ����ת��Ϊ�Ƕ�		
#define AtR    	0.0174533f		//  1/RtA             RtA����		


struct _angle{
        float pitch;
		float roll;
        float yaw;};


extern struct _angle angle;

void Prepare_Data(void);
void Get_Attitude(void);
void IMUupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);
void IMUupdate1(float gx, float gy, float gz, float ax, float ay, float az);

#endif













