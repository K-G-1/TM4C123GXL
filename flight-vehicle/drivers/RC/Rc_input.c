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
//*****************************************************************************
//
// The interrupt handler for the first timer interrupt.
//
//*****************************************************************************
void
WTimer2IntHandler(void)
{
    unsigned long ulstatus;
    static uint32_t cap_time = 0;
    static uint32_t old_cap_time = 0;
    static uint32_t new_cap_time = 0;
    ulstatus = TimerIntStatus(WTIMER2_BASE, false);
 
    TimerIntClear(WTIMER2_BASE, ulstatus);
    
    if(ulstatus == TIMER_CAPA_EVENT)
        new_cap_time =  TimerValueGet(WTIMER2_BASE, TIMER_A);
    
    if(new_cap_time <old_cap_time)
        cap_time = new_cap_time + (0xffff - old_cap_time);
    else 
        cap_time = new_cap_time - old_cap_time;
    
    old_cap_time = new_cap_time;
    cap_timer = cap_time;
}


void RC_init(void)
{
    //
    // Enable the peripherals used by this example.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER2);
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    
    GPIOPinConfigure(GPIO_PD0_WT2CCP0);
    GPIOPinTypeTimer(GPIO_PORTD_BASE, GPIO_PIN_0); 
    
    GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
            
    TimerConfigure(WTIMER2_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_TIME);
    
    TimerControlEvent(WTIMER2_BASE, TIMER_A, TIMER_EVENT_NEG_EDGE);
    
    TimerLoadSet(WTIMER2_BASE, TIMER_A, 0xffffffff);
    TimerMatchSet(WTIMER2_BASE, TIMER_A, 0);
    
    TimerIntRegister(WTIMER2_BASE, TIMER_A, WTimer2IntHandler);
    
    IntMasterEnable();
    
    TimerIntEnable(WTIMER2_BASE, TIMER_CAPA_EVENT);
    
    IntPrioritySet(INT_WTIMER2A,0x20);
    
    IntEnable(INT_WTIMER2A);
    
    TimerEnable(WTIMER2_BASE, TIMER_A);
}




