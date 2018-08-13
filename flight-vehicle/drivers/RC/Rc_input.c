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

uint32_t cap_timer = 0;
uint32_t cap_timer_B = 0;
//*****************************************************************************
//
// The interrupt handler for the first timer interrupt.
//
//*****************************************************************************

uint16_t WTIM2CHA_CAPTURE_STA = 0;
uint16_t WTIM2CHA_CAPTURE_STB = 0;
void
WTimer2IntHandler(void)
{
    unsigned long ulstatus;
    static uint32_t cap_time = 0;
    static uint32_t old_cap_time = 0;
    static uint32_t new_cap_time = 0;
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
            
            if(temp_cnt1_2>=temp_cnt1)
                cap_timer = temp_cnt1_2-temp_cnt1;
            else
                cap_timer = 0xffffffff-temp_cnt1_2+temp_cnt1+1;
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
        if(WTIM2CHA_CAPTURE_STB & 0x40)
        {
            WTIM2CHA_CAPTURE_STB = 0x80;
            temp_cnt2 = TimerValueGet(WTIMER2_BASE, TIMER_B);
            TimerControlEvent(WTIMER2_BASE, TIMER_B, TIMER_EVENT_POS_EDGE);
            
            if(temp_cnt2_2>=temp_cnt2)
                cap_timer_B = temp_cnt2_2-temp_cnt2_2;
            else
                cap_timer_B = 0xffffffff-temp_cnt2_2+temp_cnt2+1;
        }
        else
        {
            WTIM2CHA_CAPTURE_STB = 0;
            WTIM2CHA_CAPTURE_STB |= 0x40;
            TimerControlEvent(WTIMER2_BASE, TIMER_B, TIMER_EVENT_NEG_EDGE);
            
            temp_cnt2_2 = TimerValueGet(WTIMER2_BASE, TIMER_B);  
            
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
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    
    GPIOPinConfigure(GPIO_PD0_WT2CCP0);
    GPIOPinConfigure(GPIO_PD1_WT2CCP1);
    GPIOPinTypeTimer(GPIO_PORTD_BASE, GPIO_PIN_0); 
    GPIOPinTypeTimer(GPIO_PORTD_BASE, GPIO_PIN_1);
    
    GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
    GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
    //wtimer2
    TimerConfigure(WTIMER2_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_TIME_UP|TIMER_CFG_B_CAP_TIME_UP);
    
    TimerControlEvent(WTIMER2_BASE, TIMER_A|TIMER_B, TIMER_EVENT_POS_EDGE);
    
    TimerLoadSet(WTIMER2_BASE, TIMER_A|TIMER_B, 0xffffffff);
    TimerMatchSet(WTIMER2_BASE, TIMER_A|TIMER_B, 0);
    
    TimerIntRegister(WTIMER2_BASE, TIMER_A|TIMER_B, WTimer2IntHandler);
    
    IntMasterEnable();
    
    TimerIntEnable(WTIMER2_BASE, TIMER_CAPA_EVENT|TIMER_CAPB_EVENT);
    
    IntPrioritySet(INT_WTIMER2A,0x20);
    IntPrioritySet(INT_WTIMER2A,0x30);
    IntEnable(INT_WTIMER2A);
    
    TimerEnable(WTIMER2_BASE, TIMER_A|TIMER_B);
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


