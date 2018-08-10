
#include "IIC.h"


void delay_us(uint16_t time)
{
    SysCtlDelay(time * (SysCtlClockGet()/3000000));
}

void delay_ms(uint16_t time)
{
    SysCtlDelay(time * (SysCtlClockGet()/3000));
}
/********************************IIC��������*****************************************************/
//��ʼ��IIC
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
//����IIC��ʼ�ź�
u16 IIC_Start(void)
{
	SDA_OUT();//sda�����
	SDA_H;     	  
    SCL_H;
	delay_us(1);
    SDA_IN();
 	if(!SDA_READ) 
        return 1;	//SDA��Ϊ�͵�ƽ������æ,�˳�
    SDA_OUT();
	SDA_L;
	delay_us(1);
	return 0;
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
    SCL_L;
    SDA_L;
 	delay_us(1);
    SCL_H;
	delay_us(1);
    SDA_H;
	delay_us(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
    SCL_L;
	SDA_OUT();      //SDA����Ϊ����  
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
//����ACKӦ��
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
//������ACKӦ��		    
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

//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    SCL_L;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        if(txd&0x80)
				{
					SDA_H;
				}
				else SDA_L;
        txd<<=1; 	  
		delay_us(2);   //��TEA5767��������ʱ���Ǳ����
		SCL_H;
		delay_us(2); 
		SCL_L;	
		delay_us(2);
    }	 
} 

//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC_Read_Byte(u8 ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
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

/*********************************����IIC����****************************************************/


/************************************************************   
* ������:IIC_ADD_write   
* ���� : ���ض��豸id���ض���ַ��д���ֽ� 
* ����  :�豸id���ڲ���ַ������    
* ���  :��    
*/
void IIC_ADD_write(u8 DeviceAddr,u8 address,u8 Bytes)
{
	IIC_Start();
	IIC_Send_Byte(DeviceAddr);
	IIC_Wait_Ack();
	IIC_Send_Byte(address);   //���͵͵�ַ
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(Bytes);     //�����ֽ�							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//����һ��ֹͣ���� 
	delay_us(10);	
}

/************************************************************   
* ������:I2C_ReadByte   
* ���� : ���ض��豸id���ض���ַ��ȡ����
* ����  :�豸id���ڲ���ַ   
* ���  :��ȡ������   
*/
u8 IIC_ADD_read(u8 DeviceAddr,u8 address)
{
	  unsigned char temp;
   IIC_Start();
   IIC_Send_Byte(DeviceAddr);
   IIC_Wait_Ack();
 
    IIC_Send_Byte(address);   //���͵͵�ַ
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(DeviceAddr+1);           //�������ģʽ			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		

    IIC_Stop();//����һ��ֹͣ����	    
	return temp;
}

/************************************************************   
* ������:IIC_Read_MultiBytes   
* ���� : ���ض��豸id���ض���ַ��ȡ����ֽ�
* ����  :�豸id���ڲ���ַ����Ҫ�����ֽڸ�����0����ȡ��
* ���  :��ȡ������   
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
* ������:IIC_NoAddr_WriteByte   
* ���� : ���ض��豸id��д���ֽ� 
* ����  :�豸id������   
* ���  :��    
*/
void IIC_NoAddr_WriteByte(unsigned char address,unsigned char Bytes)
{

	IIC_Start();
	IIC_Send_Byte(address);   //���͵͵�ַ
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(Bytes);     //�����ֽ�							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//����һ��ֹͣ���� 
	delay_ms(10);

}


/***************************************************************/











