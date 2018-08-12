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
#include "IIC.h"
#include "oled.h"
#include "inc/hw_eeprom.h"
#include "driverlib/eeprom.h"
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


int main()
{
    unsigned long ulColors[3];
    uint8_t ucDelta,ucState;
    uint16_t Key_status;
    int PWM_data = 5000 ;
    unsigned long ulPrevCount = 0;
    int eeprom_size = 0;
    int eeprom_write_buf[2] = {0x55,0x33};
    int eeprom_write_data = 0;
    int eeprom_read_data = 0;
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

    UARTprintf("32-Bit Timer cap Interrupt ->");   
    UARTprintf("\n   Timer = Wide Timer2A");  
    UARTprintf("\n   Mode = cap");   
    
    OLED_Init();
    oled_dis_str();
    
    //
    // Enable the EEPROM module.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
    //
    // Wait for the EEPROM module to be ready.
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_EEPROM0))
    { 
        OLED_P6x8Str(0,4,"eeprom init");
    }
    if(EEPROMInit()==EEPROM_INIT_OK)
    {
        OLED_P6x8Str(0,4,"eeprom ok  ");
    }
    eeprom_size = EEPROMSizeGet();
    
    while(1)
    {
        ucState = ButtonsPoll(&ucDelta, 0);
        
        if(BUTTON_PRESSED(LEFT_BUTTON, ucState, ucDelta))
        {
            Key_status =  GPIOPinRead(IIC_PORT,SDA);
			UARTprintf("\n   systick = %d",SysCtlClockGet());
            UARTprintf("\n   key status = %d",Key_status);
            eeprom_write_data += 0x0f;
            EEPROMProgram((uint32_t*)&eeprom_write_data,0x00,sizeof(eeprom_write_data));
            
            
        }
        else if(BUTTON_PRESSED(RIGHT_BUTTON, ucState, ucDelta))
        {
            EEPROMRead(&eeprom_read_data,0x00,sizeof(eeprom_read_data));
            UARTprintf("\n   eeprom size = %d %d",eeprom_size,eeprom_read_data);
        }
        
    }
    return 0;
}
