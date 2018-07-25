#include <stdint.h>
#include "tm4c123gh6pm.h"

int main(void)
{
    uint32_t ulLoop;
    SYSCTL_GPIOHBCTL_R = SYSCTL_GPIOHBCTL_PORTF;
    SYSCTL_RCGCGPIO_R = SYSCTL_RCGCGPIO_R5;
    
    GPIO_PORTF_AHB_DIR_R = 0x0000000E;
    
    GPIO_PORTF_AHB_DEN_R = 0x0000000e;
    //QQ:957902197
    while(1)
    {
        for(ulLoop = 0;ulLoop<2000000;ulLoop ++)
            GPIO_PORTF_AHB_DATA_R = 0x00000002;//red
        for(ulLoop = 0;ulLoop<2000000;ulLoop ++)
            GPIO_PORTF_AHB_DATA_R = 0x00000004;//green
        for(ulLoop = 0;ulLoop<2000000;ulLoop ++)
            GPIO_PORTF_AHB_DATA_R = 0x00000008;//blue
        
    }
    
}

