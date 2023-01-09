// I2C0 LCD Library
// Jason Losh

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL with LCD/Keyboard Interface
// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

// Hardware configuration:
// HD44780-based 16x2, 20x2, 16x4 20x4 LCD display
// Display driven by PCF8574 I2C 8-bit I/O expander at address 0x27
// I2C devices on I2C bus 0 with 2kohm pullups on SDA and SCL
// Display RS, R/W, E, backlight enable, and D4-7 connected to PCF8574 P0-7

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#ifndef I2C0_LCD_H_
#define I2C0_LCD_H_

#include <stdint.h>
#include <stdbool.h>

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

void initLcd();
void putsLcd(uint8_t row, uint8_t col, const char str[]);

#endif

