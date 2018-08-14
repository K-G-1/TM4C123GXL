#include "Rc_input.h"

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"


#define RC_scale  50
uint32_t Rc_Pwm_In[8];
T_RC_DATA Rc_Data;//1000~2000
uint32_t  RC_Pwm_In_his[8];
uint8_t fly_mode = 0;
uint8_t ARMED = 0;
//*****************************************************************************
//
// The interrupt handler for the first timer interrupt.
//
//*****************************************************************************

uint16_t WTIM2CHA_CAPTURE_STA = 0;
uint16_t WTIM2CHB_CAPTURE_STA = 0;
void
WTimer2IntHandler(void)
{
    unsigned long ulstatus;
    static uint32_t temp_cnt1,temp_cnt1_2,temp_cnt2,temp_cnt2_2;
    ulstatus = TimerIntStatus(WTIMER2_BASE, false);
 
    TimerIntClear(WTIMER2_BASE, ulstatus);

    if(ulstatus & TIMER_CAPA_EVENT)
    {
        if(WTIM2CHA_CAPTURE_STA & 0x40)
        {
            WTIM2CHA_CAPTURE_STA = 0x80;
            temp_cnt1 = TimerValueGet(WTIMER2_BASE, TIMER_A);
            TimerControlEvent(WTIMER2_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);
            
            if(temp_cnt1>temp_cnt1_2)
                Rc_Pwm_In[0] = (0xffffffff-temp_cnt1+temp_cnt1_2) / RC_scale;
            else
                Rc_Pwm_In[0] = (temp_cnt1_2-temp_cnt1) / RC_scale;
                
        }
        else
        {
            WTIM2CHA_CAPTURE_STA = 0;
            WTIM2CHA_CAPTURE_STA |= 0x40;
            TimerControlEvent(WTIMER2_BASE, TIMER_A, TIMER_EVENT_NEG_EDGE);
            
            temp_cnt1_2 = TimerValueGet(WTIMER2_BASE, TIMER_A);  
            
        }
    }
    
    if(ulstatus & TIMER_CAPB_EVENT)
    {
        if(WTIM2CHB_CAPTURE_STA & 0x40)
        {
            WTIM2CHB_CAPTURE_STA = 0x80;
            temp_cnt2 = TimerValueGet(WTIMER2_BASE, TIMER_B);
            TimerControlEvent(WTIMER2_BASE, TIMER_B, TIMER_EVENT_POS_EDGE);
            
            if(temp_cnt2>temp_cnt2_2)
                Rc_Pwm_In[1] = (0xffffffff-temp_cnt2+temp_cnt2_2) / RC_scale;
            else
                Rc_Pwm_In[1] = (temp_cnt2_2-temp_cnt2 ) / RC_scale;
        }
        else
        {
            WTIM2CHB_CAPTURE_STA = 0;
            WTIM2CHB_CAPTURE_STA |= 0x40;
            TimerControlEvent(WTIMER2_BASE, TIMER_B, TIMER_EVENT_NEG_EDGE);
            
            temp_cnt2_2 = TimerValueGet(WTIMER2_BASE, TIMER_B);  
            
        }
    }
}

