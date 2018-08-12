#include "e2prom.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/eeprom.h"

#include "mpu6050.h"

void e2prom_init(void)
{
    //
    // Enable the EEPROM module.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
    //
    // Wait for the EEPROM module to be ready.
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_EEPROM0))
    { }
    //
    // Wait for the EEPROM Initialization to complete
    //
    //
    // Check if the EEPROM Initialization returned an error
    // and inform the application
    //
    if(EEPROMInit() != EEPROM_INIT_OK)
    {
        while(1)
        { }
    }
}


//*************************************************************************//

void save_Acc_Gyro_offest(void)
{
    uint32_t temp;
    
    temp = sensor.gyro.quiet.x ;
    EEPROMProgram(&temp,save_gyro_x,sizeof(temp));
    temp = sensor.gyro.quiet.y ;
    EEPROMProgram(&temp,save_gyro_y,sizeof(temp));
    temp = sensor.gyro.quiet.z ;
    EEPROMProgram(&temp,save_gyro_z,sizeof(temp));
    
    temp = sensor.acc.quiet.x ;
    EEPROMProgram(&temp,save_acc_x,sizeof(temp));
    temp = sensor.acc.quiet.y ;
    EEPROMProgram(&temp,save_acc_y,sizeof(temp));
    temp = sensor.acc.quiet.z ;
    EEPROMProgram(&temp,save_acc_z,sizeof(temp));
}

void AT24cxx_save_5883_offest(void)
{
	
}




//void AT24cxx_save_PID_shell(void)
//{
//	u16 temp_P,temp_I,temp_D;
//	
//	temp_P= ctrl_1.PID[PIDPITCH].kp *1000;
//	temp_I= ctrl_1.PID[PIDPITCH].ki *1000;
//	temp_D= ctrl_1.PID[PIDPITCH].kd *1000;
//	
//	IIC_ADD_writelen_BYTE(Shell_Pitch_P,temp_P,2);
//	IIC_ADD_writelen_BYTE(Shell_Pitch_I,temp_I,2);
//	IIC_ADD_writelen_BYTE(Shell_Pitch_D,temp_D,2);

//	
//	temp_P= ctrl_1.PID[PIDROLL].kp *1000;
//	temp_I= ctrl_1.PID[PIDROLL].ki *1000;
//	temp_D= ctrl_1.PID[PIDROLL].kd *1000;
//	
//	IIC_ADD_writelen_BYTE(Shell_Roll_P,temp_P,2);
//	IIC_ADD_writelen_BYTE(Shell_Roll_I,temp_I,2);
//	IIC_ADD_writelen_BYTE(Shell_Roll_D,temp_D,2);
//	
//	
//	temp_P= ctrl_1.PID[PIDYAW].kp *1000;
//	temp_I= ctrl_1.PID[PIDYAW].ki *1000;
//	temp_D= ctrl_1.PID[PIDYAW].kd *1000;
//	
//	IIC_ADD_writelen_BYTE(Shell_Yaw_P,temp_P,2);
//	IIC_ADD_writelen_BYTE(Shell_Yaw_I,temp_I,2);
//	IIC_ADD_writelen_BYTE(Shell_Yaw_D,temp_D,2);
//	
//	
//}



//void AT24cxx_save_PID_core(void)
//{
//	u16 temp_P,temp_I,temp_D;
//	
//	temp_P= ctrl_2.PID[PIDPITCH].kp *1000;
//	temp_I= ctrl_2.PID[PIDPITCH].ki *1000;
//	temp_D= ctrl_2.PID[PIDPITCH].kd *1000;
//	
//	IIC_ADD_writelen_BYTE(Core_Pitch_P,temp_P,2);
//	IIC_ADD_writelen_BYTE(Core_Pitch_I,temp_I,2);
//	IIC_ADD_writelen_BYTE(Core_Pitch_D,temp_D,2);

//	
//	temp_P= ctrl_2.PID[PIDROLL].kp *1000;
//	temp_I= ctrl_2.PID[PIDROLL].ki *1000;
//	temp_D= ctrl_2.PID[PIDROLL].kd *1000;
//	
//	IIC_ADD_writelen_BYTE(Core_Roll_P,temp_P,2);
//	IIC_ADD_writelen_BYTE(Core_Roll_I,temp_I,2);
//	IIC_ADD_writelen_BYTE(Core_Roll_D,temp_D,2);
//	
//	
//	temp_P= ctrl_2.PID[PIDYAW].kp *1000;
//	temp_I= ctrl_2.PID[PIDYAW].ki *1000;
//	temp_D= ctrl_2.PID[PIDYAW].kd *1000;
//	
//	IIC_ADD_writelen_BYTE(Core_Yaw_P,temp_P,2);
//	IIC_ADD_writelen_BYTE(Core_Yaw_I,temp_I,2);
//	IIC_ADD_writelen_BYTE(Core_Yaw_D,temp_D,2);

