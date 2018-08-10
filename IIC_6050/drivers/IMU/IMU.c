#include "IMU.h"
#include "math.h"
#include "mpu6050.h"
#include "hmc5883l.h"
#include "Algorithm_math.h"
#include "Algorithm_filter.h"

struct _angle angle;

#define KALMAN_Q        0.012
#define KALMAN_R        81.0000
double ACC_KALMAN_X  =1;
double ACC_KALMAN_Y  =1;
double ACC_KALMAN_Z  =1;

//void Prepare_Data()
//{
//    READ_MPU6050_ACCEL();
//    READ_MPU6050_GYRO();
////    READ_MPU9250_ACCEL();
////    READ_MPU9250_GYRO();
////    READ_MPU9250_MAG();
//    
//    sensor.acc.temp.x = sensor.acc.origin.x - sensor.acc.quiet.x;
//    sensor.acc.temp.y = sensor.acc.origin.y - sensor.acc.quiet.y;
//    sensor.acc.temp.z = sensor.acc.origin.z - sensor.acc.quiet.z;
//    
//    sensor.gyro.origin.x = (sensor.gyro.origin.x - sensor.gyro.quiet.x);
//    sensor.gyro.origin.y = (sensor.gyro.origin.y - sensor.gyro.quiet.y);
//    sensor.gyro.origin.z = (sensor.gyro.origin.z - sensor.gyro.quiet.z);
//    
//    sensor.gyro.averag.x = LPF_1st(sensor.gyro.averag.x ,sensor.gyro.origin.x,0.1f);
//    sensor.gyro.averag.y = LPF_1st(sensor.gyro.averag.y ,sensor.gyro.origin.y,0.1f);
//    sensor.gyro.averag.z = LPF_1st(sensor.gyro.averag.z ,sensor.gyro.origin.z,0.05f);
//    
//    sensor.acc.averag.x = KalmanFilter(sensor.acc.temp.x,KALMAN_Q,KALMAN_R,sensor.acc.averag.x,ACC_KALMAN_X);
//    sensor.acc.averag.y = KalmanFilter(sensor.acc.temp.y,KALMAN_Q,KALMAN_R,sensor.acc.averag.y,ACC_KALMAN_Y);
//    sensor.acc.averag.z = KalmanFilter(sensor.acc.temp.z,KALMAN_Q,KALMAN_R*2,sensor.acc.averag.z,ACC_KALMAN_Z);
//    
//    
//    sensor.acc.radian.x = sensor.acc.averag.x ;
//    sensor.acc.radian.y = sensor.acc.averag.y ;
//    sensor.acc.radian.z = sensor.acc.averag.z ;
//    sensor.gyro.radian.x = ((float)sensor.gyro.averag.x) *GYRO_GR;
//    sensor.gyro.radian.y = ((float)sensor.gyro.averag.y) *GYRO_GR;
//    sensor.gyro.radian.z = ((float)sensor.gyro.averag.z) *GYRO_GR;
//}


float Kp= 100.0f;
float Ki= 0.002f;
//#define Kp 0.5f                        // ��������֧��������accelerometer/magnetometer  
//#define Ki 0.002f                     // ��������֧��ִ�����������ǵ��ν�gyroscopeases  //KP,KI��Ҫ����
#define halfT 0.010f                 // �������ڵ�һ��  ������ 2.5MS �ɼ�һ��  ���� halfT��1.25MS

/**************************************
 * ��������Get_Attitude
 * ����  ���õ���ǰ��̬
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 *************************************/
void Get_Attitude(void)
{
	Prepare_Data();
	
//	IMUupdate(  sensor.gyro.radian.x,
//                sensor.gyro.radian.y,
//                sensor.gyro.radian.z,
//                sensor.acc.averag.x,
//                sensor.acc.averag.y,
//                sensor.acc.averag.z,
//                Magn_x,
//                Magn_y,
//                Magn_z
//            );	
    IMUupdate1(  sensor.gyro.averag.x,
                sensor.gyro.averag.y,
                sensor.gyro.averag.z,
                sensor.acc.averag.x,
                sensor.acc.averag.y,
                sensor.acc.averag.z
            );	

}

float q0 = 1, q1 = 0, q2 = 0, q3 = 0;    //��Ԫ����Ԫ�أ�������Ʒ���
float exInt = 0, eyInt = 0, ezInt = 0;    // ��������С�������

