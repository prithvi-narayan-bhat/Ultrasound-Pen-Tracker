/**
 *       @brief LCD Library
 *          Target Platform:    EK-TM4C123GXL with PCF8574 (20x4 LCD)
 *          Target uC:          TM4C123GH6PM
 *          Hardware:           2k ohm pullup on SDA and SCL
 *                              HD44780-based 16x2, 20x2, 16x4 20x4 LCD display
 *                              Display driven by PCF8574 I2C 8-bit I/O expander at address 0x27
 *                              Display RS, R/W, E, backlight enable, and D4-7 connected to PCF8574 P0-7
 *          Clock:              40Mhz
 *       @author Jason Losh
 **/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "i2c.h"
#include "lcd.h"
#include "wait.h"

#define LCD_ADD 0x27
#define LCD_RS 1
#define LCD_RW 2
#define LCD_E  4
#define LCD_BACKLIGHT 8

void writeTextLcdCommand(uint8_t command)
{
    writeI2cByte(I2C0, LCD_ADD, SINGLE_REG_DEV, (command & 0xF0) | LCD_E | LCD_BACKLIGHT);
    writeI2cByte(I2C0, LCD_ADD, SINGLE_REG_DEV, (command & 0xF0) | LCD_BACKLIGHT);
    writeI2cByte(I2C0, LCD_ADD, SINGLE_REG_DEV, (command << 4) | LCD_E | LCD_BACKLIGHT);
    writeI2cByte(I2C0, LCD_ADD, SINGLE_REG_DEV, (command << 4) | LCD_BACKLIGHT);
}

void writeTextLcdData(char c)
{
    writeI2cByte(I2C0, LCD_ADD, SINGLE_REG_DEV, (c & 0xF0) | LCD_E | LCD_RS | LCD_BACKLIGHT);
    writeI2cByte(I2C0, LCD_ADD, SINGLE_REG_DEV, (c & 0xF0) | LCD_RS | LCD_BACKLIGHT);
    writeI2cByte(I2C0, LCD_ADD, SINGLE_REG_DEV, (c << 4) | LCD_E | LCD_RS | LCD_BACKLIGHT);
    writeI2cByte(I2C0, LCD_ADD, SINGLE_REG_DEV, (c << 4) | LCD_RS | LCD_BACKLIGHT);
}

void initLcd()
{
    initI2c(I2C0, I2C_MODE_STD);            // Initialize I2C 0 in standard mode (100kBps)

    waitMicrosecond(2000);                  // Wait for device to reset

    /*
        Set to 4-bit interface mode (single E cycle)
        Note: If device was not reset, the device could already be in 4-bit mode
           If this is the case, then this single E cycle will corrupt phase of writes
           and the display will likely only initialize correctly every other time
    */
    writeI2cByte(I2C0, LCD_ADD, SINGLE_REG_DEV, 0x20 | LCD_E);
    writeI2cByte(I2C0, LCD_ADD, SINGLE_REG_DEV, 0x20);

    // Continue configuration using dual E cycle (2 nibble) writes
    writeTextLcdCommand(0x28); // 4-bit interface, 2 lines, 5x8 font
    writeTextLcdCommand(0x0C); // display on, no cursor, no blink
    writeTextLcdCommand(0x06); // shift cursor to right after writes
}

void putsLcd(uint8_t row, uint8_t col, const char str[])
{
    uint8_t i = 0;
    writeTextLcdCommand(0x80 + (row & 1) * 64 + (row & 2) * 10 + col);
    while (str[i] != '\0')
        writeTextLcdData(str[i++]);
}
