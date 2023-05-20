/**
 *       @brief Clock Library
 *          Target Platform:    EK-TM4C123GXL
 *          Target uC:          TM4C123GH6PM
 *          Hardware:           16MHz XOSC
 *          Clock:              40Mhz
 *       @author Prithvi Bhat (with references from Jason Losh's Library)
 **/

#include <stdint.h>
#include "clock.h"
#include "tm4c123gh6pm.h"

// Initialize system clock to 40 MHz using PLL and 16 MHz crystal oscillator
void initSystemClock(sysClk_t sysClk)
{
    switch(sysClk)
    {
        case (SYS_CLK_40MHZ):
        {
            // Configure HW to work with 16 MHz XTAL, PLL enabled, sysdivider of 5, creating system clock of 40 MHz
            SYSCTL_RCC_R = SYSCTL_RCC_XTAL_16MHZ | SYSCTL_RCC_OSCSRC_MAIN | SYSCTL_RCC_USESYSDIV | (4 << SYSCTL_RCC_SYSDIV_S);
            break;
        }
    }
}