void IMUupdate1(float gx, float gy, float gz, float ax, float ay, float az)
{
  float norm;
//  float hx, hy, hz, bx, bz;
  float vx, vy, vz;// wx, wy, wz;
  float ex, ey, ez;

  // �Ȱ���Щ�õõ���ֵ���
  float q0q0 = q0*q0;
  float q0q1 = q0*q1;
  float q0q2 = q0*q2;
//  float q0q3 = q0*q3;
  float q1q1 = q1*q1;
//  float q1q2 = q1*q2;
  float q1q3 = q1*q3;
  float q2q2 = q2*q2;
  float q2q3 = q2*q3;
  float q3q3 = q3*q3;
	
	if(ax*ay*az==0)
 		return;
		
  norm = sqrt(ax*ax + ay*ay + az*az);       //acc���ݹ�һ��
  ax = ax /norm;
  ay = ay / norm;
  az = az / norm;

  // estimated direction of gravity and flux (v and w)              �����������������/��Ǩ
  vx = 2*(q1q3 - q0q2);												//��Ԫ����xyz�ı�ʾ
  vy = 2*(q0q1 + q2q3);
  vz = q0q0 - q1q1 - q2q2 + q3q3 ;

  // error is sum of cross product between reference direction of fields and direction measured by sensors
  ex = (ay*vz - az*vy) ;                           					 //�������������õ���־������
  ey = (az*vx - ax*vz) ;
  ez = (ax*vy - ay*vx) ;

  exInt = exInt + ex * Ki;								  //�������л���
  eyInt = eyInt + ey * Ki;
  ezInt = ezInt + ez * Ki;

  // adjusted gyroscope measurements
  gx = gx + Kp*ex + exInt;					   						//�����PI�󲹳��������ǣ����������Ư��
  gy = gy + Kp*ey + eyInt;
  gz = gz + Kp*ez + ezInt;				   							//�����gz����û�й۲��߽��н��������Ư�ƣ����ֳ����ľ��ǻ����������Լ�

  // integrate quaternion rate and normalise			//��Ԫ�ص�΢�ַ���
  q0 = q0 + (-q1*gx - q2*gy - q3*gz)*halfT;
  q1 = q1 + ( q0*gx + q2*gz - q3*gy)*halfT;
  q2 = q2 + ( q0*gy - q1*gz + q3*gx)*halfT;
  q3 = q3 + ( q0*gz + q1*gy - q2*gx)*halfT;

  // normalise quaternion
  norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
  q0 = q0 / norm;
  q1 = q1 / norm;
  q2 = q2 / norm;
  q3 = q3 / norm;

  angle.yaw = atan2(2 * q1 * q2 + 2 * q0 * q3, -2 * q2*q2 - 2 * q3* q3 + 1)* 57.3; // yaw
  angle.pitch = asin(-2 * q1 * q3 + 2 * q0* q2)* 57.3; // pitch
  angle.roll = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3; // roll
}



