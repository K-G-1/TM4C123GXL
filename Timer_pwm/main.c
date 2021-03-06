//http://bbs.21ic.com/icview-290844-1-1.html
//参考的这片文章，但是对于TM4C123GXL似乎不管用

//不是不管用，而是要使用uartstdio。h中的函数初始化串口

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include "tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "drivers/RGB.h"
#include "drivers/buttons.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "utils/uartstdio.h"

#include "inc/hw_timer.h"
//加快速度
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"

#define RED_LED  GPIO_PIN_1
#define BLUE_LED  GPIO_PIN_2
#define GREN_LED  GPIO_PIN_3

#define SYS_CLOCK 16000000


unsigned long g_ulCounter = 0;
//*****************************************************************************
//
// Send a string to the UART.
//
//*****************************************************************************
void
UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count)
{
    //
    // Loop while there are more characters to send.
    //
    while(ui32Count--)
    {
        //
        // Write the next character to the UART.
        //
        ROM_UARTCharPutNonBlocking(UART0_BASE, *pui8Buffer++);
    }
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

    // ???????
    ulstatus = TimerIntStatus(TIMER2_BASE, TIMER_CAPA_EVENT);
   
    // ???????   
    TimerIntClear(TIMER2_BASE, ulstatus);
   
    // ?????????
    UARTprintf("Captured Value: %04d\n", TimerValueGet(TIMER2_BASE, TIMER_A));
}



//*****************************************************************************
//
// The interrupt handler for the second timer interrupt.
//
//*****************************************************************************
void
Timer4IntHandler(void)
{
    char cOne, cTwo;

    //
    // Clear the timer interrupt.
    //
    ROM_TimerIntClear(TIMER4_BASE, TIMER_TIMA_TIMEOUT);
    
    UARTSend("Timer 4 interrupt\r\n",20);
    
    
}



//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
void
ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);
    //
    // Prompt for text to be entered.
    //
    UARTSend((uint8_t *)"Usart 0 init:", 16);

}
// ??????????
void WTimer0BIntHandler(void)
{  
    // ????????  
    TimerIntClear(WTIMER0_BASE, TIMER_TIMB_TIMEOUT);
   
    // ???????????  
    g_ulCounter++;
}

//*****************************************************************************
//
// Configure the timer and its pins. 
//
//*****************************************************************************
void Timer3_init()
{
    //
    // Enable the peripherals used by this example.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    
//    GPIOPinConfigure(GPIO_PF2_T1CCP0);
//    GPIOPinTypeTimer(GPIO_PORTF_BASE, GPIO_PIN_2); 
//    
//    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
    ROM_GPIOPinConfigure(BLUE_GPIO_PIN_CFG);
    ROM_GPIOPinTypeTimer(BLUE_GPIO_BASE, BLUE_GPIO_PIN);
    MAP_GPIOPadConfigSet(BLUE_GPIO_BASE, BLUE_GPIO_PIN, GPIO_STRENGTH_8MA_SC,
                     GPIO_PIN_TYPE_STD);     


    HWREG(TIMER1_BASE + TIMER_O_CTL) &= ~(TIMER_CTL_TAEN | TIMER_CTL_TBEN);
    HWREG(TIMER1_BASE + TIMER_O_CFG) = 0x0004;
    HWREG(TIMER1_BASE + TIMER_O_TAMR) = 0x0A;
    HWREG(TIMER1_BASE + TIMER_O_CTL) = 0x0001;
	HWREG(TIMER1_BASE + TIMER_O_TAPR) = 0;
    HWREG(TIMER1_BASE + TIMER_O_TAILR) = 5000 - 1 ;
    HWREG(TIMER1_BASE + TIMER_O_TAMATCHR) = 5000 -2;
    HWREG(TIMER1_BASE + TIMER_O_CTL) |= TIMER_BOTH & (TIMER_CTL_TAEN |
                                                  TIMER_CTL_TBEN);

}


int main()
{
    unsigned long ulColors[3];
    uint8_t ucDelta,ucState;
    int PWM_data = 5000 ;
    unsigned long ulPrevCount = 0;
    //
    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    //
    ROM_FPUEnable();
    ROM_FPULazyStackingEnable();
    
    //
    // Set the clocking to run directly from the crystal.
    //
//    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
//                       SYSCTL_XTAL_16MHZ);
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);
//    //
//    // Initialize the rgb driver.
//    //
//    RGBInit(0);
//    //
//    // Set the intensity level from 0.0f to 1.0f
//    //
//    RGBIntensitySet(0.02f);
//    //
//    // Initialize the three color values.
//    //
//    ulColors[BLUE] = 0xffFF;
//    ulColors[RED] = 0xffff;
//    ulColors[GREEN] = 0xffff;
//    RGBColorSet(ulColors);
//    //
//    // Enable the RGB. This configure GPIOs to the Timer PWM mode needed
//    // to generate the color spectrum.
//    //
//    RGBEnable();
//    
    
    //
    // Initialize the buttons.
    //
    ButtonsInit();
    //
    //uart init
    //
    ConfigureUART();
    // ??????   
    UARTprintf("32-Bit Timer cap Interrupt ->");   
    UARTprintf("\n   Timer = Wide Timer2A");  
    UARTprintf("\n   Mode = cap");   

    //
    //time 3 4 init 
    //
    Timer3_init();
    while(1)
    {
        ucState = ButtonsPoll(&ucDelta, 0);
        if(BUTTON_PRESSED(LEFT_BUTTON, ucState, ucDelta))
        {
            PWM_data -= 100;
            if(PWM_data <= 0)
            {
                PWM_data = 5000-2;
            }
            HWREG(TIMER1_BASE + TIMER_O_TAMATCHR) = PWM_data;
			
			UARTprintf("\n   systick = %d",SysCtlClockGet());
        }
    }
    return 0;
}
