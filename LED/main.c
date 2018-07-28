#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
//加快速度
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"

#define RED_LED  GPIO_PIN_1
#define BLUE_LED  GPIO_PIN_2
#define GREN_LED  GPIO_PIN_3
#define BLUE_LED_H             GPIOPinWrite(GPIO_PORTF_BASE,BLUE_LED,(BLUE_LED))
#define BLUE_LED_L               GPIOPinWrite(GPIO_PORTF_BASE,BLUE_LED,~(BLUE_LED))
#define SYS_CLOCK 16000000
int main()
{
    //
    // Set the clocking to run directly from the crystal.
    //
//    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
//                       SYSCTL_XTAL_16MHZ);
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                       SYSCTL_OSC_MAIN);
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RED_LED);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, BLUE_LED);
    

   
    
//    GPIOPadConfigSet(SYSCTL_PERIPH_GPIOF, GPIO_PIN_2, GPIO_STRENGTH_8MA,
//                     GPIO_PIN_TYPE_STD);  
//    GPIOPadConfigSet(SYSCTL_PERIPH_GPIOF, GPIO_PIN_3, GPIO_STRENGTH_8MA,
//                     GPIO_PIN_TYPE_STD);
    
    while(1)
    {
        GPIOPinWrite(GPIO_PORTF_BASE, RED_LED, (RED_LED));
        //得到系统时钟，3:系统3个周期去运行汇编，为1us的计时，3000为1ms的计时 
        SysCtlDelay(2000*(SysCtlClockGet()/3000));
        GPIOPinWrite(GPIO_PORTF_BASE, RED_LED, ~(RED_LED));
        SysCtlDelay(2000*(SysCtlClockGet()/3000));

        BLUE_LED_H;
        SysCtlDelay(2000*(SysCtlClockGet()/3000));
        BLUE_LED_L;
        SysCtlDelay(3000 * (SysCtlClockGet() / 3000));    //   ??150ms
    }
    
    
    return 0;
}