uint16_t WTIM3CHA_CAPTURE_STA = 0;
uint16_t WTIM3CHB_CAPTURE_STA = 0;
void
WTimer3IntHandler(void)
{
    unsigned long ulstatus;
    static uint32_t temp_cnt3,temp_cnt3_2,temp_cnt4,temp_cnt4_2;
    ulstatus = TimerIntStatus(WTIMER3_BASE, false);
 
    TimerIntClear(WTIMER3_BASE, ulstatus);

    if(ulstatus & TIMER_CAPA_EVENT)
    {
        if(WTIM3CHA_CAPTURE_STA & 0x40)
        {
            WTIM3CHA_CAPTURE_STA = 0x80;
            temp_cnt3 = TimerValueGet(WTIMER3_BASE, TIMER_A);
            TimerControlEvent(WTIMER3_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);
            
            if(temp_cnt3_2>=temp_cnt3)
                Rc_Pwm_In[2] = (temp_cnt3_2-temp_cnt3) / RC_scale;
            else
                Rc_Pwm_In[2] = (0xffffffff-temp_cnt3_2+temp_cnt3) / RC_scale;
        }
        else
        {
            WTIM3CHA_CAPTURE_STA = 0;
            WTIM3CHA_CAPTURE_STA |= 0x40;
            TimerControlEvent(WTIMER3_BASE, TIMER_A, TIMER_EVENT_NEG_EDGE);
            
            temp_cnt3_2 = TimerValueGet(WTIMER3_BASE, TIMER_A);  
            
        }
    }
    
    if(ulstatus & TIMER_CAPB_EVENT)
    {
        if(WTIM3CHB_CAPTURE_STA & 0x40)
        {
            WTIM3CHB_CAPTURE_STA = 0x80;
            temp_cnt4 = TimerValueGet(WTIMER3_BASE, TIMER_B);
            TimerControlEvent(WTIMER3_BASE, TIMER_B, TIMER_EVENT_POS_EDGE);
            
            if(temp_cnt4_2>=temp_cnt4)
                Rc_Pwm_In[3] = (temp_cnt4_2-temp_cnt4_2) / RC_scale;
            else
                Rc_Pwm_In[3] = (0xffffffff-temp_cnt4_2+temp_cnt4) / RC_scale;
        }
        else
        {
            WTIM3CHB_CAPTURE_STA = 0;
            WTIM3CHB_CAPTURE_STA |= 0x40;
            TimerControlEvent(WTIMER3_BASE, TIMER_B, TIMER_EVENT_NEG_EDGE);
            
            temp_cnt4_2 = TimerValueGet(WTIMER3_BASE, TIMER_B);  
            
        }
    }
}

//默认向下计数，重装载值是32位，所以中断李也是这样

void RC_init(void)
{
    //
    // Enable the peripherals used by this example.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER2);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER3);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    
    GPIOPinConfigure(GPIO_PD0_WT2CCP0);
    GPIOPinConfigure(GPIO_PD1_WT2CCP1);
    GPIOPinConfigure(GPIO_PD2_WT3CCP0);
    GPIOPinConfigure(GPIO_PD3_WT3CCP1);
    GPIOPinTypeTimer(GPIO_PORTD_BASE, GPIO_PIN_0); 
    GPIOPinTypeTimer(GPIO_PORTD_BASE, GPIO_PIN_1);
    GPIOPinTypeTimer(GPIO_PORTD_BASE, GPIO_PIN_2); 
    GPIOPinTypeTimer(GPIO_PORTD_BASE, GPIO_PIN_3);
    
    GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
    GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
    GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
    GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_3, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
    //wtimer2
    TimerConfigure(WTIMER2_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_TIME|TIMER_CFG_B_CAP_TIME);
    
    TimerControlEvent(WTIMER2_BASE, TIMER_A|TIMER_B, TIMER_EVENT_POS_EDGE);
    TimerPrescaleSet(WTIMER2_BASE, TIMER_A|TIMER_B,50);
    TimerLoadSet(WTIMER2_BASE, TIMER_A|TIMER_B, 0xffffff);
    TimerMatchSet(WTIMER2_BASE, TIMER_A|TIMER_B, 0);
    
    TimerIntRegister(WTIMER2_BASE, TIMER_A|TIMER_B, WTimer2IntHandler);
    
    IntMasterEnable();
    
    TimerIntEnable(WTIMER2_BASE, TIMER_CAPA_EVENT|TIMER_CAPB_EVENT);
    
    IntPrioritySet(INT_WTIMER2A,0x20);
    IntPrioritySet(INT_WTIMER2A,0x21);
    IntEnable(INT_WTIMER2A);
    
    TimerEnable(WTIMER2_BASE, TIMER_A|TIMER_B);
    
    //wtimer3
    TimerConfigure(WTIMER3_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_TIME_UP|TIMER_CFG_B_CAP_TIME_UP);
    
    TimerControlEvent(WTIMER3_BASE, TIMER_A|TIMER_B, TIMER_EVENT_POS_EDGE);
    
    TimerLoadSet(WTIMER3_BASE, TIMER_A|TIMER_B, 0xffffffff);
    TimerMatchSet(WTIMER3_BASE, TIMER_A|TIMER_B, 0);
    
    TimerIntRegister(WTIMER3_BASE, TIMER_A|TIMER_B, WTimer3IntHandler);
    
    IntMasterEnable();
    
    TimerIntEnable(WTIMER3_BASE, TIMER_CAPA_EVENT|TIMER_CAPB_EVENT);
    
    IntPrioritySet(INT_WTIMER3A,0x22);
    IntPrioritySet(INT_WTIMER3B,0x23);
    IntEnable(INT_WTIMER3B);
    
    TimerEnable(WTIMER3_BASE, TIMER_A|TIMER_B);
    
}


