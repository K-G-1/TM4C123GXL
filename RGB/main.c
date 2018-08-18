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
#include "utils/softi2c.h"
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

#define RED_COLOR     ulColors[BLUE] = 0x0000;\
    ulColors[RED] = 0xFFFF;\
    ulColors[GREEN] = 0x0000;

#define BULE_COLOR     ulColors[BLUE] = 0xFFFF;\
    ulColors[RED] = 0x0000;\
    ulColors[GREEN] = 0x0000;

#define GREEN_COLOR     ulColors[BLUE] = 0x0000;\
    ulColors[RED] = 0x0000;\
    ulColors[GREEN] = 0xFFFF;

#define BLACK_COLOR     ulColors[BLUE] = 0x0000;\
    ulColors[RED] = 0x0000;\
    ulColors[GREEN] = 0x0000;

#define WHITE_COLOR     ulColors[BLUE] = 0xFFFF;\
    ulColors[RED] = 0xFFFFF;\
    ulColors[GREEN] = 0xFFFF;

#define Purple_COLOR     ulColors[BLUE] = 0xFFFF;\
    ulColors[RED] = 0xFFFF;\
    ulColors[GREEN] = 0x0000;
    
#define Yellow_COLOR     ulColors[BLUE] = 0x0000;\
    ulColors[RED] = 0xFFFF;\
    ulColors[GREEN] = 0xFFFF;  
    
#define Cyan_COLOR     ulColors[BLUE] = 0xFFFF;\
    ulColors[RED] = 0x0000;\
    ulColors[GREEN] = 0xFFFF;  
    
#define GreenYellow_COLOR     ulColors[BLUE] = 0x0000;\
    ulColors[RED] = 0x3E00;\
    ulColors[GREEN] = 0xFFFF; 
    
#define Orange_COLOR     ulColors[BLUE] = 0x0000;\
    ulColors[RED] = 0xBFFF;\
    ulColors[GREEN] = 0x3B00; 
    


int main()
{
    unsigned long ulColors[3];
    uint8_t ucDelta,ucState;
    uint16_t Key_status;
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
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);
    //
    // Initialize the rgb driver.
    //
    RGBInit(0);
    //
    // Set the intensity level from 0.0f to 1.0f
    //
    RGBIntensitySet(0.04f);
    //
    // Initialize the three color values.
    //
    ulColors[BLUE] = 0x0000;
    ulColors[RED] = 0xFFFF;
    ulColors[GREEN] = 0x0000;
    GreenYellow_COLOR;
    RGBColorSet(ulColors);
    //
    // Enable the RGB. This configure GPIOs to the Timer PWM mode needed
    // to generate the color spectrum.
    //
    RGBEnable();
    
    
    //
    // Initialize the buttons.
    //
    ButtonsInit();
    //
    //uart init
    //
    ConfigureUART();  
    

    
    while(1)
    {
        ucState = ButtonsPoll(&ucDelta, 0);
        
        if(BUTTON_PRESSED(LEFT_BUTTON, ucState, ucDelta))
        {
            Yellow_COLOR;
            RGBColorSet(ulColors);
        }
        else if(BUTTON_PRESSED(RIGHT_BUTTON, ucState, ucDelta))
        {
            Orange_COLOR;
            RGBColorSet(ulColors);
        }
        
    }
    return 0;
}
