/**
 *       @brief GPIO Library (ports A-F)
 *          Target Platform:    EK-TM4C123GXL
 *          Target uC:          TM4C123GH6PM
 *          Hardware:           16MHz XOSC
 *          Clock:              40Mhz
 *       @author Jason Losh
 **/

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "gpio.h"
#include "wait.h"

/*
    Bit offset of the registers relative to bit 0 of DATA_R at 3FCh
    reg offset x 4 bytes / reg x 8 bits / byte
*/
#define OFS_DATA_TO_DIR    1*4*8
#define OFS_DATA_TO_IS     2*4*8
#define OFS_DATA_TO_IBE    3*4*8
#define OFS_DATA_TO_IEV    4*4*8
#define OFS_DATA_TO_IM     5*4*8
#define OFS_DATA_TO_IC     8*4*8
#define OFS_DATA_TO_AFSEL  9*4*8
#define OFS_DATA_TO_ODR   68*4*8
#define OFS_DATA_TO_PUR   69*4*8
#define OFS_DATA_TO_PDR   70*4*8
#define OFS_DATA_TO_DEN   72*4*8
#define OFS_DATA_TO_CR    74*4*8
#define OFS_DATA_TO_AMSEL 75*4*8


/**
*      @brief Function to enable ports
*      @param port enumerated port address
**/
void enablePort(PORT port)
{
    switch(port)
    {
        case PORTA:
            SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;
            SYSCTL_GPIOHBCTL_R &= ~1;
            break;
        case PORTB:
            SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;
            SYSCTL_GPIOHBCTL_R &= ~2;
            break;
        case PORTC:
            SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R2;
            SYSCTL_GPIOHBCTL_R &= ~4;
            break;
        case PORTD:
            SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R3;
            SYSCTL_GPIOHBCTL_R &= ~8;
            break;
        case PORTE:
            SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;
            SYSCTL_GPIOHBCTL_R &= ~16;
            break;
        case PORTF:
            SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;
            SYSCTL_GPIOHBCTL_R &= ~32;
    }
    _delay_cycles(3);
}

/**
 *      @brief Function to disable ports
 *      @param port enumerated port address
 **/
void disablePort(PORT port)
{
    switch(port)
    {
        case PORTA:
            SYSCTL_RCGCGPIO_R &= ~SYSCTL_RCGCGPIO_R0;
            break;
        case PORTB:
            SYSCTL_RCGCGPIO_R &= ~SYSCTL_RCGCGPIO_R1;
            break;
        case PORTC:
            SYSCTL_RCGCGPIO_R &= ~SYSCTL_RCGCGPIO_R2;
            break;
        case PORTD:
            SYSCTL_RCGCGPIO_R &= ~SYSCTL_RCGCGPIO_R3;
            break;
        case PORTE:
            SYSCTL_RCGCGPIO_R &= ~SYSCTL_RCGCGPIO_R4;
            break;
        case PORTF:
            SYSCTL_RCGCGPIO_R &= ~SYSCTL_RCGCGPIO_R5;
    }
    _delay_cycles(3);
}

/**
*      @brief Function to configure particular pin as output
*      @param port enumerated port address
*      @param pin offset from the base of port address
**/
void selectPinPushPullOutput(PORT port, uint8_t pin)
{
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_ODR;
    *p = 0;
    p = (uint32_t*)port + pin + OFS_DATA_TO_DIR;
    *p = 1;
    p = (uint32_t*)port + pin + OFS_DATA_TO_DEN;
    *p = 1;
}

/**
*      @brief Function to configure pin as an open-drain output
*      @param port enumerated port address
*      @param pin offset from the base of port address
**/
void selectPinOpenDrainOutput(PORT port, uint8_t pin)
{
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_ODR;
    *p = 1;
    p = (uint32_t*)port + pin + OFS_DATA_TO_DIR;
    *p = 1;
    p = (uint32_t*)port + pin + OFS_DATA_TO_DEN;
    *p = 1;
}

/**
*      @brief Function to configure pin to accept digital inputs
*      @param port enumerated port address
*      @param pin offset from base of port address
**/
void selectPinDigitalInput(PORT port, uint8_t pin)
{
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_DIR;
    *p = 0;
    p = (uint32_t*)port + pin + OFS_DATA_TO_DEN;
    *p = 1;
    p = (uint32_t*)port + pin + OFS_DATA_TO_AMSEL;
    *p = 0;
}

