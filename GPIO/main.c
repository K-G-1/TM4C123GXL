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

#define SYS_CLOCK 16000000
int main()
{
    //
    // Set the clocking to run directly from the crystal.
    //
//    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
//                       SYSCTL_XTAL_16MHZ);
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                       SYSCTL_OSC_MAIN);
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    
    //
    // Configure the GPIO port for the LED operation.
    //
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RED_LED);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, BLUE_LED);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GREN_LED);
    
    while(1)
    {
        GPIOPinWrite(GPIO_PORTF_BASE, BLUE_LED|RED_LED|GREN_LED, (RED_LED));
        //得到系统时钟，3:系统3个周期去运行汇编，为1us的计时，3000为1ms的计时 
        SysCtlDelay(1000*(SysCtlClockGet()/3000));

        GPIOPinWrite(GPIO_PORTF_BASE, BLUE_LED|RED_LED|GREN_LED, (BLUE_LED));
        SysCtlDelay(2000*(SysCtlClockGet()/3000));
        GPIOPinWrite(GPIO_PORTF_BASE, BLUE_LED|RED_LED|GREN_LED, (GREN_LED));
        SysCtlDelay(3000 * (SysCtlClockGet() / 3000));    //   ??150ms
    }
    
    
    return 0;
}
