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
        }
    }
    return 0;
}