float invSqrt(float x) {
	float halfx = 0.5f * x;
	float y = x;
	long i = *(long*)&y;
	i = 0x5f3759df - (i>>1);
	y = *(float*)&i;
	y = y * (1.5f - (halfx * y * y));
	return y;
}
void IMUupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz)
{
    volatile float norm;
  volatile float hx, hy, hz, bx, bz;
  volatile float vx, vy, vz, wx, wy, wz;
  volatile float ex, ey, ez;
  float temp0,temp1,temp2,temp3;
  float temp;
	float Xr,Yr;
  // ?????????
  float q0q0 = q0*q0;
  float q0q1 = q0*q1;
  float q0q2 = q0*q2;
  float q0q3 = q0*q3;
  float q1q1 = q1*q1;
  float q1q2 = q1*q2;
  float q1q3 = q1*q3;
  float q2q2 = q2*q2;   
  float q2q3 = q2*q3;
  float q3q3 = q3*q3;          
  
//  now = micros();  //��ȡʱ��
//  if(now < lastUpdate){ //��ʱ��������ˡ�
//		halfT =  ((float)(now + (0xffffffff- lastUpdate)) / 2000000.0f);	
//		lastUpdate = now;
//		//return ;
//  }
//  else{
//		halfT =  ((float)(now - lastUpdate) / 2000000.0f);
//  	}
//  halftime = halfT;
//  lastUpdate = now;	//����ʱ��

//  temp = sqrt(ax*ax + ay*ay + az*az);
//  temp = (temp / 16384.0f) * 9.8f;   //ת��M/S^2Ϊ��λ�� 
//  acc_vector = acc_vector +   //��ͨ�˲�����ֹƵ��20hz
//  			(halfT*2.0f / (7.9577e-3f + halfT*2.0f)) * (temp - acc_vector);

  norm = invSqrt(ax*ax + ay*ay + az*az);       
  ax = ax * norm;
  ay = ay * norm;
  az = az * norm;

  norm = invSqrt(mx*mx + my*my + mz*mz);          
  mx = mx * norm;
  my = my * norm;
  mz = mz * norm;

  // compute reference direction of flux
  hx = 2*mx*(0.5f - q2q2 - q3q3) + 2*my*(q1q2 - q0q3) + 2*mz*(q1q3 + q0q2);
  hy = 2*mx*(q1q2 + q0q3) + 2*my*(0.5f - q1q1 - q3q3) + 2*mz*(q2q3 - q0q1);
  hz = 2*mx*(q1q3 - q0q2) + 2*my*(q2q3 + q0q1) + 2*mz*(0.5f - q1q1 - q2q2);         
  bx = sqrt((hx*hx) + (hy*hy));
  bz = hz;     
  
  // estimated direction of gravity and flux (v and w)
  vx = 2*(q1q3 - q0q2);
  vy = 2*(q0q1 + q2q3);
  vz = q0q0 - q1q1 - q2q2 + q3q3;
  wx = 2*bx*(0.5f - q2q2 - q3q3) + 2*bz*(q1q3 - q0q2);
  wy = 2*bx*(q1q2 - q0q3) + 2*bz*(q0q1 + q2q3);
  wz = 2*bx*(q0q2 + q1q3) + 2*bz*(0.5f - q1q1 - q2q2);  
  
  // error is sum of cross product between reference direction of fields and direction measured by sensors
  ex = (ay*vz - az*vy) + (my*wz - mz*wy);
  ey = (az*vx - ax*vz) + (mz*wx - mx*wz);
  ez = (ax*vy - ay*vx) + (mx*wy - my*wx);

if(ex != 0.0f && ey != 0.0f && ez != 0.0f){
  exInt = exInt + ex * Ki * halfT;
  eyInt = eyInt + ey * Ki * halfT;	
  ezInt = ezInt + ez * Ki * halfT;

  // adjusted gyroscope measurements
  gx = gx + (Kp*ex + exInt);
  gy = gy + (Kp*ey + eyInt);
  gz = gz + (Kp*ez + ezInt);

  }

  // integrate quaternion rate and normalise
  temp0 = q0 + (-q1*gx - q2*gy - q3*gz)*halfT;
  temp1 = q1 + (q0*gx + q2*gz - q3*gy)*halfT;
  temp2 = q2 + (q0*gy - q1*gz + q3*gx)*halfT;
  temp3 = q3 + (q0*gz + q1*gy - q2*gx)*halfT;  
  
  // normalise quaternion
  norm = invSqrt(temp0*temp0 + temp1*temp1 + temp2*temp2 + temp3*temp3);
  q0 = temp0 * norm;
  q1 = temp1 * norm;
  q2 = temp2 * norm;
  q3 = temp3 * norm;
	

  angle.roll = atan2(2*q2*q3 + 2*q0*q1, -2*q1*q1 - 2*q2*q2 + 1) ; // roll
	angle.pitch = asin(-2*q1*q3 + 2*q0*q2) ; // pitch
	
	angle.yaw= atan2(2 * q1 * q2 + 2 * q0 * q3, -2 * q2*q2 - 2 * q3 * q3 + 1);
	
//	angle.roll= LPF_1st(old_roll ,angle.roll ,0.8f);
//	old_roll= angle.roll;
//	 
//	angle.pitch= LPF_1st(old_pitch ,angle.pitch ,0.8f);
//	old_pitch= angle.pitch;
	
	
//		if(tempyaw>0)
//		angle.yaw=tempyaw;
//	else
//		angle.yaw=360+tempyaw;
//	//          ���ڵش���ν�����ǲ���                       //    
//	//�ο�  http://baike.baidu.com/view/1239157.htm?fr=aladdin    //

//	Xr = mx * COS(angle.pitch*RtA) + my * SIN(-angle.pitch*RtA) * SIN(-angle.roll*RtA) - mz * COS(angle.roll*RtA) * SIN(-angle.pitch*RtA);
//	Yr = my * COS(angle.roll*RtA) + mz * SIN(-angle.roll*RtA);
//	
//	angle.yaw = atan2((double)Yr,(double)Xr) ; // yaw 
	angle.roll *= RtA;
	angle.pitch *= RtA;
	angle.yaw*= RtA;




}











