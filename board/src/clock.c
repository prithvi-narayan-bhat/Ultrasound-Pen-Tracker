/**
 *       @brief Clock Library
 *          Target Platform:    EK-TM4C123GXL
 *          Target uC:          TM4C123GH6PM
 *          Hardware:           16MHz XOSC
 *          Clock:              40Mhz
 *       @author Jason Losh
 **/

#include <stdint.h>
#include "clock.h"
#include "tm4c123gh6pm.h"

// Initialize system clock to 40 MHz using PLL and 16 MHz crystal oscillator
void initSystemClockTo40Mhz(void)
{
    // Configure HW to work with 16 MHz XTAL, PLL enabled, sysdivider of 5, creating system clock of 40 MHz
    SYSCTL_RCC_R = SYSCTL_RCC_XTAL_16MHZ | SYSCTL_RCC_OSCSRC_MAIN | SYSCTL_RCC_USESYSDIV | (4 << SYSCTL_RCC_SYSDIV_S);
}