/**
 *      @brief Function to configure pin to accept analog inputs
 *      @param port enumerated port address
 *      @param pin offset from base of port address
 **/
void selectPinAnalogInput(PORT port, uint8_t pin)
{
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_DEN;
    *p = 0;
    p = (uint32_t*)port + pin + OFS_DATA_TO_AMSEL;
    *p = 1;
    p = (uint32_t*)port + pin + OFS_DATA_TO_AFSEL;
    *p = 1;
}

void setPinCommitControl(PORT port, uint8_t pin)
{
    switch(port)
    {
        case PORTA:
            GPIO_PORTA_LOCK_R = GPIO_LOCK_KEY;
            break;
        case PORTB:
            GPIO_PORTB_LOCK_R = GPIO_LOCK_KEY;
            break;
        case PORTC:
            GPIO_PORTC_LOCK_R = GPIO_LOCK_KEY;
            break;
        case PORTD:
            GPIO_PORTD_LOCK_R = GPIO_LOCK_KEY;
            break;
        case PORTE:
            GPIO_PORTE_LOCK_R = GPIO_LOCK_KEY;
            break;
        case PORTF:
            GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
    }
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_CR;
    *p = 1;
}

/**
 *      @brief Function to enable internal pull-up on pin
 *      @param port enumerated port address
 *      @param pin offset from base of port address
 **/
void enablePinPullup(PORT port, uint8_t pin)
{
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_PUR;
    *p = 1;
}

/**
 *      @brief Function to disable internal pull-up on pin
 *      @param port enumerated port address
 *      @param pin offset from base of port address
 **/
void disablePinPullup(PORT port, uint8_t pin)
{
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_PUR;
    *p = 0;
}

/**
 *      @brief Function to enable internal pull-down on pin
 *      @param port enumerated port address
 *      @param pin offset from base of port address
 **/
void enablePinPulldown(PORT port, uint8_t pin)
{
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_PDR;
    *p = 1;
}

/**
 *      @brief Function to disable internal pull-down on pin
 *      @param port enumerated port address
 *      @param pin offset from base of port address
 **/
void disablePinPulldown(PORT port, uint8_t pin)
{
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_PDR;
    *p = 0;
}

/**
 *      @brief Function to enable alternative pin function from MUX
 *      @param port enumerated port address
 *      @param pin offset from base of port address
 *      @param fn alternative function to be enabled
 **/
void setPinAuxFunction(PORT port, uint8_t pin, uint32_t fn)
{
    // Call with header file shifted values or 4-bit number
    if (fn <= 15)
        fn = fn << (pin*4);
    else
        fn = fn & (0x0000000F << (pin*4));
    switch(port)
    {
        case PORTA:
            GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & ~(0x0000000F << (pin*4))) | fn;
            break;
        case PORTB:
            GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & ~(0x0000000F << (pin*4))) | fn;
            break;
        case PORTC:
            GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R & ~(0x0000000F << (pin*4))) | fn;
            break;
        case PORTD:
            GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R & ~(0x0000000F << (pin*4))) | fn;
            break;
        case PORTE:
            GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R & ~(0x0000000F << (pin*4))) | fn;
            break;
        case PORTF:
            GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R & ~(0x0000000F << (pin*4))) | fn;
    }
    // Set AFSEL bit only if using aux function, otherwise clear bit
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_AFSEL;
    *p = (fn > 0);
}

/**
*      @brief Function to configure pin to trigger interrupt on rising edge of input signal
*      @param port enumerated port address
*      @param pin offset from base of port address
**/
void selectPinInterruptRisingEdge(PORT port, uint8_t pin)
{
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_IS;
    *p = 0;
    p = (uint32_t*)port + pin + OFS_DATA_TO_IBE;
    *p = 0;
    p = (uint32_t*)port + pin + OFS_DATA_TO_IEV;
    *p = 1;
}

/**
 *      @brief Function to configure pin to trigger interrupt on falling edge of input signal
 *      @param port enumerated port address
 *      @param pin offset from base of port address
 **/
