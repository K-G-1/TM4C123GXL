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
    unsigned long ulColors[3];
    //
    // Initialize the rgb driver.
    //
    RGBInit(0);
    //
    // Set the intensity level from 0.0f to 1.0f
    //
    RGBIntensitySet(0.3f);
    //
    // Initialize the three color values.
    //
    ulColors[BLUE] = 0x00FF;
    ulColors[RED] = 0xFFFF;
    ulColors[GREEN] = 0x0000;
    RGBColorSet(ulColors);
    //
    // Enable the RGB. This configure GPIOs to the Timer PWM mode needed
    // to generate the color spectrum.
    //
    RGBEnable();
    //
    // Application may now call RGB API to suit program requirements.
    //
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
