
#include "IIC.h"


void delay_us(uint16_t time)
{
    SysCtlDelay(time * (SysCtlClockGet()/3000000));
}

void delay_ms(uint16_t time)
{
    SysCtlDelay(time * (SysCtlClockGet()/3000));
}
/********************************IIC基础函数*****************************************************/
//初始化IIC
void IIC_Init(void)
{			
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    
    
    GPIOPinConfigure(IIC_PORT);
    GPIOPadConfigSet(IIC_PORT, SCL, GPIO_STRENGTH_8MA,GPIO_PIN_TYPE_STD);  
    GPIOPadConfigSet(IIC_PORT, SDA, GPIO_STRENGTH_8MA,GPIO_PIN_TYPE_STD); 
    
    SCL_OUT();
    SDA_OUT();
    SCL_H;
    SDA_H;
}
//产生IIC起始信号
u16 IIC_Start(void)
{
	SDA_OUT();//sda线输出
	SDA_H;     	  
    SCL_H;
	delay_us(1);
    SDA_IN();
 	if(!SDA_READ) 
        return 1;	//SDA线为低电平则总线忙,退出
    SDA_OUT();
	SDA_L;
	delay_us(1);
	return 0;
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
    SCL_L;
    SDA_L;
 	delay_us(1);
    SCL_H;
	delay_us(1);
    SDA_H;
	delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
    SCL_L;
	SDA_OUT();      //SDA设置为输入  
    SDA_H; 
    delay_us(1);    
    SCL_H;
    delay_us(1);
    SDA_IN();
	while(SDA_READ)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
            SDA_OUT();
			IIC_Stop();
			return 1;
		}
	}
    SDA_OUT();
    SCL_L;
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
    SCL_L;
	SDA_OUT();
    SDA_L;
	delay_us(2);
    SCL_H;
	delay_us(2);
    SCL_L;
	delay_us(2);
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	SCL_L;
	delay_us(2);
	SDA_OUT();
    SDA_H;
	delay_us(2);
	SCL_H;
	delay_us(2);
	SCL_L;
}	

//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    SCL_L;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        if(txd&0x80)
				{
					SDA_H;
				}
				else SDA_L;
        txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		SCL_H;
		delay_us(2); 
		SCL_L;	
		delay_us(2);
    }	 
} 

//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(u8 ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    SDA_H;
    for(i=0;i<8;i++ )
	{
        SCL_L; 
        delay_us(2);
		SCL_H;
        receive<<=1;
        if(SDA_READ)
					receive++;   
//        SDA_OUT();
		delay_us(1); 
    }					 
    
//    if(ack)
//        IIC_Ack();
//    else
//        IIC_NAck();
    return receive;
}

/*********************************额外IIC函数****************************************************/


/************************************************************   
* 函数名:IIC_ADD_write   
* 描述 : 向特定设备id的特定地址，写入字节 
* 输入  :设备id，内部地址，数据    
* 输出  :无    
*/
void IIC_ADD_write(u8 DeviceAddr,u8 address,u8 Bytes)
{
	IIC_Start();
	IIC_Send_Byte(DeviceAddr);
	IIC_Wait_Ack();
	IIC_Send_Byte(address);   //发送低地址
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(Bytes);     //发送字节							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//产生一个停止条件 
	delay_us(10);	
}

/************************************************************   
* 函数名:I2C_ReadByte   
* 描述 : 从特定设备id的特定地址读取内容
* 输入  :设备id，内部地址   
* 输出  :读取的内容   
*/
u8 IIC_ADD_read(u8 DeviceAddr,u8 address)
{
	  unsigned char temp;
   IIC_Start();
   IIC_Send_Byte(DeviceAddr);
   IIC_Wait_Ack();
 
    IIC_Send_Byte(address);   //发送低地址
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(DeviceAddr+1);           //进入接收模式			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		

    IIC_Stop();//产生一个停止条件	    
	return temp;
}

/************************************************************   
* 函数名:IIC_Read_MultiBytes   
* 描述 : 从特定设备id的特定地址读取多个字节
* 输入  :设备id，内部地址，需要读的字节个数（0不读取）
* 输出  :读取的内容   
*************************************************************/
u8 IIC_Read_MultiBytes(u8 DeviceAddr,u8 address,u8 Len)
{
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=IIC_ADD_read(DeviceAddr,address+Len-t-1); 	 				   
	}
	return temp;
}


/************************************************************   
* 函数名:IIC_NoAddr_WriteByte   
* 描述 : 向特定设备id，写入字节 
* 输入  :设备id，内容   
* 输出  :无    
*/
void IIC_NoAddr_WriteByte(unsigned char address,unsigned char Bytes)
{

	IIC_Start();
	IIC_Send_Byte(address);   //发送低地址
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(Bytes);     //发送字节							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//产生一个停止条件 
	delay_ms(10);

}


/***************************************************************/