void selectPinInterruptFallingEdge(PORT port, uint8_t pin)
{
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_IS;
    *p = 0;
    p = (uint32_t*)port + pin + OFS_DATA_TO_IBE;
    *p = 0;
    p = (uint32_t*)port + pin + OFS_DATA_TO_IEV;
    *p = 0;
}

/**
 *      @brief Function to configure pin to trigger interrupt on both rising and falling edges of input signal
 *      @param port enumerated port address
 *      @param pin offset from base of port address
 **/
void selectPinInterruptBothEdges(PORT port, uint8_t pin)
{
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_IS;
    *p = 0;
    p = (uint32_t*)port + pin + OFS_DATA_TO_IBE;
    *p = 1;
}

/**
 *      @brief Function to configure pin to trigger interrupt on high level of input signal
 *      @param port enumerated port address
 *      @param pin offset from base of port address
 **/
void selectPinInterruptHighLevel(PORT port, uint8_t pin)
{
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_IS;
    *p = 1;
    p = (uint32_t*)port + pin + OFS_DATA_TO_IEV;
    *p = 1;
}

/**
 *      @brief Function to configure pin to trigger interrupt on low level of input signal
 *      @param port enumerated port address
 *      @param pin offset from base of port address
 **/
void selectPinInterruptLowLevel(PORT port, uint8_t pin)
{
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_IS;
    *p = 1;
    p = (uint32_t*)port + pin + OFS_DATA_TO_IEV;
    *p = 0;
}

/**
 *      @brief Function to enable pin to generate interrupts
 *      @param port enumerated port address
 *      @param pin offset from base of port address
 **/
void enablePinInterrupt(PORT port, uint8_t pin)
{
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_IM;
    *p = 1;
}

/**
 *      @brief Function to disable pin to generate interrupts
 *      @param port enumerated port address
 *      @param pin offset from base of port address
 **/
void disablePinInterrupt(PORT port, uint8_t pin)
{
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_IM;
    *p = 0;
}

/**
 *      @brief Function to clear interrupt generated on pin
 *      @param port enumerated port address
 *      @param pin offset from base of port address
 **/
void clearPinInterrupt(PORT port, uint8_t pin)
{
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_IC;
    *p = 1;
}

/**
 *      @brief Function to set high value on a pin
 *      @param port enumerated port address
 *      @param pin offset from base of port address
 **/
void setPinValue(PORT port, uint8_t pin, bool value)
{
    uint32_t* p;
    p = (uint32_t*)port + pin;
    *p = value;
}

/**
 *      @brief Function to read state of pin
 *      @param port enumerated port address
 *      @param pin offset from base of port address
 **/
bool getPinValue(PORT port, uint8_t pin)
{
    uint32_t* p;
    p = (uint32_t*)port + pin;
    return *p;
}

/**
 *      @brief Function to set value of a port
 *      @param port enumerated port address
 *      @param value value to be set
 **/
void setPortValue(PORT port, uint8_t value)
{
    switch(port)
    {
        case PORTA:
            GPIO_PORTA_DATA_R = value;
            break;
        case PORTB:
            GPIO_PORTB_DATA_R = value;
            break;
        case PORTC:
            GPIO_PORTC_DATA_R = value;
            break;
        case PORTD:
            GPIO_PORTD_DATA_R = value;
            break;
        case PORTE:
            GPIO_PORTE_DATA_R = value;
            break;
        case PORTF:
            GPIO_PORTF_DATA_R = value;
    }
}

/**
 *      @brief Function to read the value of a port
 *      @param port enumerated port address
 **/
uint8_t getPortValue(PORT port)
{
    uint8_t value;
    switch(port)
    {
        case PORTA:
            value = GPIO_PORTA_DATA_R;
            break;
        case PORTB:
            value = GPIO_PORTB_DATA_R;
            break;
        case PORTC:
            value = GPIO_PORTC_DATA_R;
            break;
        case PORTD:
            value = GPIO_PORTD_DATA_R;
            break;
        case PORTE:
            value = GPIO_PORTE_DATA_R;
            break;
        case PORTF:
            value = GPIO_PORTF_DATA_R;
    }
    return value;
}
