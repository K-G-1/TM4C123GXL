#include "timer_config.h"
#include "tm4c123gh6pm.h"
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
//#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/timer.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "utils/uartstdio.h"

#include "mpu6050.h"
#include "IMU.h"
#include "hmc5883l.h"
#include "send_data.h"
#include "oled.h"
#include "Rc_input.h"
#include "control.h"

volatile uint32_t g_ui32Counter = 0;
volatile uint32_t g_Timer_0_A_Counter = 0;


//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - SysTickIntHandler
void systick_init(void)
{

    //
    // Set up the period for the SysTick timer.  The SysTick timer period will
    // be equal to the system clock, resulting in a period of 1 second.
    //
    SysTickPeriodSet(SysCtlClockGet()/4);
    
    //
    // Enable interrupts to the processor.
    //
    IntMasterEnable();

    //
    // Enable the SysTick Interrupt.
    //
    SysTickIntEnable();
    
    //
    // Enable SysTick.
    //
    SysTickEnable();
}


//*****************************************************************************
//
// The interrupt handler for the for Systick interrupt.
//
//*****************************************************************************
void
SysTickIntHandler(void)
{
    static u8 cnt = 0;
    //
    // Update the Systick interrupt counter.
    //
    cnt++;
    if(cnt >= 4)
    {
        cnt = 0;
        g_ui32Counter++;
        g_Timer_0_A_Counter = 0;
//        oled_dis_data(angle.pitch,angle.roll,angle.yaw,0);
    }
    
}

void Timer_2_A_init(void)
{
    //
    // Enable the peripherals used by this example.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
    //
    // Enable processor interrupts.
    //
    ROM_IntMasterEnable();
    //
    // Configure the two 32-bit periodic timers.
    //
    TimerConfigure(TIMER2_BASE, TIMER_CFG_PERIODIC);
//    TimerPrescaleSet(TIMER0_BASE,TIMER_A,16);
    TimerLoadSet(TIMER2_BASE, TIMER_A, SysCtlClockGet()/200);   //50hz 20ms
    //
    // Setup the interrupts for the timer timeouts.
    //
    TimerIntRegister(TIMER2_BASE, TIMER_A, Timer2AIntHandler);
    IntMasterEnable();
    TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
    
    //
    // Enable the timers.
    //
    IntEnable(INT_TIMER2A);
    IntPrioritySet(INT_TIMER2A,0x40);
    TimerEnable(TIMER2_BASE, TIMER_A);
    
    
}

void Timer2AIntHandler(void)
{
    static int time_cnt_1 = 0;
    
    g_Timer_0_A_Counter ++;
    time_cnt_1++;
    Get_Attitude();
    RC_Data_Refine();
//    CONTROL(angle.roll,angle.pitch,angle.yaw);
    if(time_cnt_1 == 2)
        sand_IMU_data();
    else if(time_cnt_1 == 7)
        sand_ACC_GYRO_data();
    else if(time_cnt_1 == 9)
        sand_RC_data();
    else if(time_cnt_1 >= 10)
        time_cnt_1 = 0;
    TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
    
    
    
     
    
}






