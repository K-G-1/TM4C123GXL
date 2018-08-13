#include "motor.h"
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_timer.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"

#define Moto_PwmMax 1000
#define Moto_scale 50
#define Moto_Timer WTIMER0_BASE

//电调的PWM频率要求是400HZ
//使用两个64位的定时器 WTIMER0_BASE WTIMER1_BASE
void motor_init()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER1);
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    
    GPIOPinConfigure(GPIO_PC4_WT0CCP0);
    GPIOPinConfigure(GPIO_PC5_WT0CCP1);
    GPIOPinConfigure(GPIO_PC6_WT1CCP0);
    GPIOPinConfigure(GPIO_PC7_WT1CCP1);
    
    GPIOPinTypeTimer(GPIO_PORTC_BASE, GPIO_PIN_4);
    GPIOPinTypeTimer(GPIO_PORTC_BASE, GPIO_PIN_5);
    GPIOPinTypeTimer(GPIO_PORTC_BASE, GPIO_PIN_6);
    GPIOPinTypeTimer(GPIO_PORTC_BASE, GPIO_PIN_7);
    
    GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA,
                     GPIO_PIN_TYPE_STD);     
    GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_5, GPIO_STRENGTH_2MA,
                     GPIO_PIN_TYPE_STD); 
    GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_6, GPIO_STRENGTH_2MA,
                     GPIO_PIN_TYPE_STD); 
    GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_7, GPIO_STRENGTH_2MA,
                     GPIO_PIN_TYPE_STD); 
    // WTIMER0
    HWREG(WTIMER0_BASE + TIMER_O_CTL) &= ~(TIMER_CTL_TAEN | TIMER_CTL_TBEN);//Disable the timers.
    HWREG(WTIMER0_BASE + TIMER_O_CFG) = 0x0004;                              //不分离定时器，TIMER2作为32位定时器
    //timerA
    HWREG(WTIMER0_BASE + TIMER_O_TAMR) = 0x0A;                               //作为PWM模式    
	HWREG(WTIMER0_BASE + TIMER_O_TAPR) = 0;                                  //不分频
    HWREG(WTIMER0_BASE + TIMER_O_TAILR) = 125000 - 1 ;
    HWREG(WTIMER0_BASE + TIMER_O_TAMATCHR) = 125000-2;
    //timerB
    HWREG(WTIMER0_BASE + TIMER_O_TBMR) = 0x0A;                               //作为PWM模式    
	HWREG(WTIMER0_BASE + TIMER_O_TBPR) = 0;                                  //不分频
    HWREG(WTIMER0_BASE + TIMER_O_TBILR) = 125000 - 1 ;
    HWREG(WTIMER0_BASE + TIMER_O_TBMATCHR) = 125000-2;
    
//    HWREG(WTIMER0_BASE + TIMER_O_CTL) = 0x0001;                              //timerA timerB 开始计数
    HWREG(WTIMER0_BASE + TIMER_O_CTL) |= TIMER_BOTH & (TIMER_CTL_TAEN |
                                                  TIMER_CTL_TBEN |TIMER_CTL_TAPWML |TIMER_CTL_TBPWML);
    //WTIMER1                                              
    HWREG(WTIMER1_BASE + TIMER_O_CTL) &= ~(TIMER_CTL_TAEN | TIMER_CTL_TBEN);//Disable the timers.
    HWREG(WTIMER1_BASE + TIMER_O_CFG) = 0x0004;                              //不分离定时器，TIMER2作为32位定时器
    //timerA
    HWREG(WTIMER1_BASE + TIMER_O_TAMR) = 0x0A;                               //作为PWM模式    
	HWREG(WTIMER1_BASE + TIMER_O_TAPR) = 0;                                  //不分频
    HWREG(WTIMER1_BASE + TIMER_O_TAILR) = 125000 - 1 ;
    HWREG(WTIMER1_BASE + TIMER_O_TAMATCHR) = 125000-2;
    //timerB
    HWREG(WTIMER1_BASE + TIMER_O_TBMR) = 0x0A;                               //作为PWM模式    
	HWREG(WTIMER1_BASE + TIMER_O_TBPR) = 0;                                  //不分频
    HWREG(WTIMER1_BASE + TIMER_O_TBILR) = 125000 - 1 ;
    HWREG(WTIMER1_BASE + TIMER_O_TBMATCHR) = 125000-2;
    
//    HWREG(WTIMER0_BASE + TIMER_O_CTL) = 0x0001;                              //timerA timerB 开始计数
    HWREG(WTIMER1_BASE + TIMER_O_CTL) |= TIMER_BOTH & (TIMER_CTL_TAEN |
                                                  TIMER_CTL_TBEN |TIMER_CTL_TAPWML |TIMER_CTL_TBPWML );

}

void Moto_PwmRflash(uint16_t MOTO1_PWM,uint16_t MOTO2_PWM,uint16_t MOTO3_PWM,uint16_t MOTO4_PWM)
{		
	if(MOTO1_PWM>Moto_PwmMax)	MOTO1_PWM = Moto_PwmMax;
	if(MOTO2_PWM>Moto_PwmMax)	MOTO2_PWM = Moto_PwmMax;
	if(MOTO3_PWM>Moto_PwmMax)	MOTO3_PWM = Moto_PwmMax;
	if(MOTO4_PWM>Moto_PwmMax)	MOTO4_PWM = Moto_PwmMax;
	
	
	HWREG(WTIMER0_BASE + TIMER_O_TAMATCHR) = Moto_scale * (MOTO1_PWM + 1000);
    HWREG(WTIMER0_BASE + TIMER_O_TBMATCHR) = Moto_scale * (MOTO2_PWM + 1000);
    HWREG(WTIMER1_BASE + TIMER_O_TAMATCHR) = Moto_scale * (MOTO3_PWM + 1000);
    HWREG(WTIMER1_BASE + TIMER_O_TBMATCHR) = Moto_scale * (MOTO4_PWM + 1000);

}
