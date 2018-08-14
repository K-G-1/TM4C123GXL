#ifndef _IIC_H_
#define _IIC_H_

#include <stdint.h>
#include <stdbool.h>
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "inc/hw_memmap.h"

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

/*函数声明----------------------------------------------------------------*/
#define IIC_SYS_PORT     SYSCTL_PERIPH_GPIOA //24C02 SCL
#define IIC_PORT GPIO_PORTA_BASE
#define SCL     GPIO_PIN_6 //24C02 SCL
#define SDA     GPIO_PIN_7 //24C02 SDA 

//IO方向设置
#define SCL_OUT() GPIODirModeSet(IIC_PORT, SCL, GPIO_DIR_MODE_OUT)
#define SDA_IN()  GPIODirModeSet(IIC_PORT, SDA, GPIO_DIR_MODE_IN)	//PB9输入模式
#define SDA_OUT() GPIODirModeSet(IIC_PORT, SDA, GPIO_DIR_MODE_OUT) //PB9输出模式
//IO操作函数	
//#define IIC_SCL    PBout(6) //SCL
//#define IIC_SDA    PBout(7) //SDA
#define SCL_H               GPIOPinWrite(IIC_PORT,SCL,SCL)
#define SCL_L               GPIOPinWrite(IIC_PORT,SCL,~(SCL))
#define SDA_H               GPIOPinWrite(IIC_PORT,SDA,SDA)          /*SDA做输出*/
#define SDA_L               GPIOPinWrite(IIC_PORT,SDA,~(SDA))        /*SDA做输入*/
#define SDA_READ            GPIOPinRead(IIC_PORT,SDA) /* 读取SDA*/




//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口				 
u16 IIC_Start(void);
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC_Read_Byte(u8 ack);//IIC读取一个字节
u8 IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号
void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	 


//额外函数
void IIC_ADD_write(u8 DeviceAddr,u8 address,u8 Bytes);
u8 IIC_ADD_read(u8 DeviceAddr,u8 address);
u8 IIC_Read_MultiBytes(u8 DeviceAddr,u8 address,u8 Len,u8 *data);
void IIC_NoAddr_WriteByte(unsigned char address,unsigned char Bytes);

void delay_us(uint16_t time);
void delay_ms(uint16_t time);

#endif

//------------------End of File----------------------------