//	
//}


//void AT24cxx_save_PID_hight(void)
//{
//	u16 temp_P,temp_I,temp_D;
//	
//	temp_P= pid_setup.groups.hc_height.kp *1000;
//	temp_I= pid_setup.groups.hc_height.ki *1000;
//	temp_D= pid_setup.groups.hc_height.kd *1000;
//	
//	IIC_ADD_writelen_BYTE(Hight_P,temp_P,2);
//	IIC_ADD_writelen_BYTE(Hight_I,temp_I,2);
//	IIC_ADD_writelen_BYTE(Hight_D,temp_D,2);
//	
//	
//}

//void AT24cxx_save_PID_hight_Rate(void)
//{
//	u16 temp_P,temp_I,temp_D;
//	
//	temp_P= pid_setup.groups.hc_sp.kp *1000;
//	temp_I= pid_setup.groups.hc_sp.ki *1000;
//	temp_D= pid_setup.groups.hc_sp.kd *1000;
//	
//	IIC_ADD_writelen_BYTE(Hight_Rate_P,temp_P,2);
//	IIC_ADD_writelen_BYTE(Hight_Rate_I,temp_I,2);
//	IIC_ADD_writelen_BYTE(Hight_Rate_D,temp_D,2);
//	
//	
//}

//void AT24cxx_save_PID_CCD(void)
//{
//	u16 temp_P,temp_I,temp_D;
//	
//	temp_P= pid_setup.groups.ctrl3.kp *1000;
//	temp_I= pid_setup.groups.ctrl3.ki *1000;
//	temp_D= pid_setup.groups.ctrl3.kd *1000;
//	
//	IIC_ADD_writelen_BYTE(CCD_P,temp_P,2);
//	IIC_ADD_writelen_BYTE(CCD_I,temp_I,2);
//	IIC_ADD_writelen_BYTE(CCD_D,temp_D,2);
//	
//	
//}


//void AT24cxx_save_PID_others(void)
//{
//	u16 temp_P,temp_I,temp_D;
//	
//	temp_P= pid_setup.groups.ctrl4.kp *1000;
//	temp_I= pid_setup.groups.ctrl4.ki *1000;
//	temp_D= pid_setup.groups.ctrl4.kd *1000;
//	
//	IIC_ADD_writelen_BYTE(ctrl4_P,temp_P,2);
//	IIC_ADD_writelen_BYTE(ctrl4_I,temp_I,2);
//	IIC_ADD_writelen_BYTE(ctrl4_D,temp_D,2);
//	
//    temp_P= pid_setup.groups.Forward_time.kp *1000;
//	temp_I= pid_setup.groups.Forward_time.ki *1000;
//	temp_D= pid_setup.groups.Forward_time.kd *1000;
//	
//	IIC_ADD_writelen_BYTE(Forward_time_P,temp_P,2);
//	IIC_ADD_writelen_BYTE(Forward_time_I,temp_I,2);
//	IIC_ADD_writelen_BYTE(Forward_time_D,temp_D,2);
//    
//    
//    temp_P= pid_setup.groups.Left_time.kp *1000;
//	temp_I= pid_setup.groups.Left_time.ki *1000;
//	temp_D= pid_setup.groups.Left_time.kd *1000;
//	
//	IIC_ADD_writelen_BYTE(Left_time_P,temp_P,2);
//	IIC_ADD_writelen_BYTE(Left_time_I,temp_I,2);
//	IIC_ADD_writelen_BYTE(Left_time_D,temp_D,2);
//    
//	temp_P= pid_setup.groups.exp_angle.exp_forward *1000;
//	temp_I= pid_setup.groups.exp_angle.exp_left *1000;
//	temp_D= pid_setup.groups.exp_angle.exp_back *1000;
//	
//	IIC_ADD_writelen_BYTE(Exp_Forward,temp_P,2);
//	IIC_ADD_writelen_BYTE(Exp_Left,temp_I,2);
//	IIC_ADD_writelen_BYTE(Exp_Back,temp_D,2);
//}



//*************************************************************************//