//*****************************************************************************
//
// The interrupt handler for the first timer interrupt.
//
//*****************************************************************************
void
Timer3IntHandler(void)
{
    unsigned long ulstatus;
    static uint32_t cap_time = 0;
    static uint32_t old_cap_time = 0;
    static uint32_t new_cap_time = 0;
    ulstatus = TimerIntStatus(TIMER3_BASE, false);
 
    TimerIntClear(TIMER3_BASE, ulstatus);
    
    if(ulstatus == TIMER_CAPA_EVENT)
        new_cap_time =  TimerValueGet(TIMER3_BASE, TIMER_A);
    
    if(new_cap_time <old_cap_time)
        cap_time = new_cap_time + (0xffff - old_cap_time);
    else 
        cap_time = new_cap_time - old_cap_time;
    
    old_cap_time = new_cap_time;

}



void time_cap_init(void)
{
    //
    // Enable the peripherals used by this example.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    
    GPIOPinConfigure(GPIO_PB2_T3CCP0);
    GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_2); 
    
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
            
    TimerConfigure(TIMER3_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_TIME_UP);
    
    TimerControlEvent(TIMER3_BASE, TIMER_A, TIMER_EVENT_NEG_EDGE);
    
    TimerLoadSet(TIMER3_BASE, TIMER_A, 0xffff);
    TimerMatchSet(TIMER3_BASE, TIMER_A, 0);
    
    TimerIntRegister(TIMER3_BASE, TIMER_A, Timer3IntHandler);
    
    IntMasterEnable();
    
    TimerIntEnable(TIMER3_BASE, TIMER_CAPA_EVENT);
    
    IntEnable(INT_TIMER3A);
    
    TimerEnable(TIMER3_BASE, TIMER_A);
    
}

///////////


/*====================================================================================================*/
/*====================================================================================================*
**函数 : RcData_Refine
**功能 : 提炼遥控数据
**输入 : None
**输出 : None
**备注 : 无
**====================================================================================================*/
/*====================================================================================================*/

