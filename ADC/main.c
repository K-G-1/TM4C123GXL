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
#include "driverlib/adc.h"
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
#include "inc/hw_adc.h"
#include "IIC.h"
#include "oled.h"
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
    uint32_t pui32ADC0Value[4];
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

    //
    // Display the setup on the console.
    //
    UARTprintf("ADC ->\n");
    UARTprintf("  Type: Single Ended\n");
    UARTprintf("  Samples: One\n");
    UARTprintf("  Update Rate: 250ms\n");
    UARTprintf("  Input Pin: AIN0/PE3\n\n");  
    
    OLED_Init();
    oled_dis_str();
    
    //
    // The ADC0 peripheral must be enabled for use.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    //
    // For this example ADC0 is used with AIN0 on port E7.
    // The actual port and pins used may be different on your part, consult
    // the data sheet for more information.  GPIO port E needs to be enabled
    // so these pins can be used.
    // TODO: change this to whichever GPIO port you are using.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    
    //
    // Select the analog ADC function for these pins.
    // Consult the data sheet to see which functions are allocated per pin.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);
    
   //
    // Enable sample sequence 3 with a processor signal trigger.  Sequence 3
    // will do a single sample when the processor sends a signal to start the
    // conversion.  Each ADC module has 4 programmable sequences, sequence 0
    // to sequence 3.  This example is arbitrarily using sequence 3.
    //
    ADCSequenceConfigure(ADC0_BASE, 2, ADC_TRIGGER_PROCESSOR, 0);
    
    //
    // Configure step 0 on sequence 3.  Sample channel 0 (ADC_CTL_CH0) in
    // single-ended mode (default) and configure the interrupt flag
    // (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
    // that this is the last conversion on sequence 3 (ADC_CTL_END).  Sequence
    // 3 has only one programmable step.  Sequence 1 and 2 have 4 steps, and
    // sequence 0 has 8 programmable steps.  Since we are only doing a single
    // conversion using sequence 3 we will only configure step 0.  For more
    // information on the ADC sequences and steps, reference the datasheet.
    //
    ADCSequenceStepConfigure(ADC0_BASE, 2, 2, ADC_CTL_CH0 | ADC_CTL_IE |
                             ADC_CTL_END);
                             
    //
    // Since sample sequence 3 is now configured, it must be enabled.
    //
    ADCSequenceEnable(ADC0_BASE, 2);

    //
    // Clear the interrupt status flag.  This is done to make sure the
    // interrupt flag is cleared before we sample.
    //
    ADCIntClear(ADC0_BASE, 2);

/**
    HWREG(ADC0_BASE + ADC_O_EMUX) = 0xF000;
    HWREG(ADC0_BASE + ADC_O_SSMUX0) = 0x00;
    HWREG(ADC0_BASE + ADC_O_CTL) = 0x00;
    HWREG(ADC0_BASE + ADC_O_ACTSS) = 0x08;
    **/
    while(1)
    {
        ucState = ButtonsPoll(&ucDelta, 0);
        
        if(BUTTON_PRESSED(LEFT_BUTTON, ucState, ucDelta))
        {
            Key_status =  GPIOPinRead(IIC_PORT,SDA);
			UARTprintf("\n   systick = %d",SysCtlClockGet());
            UARTprintf("\n   key status = %d",Key_status);
        }
        else if(BUTTON_PRESSED(RIGHT_BUTTON, ucState, ucDelta))
        {
            //
            // Trigger the ADC conversion.
            //
            ADCProcessorTrigger(ADC0_BASE, 2);

            //
            // Wait for conversion to be completed.
            //
            while(!ADCIntStatus(ADC0_BASE, 2, false))
            {
            }

            //
            // Clear the ADC interrupt flag.
            //
            ADCIntClear(ADC0_BASE, 2);

            //
            // Read ADC Value.
            //
            ADCSequenceDataGet(ADC0_BASE, 2, pui32ADC0Value);

            //
            // Display the AIN0 (PE3) digital value on the console.
            //
            UARTprintf("\n AIN0 = %4d %4d\r", pui32ADC0Value[0],pui32ADC0Value[1]);
            //distence (/cm)
            OLED_P6x8data(64,0,pui32ADC0Value[0] * 3096/40960);
            
            ROM_SysCtlDelay(1000 * SysCtlClockGet()/3000);
        }
        
    }
    return 0;
}