void read_Acc_Gyro_offest(void)
{
    uint32_t temp;
    
    EEPROMRead(&temp,save_gyro_x,sizeof(temp));
	sensor.gyro.quiet.x = (int16_t)temp;
    EEPROMRead(&temp,save_gyro_y,sizeof(temp));
	sensor.gyro.quiet.y = (int16_t)temp;
    EEPROMRead(&temp,save_gyro_z,sizeof(temp));
	sensor.gyro.quiet.z = (int16_t)temp;
    
    EEPROMRead(&temp,save_acc_x,sizeof(temp));
	sensor.acc.quiet.x = temp;
    EEPROMRead(&temp,save_acc_y,sizeof(temp));
	sensor.acc.quiet.y = temp;
    EEPROMRead(&temp,save_acc_z,sizeof(temp));
	sensor.acc.quiet.z = temp ;


}

void AT24cxx_read_5883_offest(void)
{
//	ak8975.Mag_Offset.x=IIC_Read_MultiBytes(0xA0,save_5883_x,2);
//	ak8975.Mag_Offset.y=IIC_Read_MultiBytes(0xA0,save_5883_y,2);
//	ak8975.Mag_Offset.z=IIC_Read_MultiBytes(0xA0,save_5883_z,2);


}


//void AT24cxx_read_PID_shell(void)
//{
//	u16 temp_P,temp_I,temp_D;
//	
//	temp_P= IIC_Read_MultiBytes(0xA0,Shell_Pitch_P,2);
//	temp_I= IIC_Read_MultiBytes(0xA0,Shell_Pitch_I,2);
//	temp_D= IIC_Read_MultiBytes(0xA0,Shell_Pitch_D,2);
//	
//	ctrl_1.PID[PIDPITCH].kp= (float)temp_P/1000.0f;
//	ctrl_1.PID[PIDPITCH].ki= (float)temp_I/1000.0f;
//	ctrl_1.PID[PIDPITCH].kd= (float)temp_D/1000.0f;
//	
//	
//	temp_P= IIC_Read_MultiBytes(0xA0,Shell_Roll_P,2);
//	temp_I= IIC_Read_MultiBytes(0xA0,Shell_Roll_I,2);
//	temp_D= IIC_Read_MultiBytes(0xA0,Shell_Roll_D,2);
//	
//	ctrl_1.PID[PIDROLL].kp= (float)temp_P/1000.0f;
//	ctrl_1.PID[PIDROLL].ki= (float)temp_I/1000.0f;
//	ctrl_1.PID[PIDROLL].kd= (float)temp_D/1000.0f;
//	
//	
//	temp_P= IIC_Read_MultiBytes(0xA0,Shell_Yaw_P,2);
//	temp_I= IIC_Read_MultiBytes(0xA0,Shell_Yaw_I,2);
//	temp_D= IIC_Read_MultiBytes(0xA0,Shell_Yaw_D,2);
//	
//	ctrl_1.PID[PIDYAW].kp= (float)temp_P/1000.0f;
//	ctrl_1.PID[PIDYAW].ki= (float)temp_I/1000.0f;
//	ctrl_1.PID[PIDYAW].kd= (float)temp_D/1000.0f;
//	
//}




//void AT24cxx_read_PID_core(void)
//{
//		u16 temp_P,temp_I,temp_D;
//	
//	temp_P= IIC_Read_MultiBytes(0xA0,Core_Pitch_P,2);
//	temp_I= IIC_Read_MultiBytes(0xA0,Core_Pitch_I,2);
//	temp_D= IIC_Read_MultiBytes(0xA0,Core_Pitch_D,2);
//	
//	ctrl_2.PID[PIDPITCH].kp= (float)temp_P/1000.0f;
//	ctrl_2.PID[PIDPITCH].ki= (float)temp_I/1000.0f;
//	ctrl_2.PID[PIDPITCH].kd= (float)temp_D/1000.0f;
//	
//	
//	temp_P= IIC_Read_MultiBytes(0xA0,Core_Roll_P,2);
//	temp_I= IIC_Read_MultiBytes(0xA0,Core_Roll_I,2);
//	temp_D= IIC_Read_MultiBytes(0xA0,Core_Roll_D,2);
//	
//	ctrl_2.PID[PIDROLL].kp= (float)temp_P/1000.0f;
//	ctrl_2.PID[PIDROLL].ki= (float)temp_I/1000.0f;
//	ctrl_2.PID[PIDROLL].kd= (float)temp_D/1000.0f;
//	
//	
//	temp_P= IIC_Read_MultiBytes(0xA0,Core_Yaw_P,2);
//	temp_I= IIC_Read_MultiBytes(0xA0,Core_Yaw_I,2);
//	temp_D= IIC_Read_MultiBytes(0xA0,Core_Yaw_D,2);
//	
//	ctrl_2.PID[PIDYAW].kp= (float)temp_P/1000.0f;
//	ctrl_2.PID[PIDYAW].ki= (float)temp_I/1000.0f;
//	ctrl_2.PID[PIDYAW].kd= (float)temp_D/1000.0f;
//	
//}