extern uint32_t Moto_duty[4];
void RC_Data_Refine(void)
{
//    static uint8_t chan,a;	

//	uint32_t rcDataMax[4], rcDataMin[4];
//	static uint32_t rcDataCache[4][4], rcDataMean[4], rcDatasum[4];
//	static uint8_t rcValuesIndex = 0;

//	rcValuesIndex++;
//	for (chan = 0; chan < 4; chan++) {
//        //滑动平均值滤波，4次
//        if(Rc_Pwm_In[chan]  >2800 || Rc_Pwm_In[chan] <800) 
//            Rc_Pwm_In[chan] = RC_Pwm_In_his[chan];
//        rcDataCache[chan][rcValuesIndex % 4] = Rc_Pwm_In[chan] ;		
//        RC_Pwm_In_his[chan] = Rc_Pwm_In[chan];

////        rcDataMean[chan] = 0;
////        rcDataMax[chan]  = 0;
////        rcDataMin[chan]  = 0xffffffff;
////        rcDatasum[chan] = 0;
////        
////          // 记录缓存中最大值 && 最小值
////        if(rcDataCache[chan][a] > rcDataMax[chan])  rcDataMax[chan] = rcDataCache[chan][a];     
////        if(rcDataCache[chan][a] < rcDataMin[chan])	rcDataMin[chan] = rcDataCache[chan][a]; 
////          // 求和
//            rcDatasum[chan] += rcDataCache[chan][rcValuesIndex % 4];  
//        
//        // 剔除缓存中 最大值 && 最小值 
////        rcDataMean[chan] = (rcDatasum[chan] - (rcDataMax[chan] + rcDataMin[chan])) / 2;
//        
//        rcDataMean[chan] = rcDatasum[chan] /4;
//	} 
//	a ++;
//    if(a >= 4)
//    {
//        a = 0;
//    }
//	//for(chan=0;chan<6;chan++)
//	 Rc_Data.YAW   = rcDataMean[0];
//	 Rc_Data.THROTTLE  = rcDataMean[1];
//	 Rc_Data.PITCH  =   rcDataMean[3];
//	 Rc_Data.ROLL =  rcDataMean[2];

	 Rc_Data.YAW   = Rc_Pwm_In[0];
	 Rc_Data.THROTTLE  = Rc_Pwm_In[1];
	 Rc_Data.PITCH  =   Rc_Pwm_In[3];
	 Rc_Data.ROLL =  Rc_Pwm_In[2];
}

/*    上锁&解锁函数  */
void Deblocking(void)
{
	 static uint8_t flag=1;
	 static uint16_t time1=0,time2=0;
	 /*               遥控上锁                 */
	 /*     —————————            —————————     */
	 /*    |         |          |         |    */
	 /*    |         |          |         |    */
	 /*    |    \    |          |   /     |    */
	 /*    |     \   |          |  /      |    */
	 /*     —————————            —————————     */
	 /*   油门拉到最低         摇杆推到左上角  */
   if(ARMED && Rc_Data.ROLL >= 1800 && Rc_Data.PITCH <= 1200 && Rc_Data.THROTTLE <= 1200 &&  Rc_Data.YAW <= 1200)		
	 {  
			time1++; 
	 }	
	 else 
		 time1=0;
	 if(time1>30 && ARMED) 
	 { 
			ARMED = 0; 
			time1 = 0;
	 }
   /*               遥控解锁                 */
	 /*     —————————            —————————     */
	 /*    |         |          |         |    */
	 /*    |         |          |         |    */
	 /*    |    /    |          |     \   |    */
	 /*    |   /     |          |      \  |    */
	 /*     —————————            —————————     */
	 /*   油门拉到最低         摇杆推到右上角  */
   if(!ARMED && Rc_Data.YAW >= 1800 && Rc_Data.PITCH <= 1200 && Rc_Data.THROTTLE <= 1200 &&  Rc_Data.ROLL <= 1200)		
    {
        time2++; 
        
    }	
	 else 
		 time2=0;
	 if(time2>=30 && !ARMED)
	 {
		  ARMED = 1; 
			time2 = 0;
	 }

}
/***************************

mode = 0;   未知
mode = 1,		姿态
mode = 2;		定高
mode = 3;		定点


*****************************/
void mode_contrl(void)
{
	if(Rc_Data.AUX1>1700)
	{
		fly_mode = 2 ;  //定高	

	}
	else if(Rc_Data.AUX1>1300&&Rc_Data.AUX1<1700)
	{
		fly_mode= 1;
	}
	else if(Rc_Data.AUX1<1300)
	{
		fly_mode= 0 ;
	}
		
}