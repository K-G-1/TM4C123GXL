//http://bbs.21ic.com/icview-290844-1-1.html
//参考的这片文章，但是对于TM4C123GXL似乎不管用

//不是不管用，而是要使用uartstdio。h中的函数初始化串口
//由于TI的IIC需要回调函数，并且还要一个定时器，和STM32的写法不同，所以就不使用了
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
//

#include "IIC.h"
#include "oled.h"
#include "mpu6050.h"
#include "timer_config.h"
#include "IMU.h"
#include "hmc5883l.h"
#include "motor.h"
#include "send_data.h"
#include "e2prom.h"
#include "Rc_input.h"

#define RED_LED  GPIO_PIN_1
#define BLUE_LED  GPIO_PIN_2
#define GREN_LED  GPIO_PIN_3

#define SYS_CLOCK 16000000
#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

extern volatile uint32_t g_ui32Counter ;
extern volatile uint32_t g_Timer_0_A_Counter;
uint32_t ulColors[3];

//*****************************************************************************
//
// The UART interrupt handler.
//
//*****************************************************************************
void
UART_0_IntHandler(void)
{
    uint32_t ui32Status;
    uint32_t bt = 0;
    //
    // Get the interrrupt status.
    //
    ui32Status = UARTIntStatus(UART0_BASE, true);

    //
    // Clear the asserted interrupts.
    //
    UARTIntClear(UART0_BASE, ui32Status);
    
    if((ui32Status == UART_INT_RX) || (ui32Status == UART_INT_RT))
    {
        while(UARTCharsAvail(UART0_BASE))
        {   
            bt=UARTCharGetNonBlocking(UART0_BASE);
            Data_Receive_Prepare(bt);
        }
    }else if(ui32Status == UART_INT_TX)
    {

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
    // Enable processor interrupts.
    //
    ROM_IntMasterEnable();
    //
    // Configure the UART for 115,200, 8-N-1 operation.
    //
    ROM_UARTConfigSetExpClk(UART0_BASE, ROM_SysCtlClockGet(), 500000,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));

    //
    // Enable the UART interrupt.
    //
    IntPrioritySet(INT_UART0,0x80);
    
    ROM_IntEnable(INT_UART0);
    //没有使用发送中断，因为暂时用不到
    
    
    
    ROM_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
    
    //
    // Prompt for text to be entered.
    //
    UARTSend((uint8_t *)"Usart 0 init:", 16);

}


int main()
{
    
    uint8_t ucDelta,ucState;
    uint16_t Key_status;
    int PWM_data = 0 ;
    unsigned long ulPrevCount = 0;
    uint8_t mpu6050_check = 0;
    
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
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_4| SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);
    // Set the priority grouping for the interrupt controller 
    IntPriorityGroupingSet(3);
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
    ulColors[BLUE] = 0x0000;
    ulColors[RED] = 0xFFFF;
    ulColors[GREEN] = 0x0000;
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

    
    OLED_Init();
    oled_dis_str();
    mpu6050_check = MPU_Init();
    while(mpu6050_check != 0x68)
    {
        
    }
    
    systick_init();
    Timer_2_A_init();
    Init_HMC5883L();
    
    e2prom_init();
    motor_init();
    RC_init();
//    sensor.gyro.CALIBRATE = 1;
//    sensor.acc.CALIBRATE = 1;
    read_Acc_Gyro_offest();
    Read_PID_shell();
    Read_PID_core();
    Read_PID_hight();
    while(1)
    {
        ucState = ButtonsPoll(&ucDelta, 0);
        RGBColorSet(ulColors);
        if(BUTTON_PRESSED(LEFT_BUTTON, ucState, ucDelta))
        {
            sensor.gyro.CALIBRATE = 1;
            sensor.acc.CALIBRATE = 1;
        }
        else if(BUTTON_PRESSED(RIGHT_BUTTON, ucState, ucDelta))
        {
            PWM_data += 0x800 ;
            HWREG(WTIMER0_BASE + TIMER_O_TAMATCHR) = PWM_data;
            HWREG(WTIMER0_BASE + TIMER_O_TBMATCHR) = PWM_data;
            HWREG(WTIMER1_BASE + TIMER_O_TAMATCHR) = PWM_data;
            HWREG(WTIMER1_BASE + TIMER_O_TBMATCHR) = PWM_data;
            
        }
        
        
    }
    return 0;
}
