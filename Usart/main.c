#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
//#include "tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "drivers/RGB.h"
#include "drivers/buttons.h"

#include "inc/hw_ints.h"
#include "inc/hw_uart.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/interrupt.h"
#include "driverlib/uart.h"
//加快速度
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"

#define RED_LED  GPIO_PIN_1
#define BLUE_LED  GPIO_PIN_2
#define GREN_LED  GPIO_PIN_3

#define SYS_CLOCK 16000000


uint8_t SndFrmCnt[5]; 
char bt;
//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif


//*****************************************************************************
//
// The UART interrupt handler.
//
//*****************************************************************************
void
UARTIntHandler(void)
{
    uint32_t ui32Status;

    //
    // Get the interrrupt status.
    //
    ui32Status = UARTIntStatus(UART0_BASE, true);

    //
    // Clear the asserted interrupts.
    //
    UARTIntClear(UART0_BASE, ui32Status);

//    //
//    // Loop while there are characters in the receive FIFO.
//    //
//    while(ROM_UARTCharsAvail(UART0_BASE))
//    {
//        //
//        // Read the next character from the UART and write it back to the UART.
//        //
//        ROM_UARTCharPutNonBlocking(UART0_BASE,
//                                   ROM_UARTCharGetNonBlocking(UART0_BASE));
//    }
    
    if((ui32Status == UART_INT_RX) || (ui32Status == UART_INT_RT))
    {
        while(UARTCharsAvail(UART0_BASE))
        {   
            bt=UARTCharGetNonBlocking(UART0_BASE);
//            ROM_UARTCharPutNonBlocking(UART0_BASE, bt);
        }
    }else if(ui32Status == UART_INT_TX)
    {
        while(UARTSpaceAvail(UART0_BASE))
        {

                UARTCharPutNonBlocking(UART0_BASE, bt);
                UARTIntDisable(UART0_BASE, UART_INT_TX);

                GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1,GPIO_PIN_1); //TXEN-1=1  ??
                
        }
    }
}


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


void Uart_TX()
{
    SndFrmCnt[5]=0;
    ROM_UARTIntEnable(UART0_BASE, UART_INT_TX);
    ROM_UARTCharPutNonBlocking(UART0_BASE, 0xFF);
}
//*****************************************************************************
//
// This example demonstrates how to send a string of data to the UART.
// 串口发送中断未完成，进不去中断，
// 接受中断OK
//*****************************************************************************

int main()
{
    unsigned long ulColors[3];
    uint8_t ucDelta,ucState;
    //
    // Initialize the rgb driver.
    //
    RGBInit(0);
    //
    // Set the intensity level from 0.0f to 1.0f
    //
    RGBIntensitySet(0.02f);
    //
    // Initialize the three color values.
    //
    ulColors[BLUE] = 0xffFF;
    ulColors[RED] = 0xffff;
    ulColors[GREEN] = 0xffff;
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
    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    //
    ROM_FPUEnable();
    ROM_FPULazyStackingEnable();

    //
    // Set the clocking to run directly from the crystal.
    //
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);

    //
    // Enable the GPIO port that is used for the on-board LED.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);


    //
    // Enable the peripherals used by this example.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable processor interrupts.
    //
    ROM_IntMasterEnable();

    //
    // Set GPIO A0 and A1 as UART pins.
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Configure the UART for 115,200, 8-N-1 operation.
    //
    ROM_UARTConfigSetExpClk(UART0_BASE, ROM_SysCtlClockGet(), 115200,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));

    //
    // Enable the UART interrupt.
    //
    ROM_IntEnable(INT_UART0);
    ROM_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT|UART_INT_TX);
    
    //
    // Prompt for text to be entered.
    //
    UARTSend((uint8_t *)"text: ", 6);

    while(1)
    {
        //
        // Poll the buttons. When called periodically this function will
        // run the button debouncing algorithm.
        //
        ucState = ButtonsPoll(&ucDelta, 0);
        //
        // Test to see if the SELECT button was pressed and do something
        //
        if(BUTTON_PRESSED(LEFT_BUTTON, ucState, ucDelta))
        {
            ulColors[BLUE] =(0xCCFF33 & 0xFF0000)>>8;
            ulColors[RED] = (0xCCFF33 &0x00FF00);
            ulColors[GREEN] = (0xCCFF33 &0x0000FF)<<8;
            RGBSet(ulColors,0.02f);
        }
        else if(BUTTON_PRESSED(RIGHT_BUTTON, ucState, ucDelta))
        {
            ulColors[BLUE] =(0xFF00FF & 0xFF0000)>>8;
            ulColors[RED] = (0xFF00FF &0x00FF00);
            ulColors[GREEN] = (0xFF00FF &0x0000FF)<<8;
            RGBSet(ulColors,0.02f);
            UARTIntEnable(UART0_BASE, UART_INT_TX);
            HWREG(UART0_BASE + UART_O_DR) = 0x34;

//            UARTCharPutNonBlocking(UART0_BASE, 0xFF);
        }
    }
    return 0;
}