//void AT24cxx_read_PID_hight(void)
//{
//		u16 temp_P,temp_I,temp_D;
//	
//	temp_P= IIC_Read_MultiBytes(0xA0,Hight_P,2);
//	temp_I= IIC_Read_MultiBytes(0xA0,Hight_I,2);
//	temp_D= IIC_Read_MultiBytes(0xA0,Hight_D,2);
//	
//	pid_setup.groups.hc_height.kp= (float)temp_P/1000.0f;
//	pid_setup.groups.hc_height.ki= (float)temp_I/1000.0f;
//	pid_setup.groups.hc_height.kd= (float)temp_D/1000.0f;
//	

//	
//}


//void AT24cxx_read_PID_hight_Rate(void)
//{
//		u16 temp_P,temp_I,temp_D;
//	
//	temp_P= IIC_Read_MultiBytes(0xA0,Hight_Rate_P,2);
//	temp_I= IIC_Read_MultiBytes(0xA0,Hight_Rate_I,2);
//	temp_D= IIC_Read_MultiBytes(0xA0,Hight_Rate_D,2);
//	
//	pid_setup.groups.hc_sp.kp= (float)temp_P/1000.0f;
//	pid_setup.groups.hc_sp.ki= (float)temp_I/1000.0f;
//	pid_setup.groups.hc_sp.kd= (float)temp_D/1000.0f;
//	

//	
//}

//void AT24cxx_read_PID_CCD(void)
//{
//		u16 temp_P,temp_I,temp_D;
//	
//	temp_P= IIC_Read_MultiBytes(0xA0,CCD_P,2);
//	temp_I= IIC_Read_MultiBytes(0xA0,CCD_I,2);
//	temp_D= IIC_Read_MultiBytes(0xA0,CCD_D,2);
//	
//	pid_setup.groups.ctrl3.kp= (float)temp_P/1000.0f;
//	pid_setup.groups.ctrl3.ki= (float)temp_I/1000.0f;
//	pid_setup.groups.ctrl3.kd= (float)temp_D/1000.0f;
//	

//	
//}
//void AT24cxx_read_PID_others(void)
//{
//		u16 temp_P,temp_I,temp_D;
//	
//	temp_P= IIC_Read_MultiBytes(0xA0,ctrl4_P,2);
//	temp_I= IIC_Read_MultiBytes(0xA0,ctrl4_I,2);
//	temp_D= IIC_Read_MultiBytes(0xA0,ctrl4_D,2);
//	
//	pid_setup.groups.ctrl4.kp= (float)temp_P/1000.0f;
//	pid_setup.groups.ctrl4.ki= (float)temp_I/1000.0f;
//	pid_setup.groups.ctrl4.kd= (float)temp_D/1000.0f;
//	
//    temp_P= IIC_Read_MultiBytes(0xA0,Forward_time_P,2);
//	temp_I= IIC_Read_MultiBytes(0xA0,Forward_time_I,2);
//	temp_D= IIC_Read_MultiBytes(0xA0,Forward_time_D,2);
//	
//	pid_setup.groups.Forward_time.kp= (float)temp_P/1000.0f;
//	pid_setup.groups.Forward_time.ki= (float)temp_I/1000.0f;
//	pid_setup.groups.Forward_time.kd= (float)temp_D/1000.0f;
//    
//    temp_P= IIC_Read_MultiBytes(0xA0,Left_time_P,2);
//	temp_I= IIC_Read_MultiBytes(0xA0,Left_time_I,2);
//	temp_D= IIC_Read_MultiBytes(0xA0,Left_time_D,2);
//	
//	pid_setup.groups.Left_time.kp= (float)temp_P/1000.0f;
//	pid_setup.groups.Left_time.ki= (float)temp_I/1000.0f;
//	pid_setup.groups.Left_time.kd= (float)temp_D/1000.0f;
//    
//    temp_P= IIC_Read_MultiBytes(0xA0,Exp_Forward,2);
//	temp_I= IIC_Read_MultiBytes(0xA0,Exp_Left,2);
//	temp_D= IIC_Read_MultiBytes(0xA0,Exp_Back,2);
//	
//	pid_setup.groups.exp_angle.exp_forward= (float)temp_P/1000.0f;
//	pid_setup.groups.exp_angle.exp_left= (float)temp_I/1000.0f;
//	pid_setup.groups.exp_angle.exp_back= (float)temp_D/1000.0f;
//	
//}





