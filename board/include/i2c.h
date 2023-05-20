/**
 *       @brief I2C Library
 *          Target Platform:    EK-TM4C123GXL
 *          Target uC:          TM4C123GH6PM
 *          Hardware:           2k ohm pullup on SDA and SCL
 *          Clock:              40Mhz
 *       @author Prithvi Bhat (with reference from Jason Losh's library)
 **/

#ifndef I2C0_H
#define I2C0_H

#include <stdint.h>
#include <stdbool.h>

#define isI2cError(channel)     ((channel##_MCS_R & I2C_MCS_ERROR) ? 1 : 0)
#define SINGLE_REG_DEV  0xFF

typedef enum
{
    I2C0 = 0,
    I2C1 = 1,
    I2C2 = 2,
    I2C3 = 3,
} i2cChannel_t;

typedef enum
{
    I2C_MODE_STD = 100,
    I2C_MODE_FAM = 400,
    I2C_MODE_FMP = 1000,
    I2C_MODE_HSM = 3330,
} i2cMode_t;

void initI2c(i2cChannel_t i2cChannel, i2cMode_t mode);
void writeI2cByte(i2cChannel_t i2cChannel, uint8_t devAddress, uint8_t regAddress, uint8_t data);
uint8_t readI2cByte(i2cChannel_t i2cChannel, uint8_t devAddress, uint8_t regAddress);
bool pollI2cDevice(i2cChannel_t i2cChannel, uint8_t devAddress);
void writeI2cArray(i2cChannel_t i2cChannel, uint8_t devAddress, uint8_t regAddress, const uint8_t data[], uint8_t size);
void readI2c0Registers(uint8_t devAddress, uint8_t regAddress, uint8_t data[], uint8_t size);

#endif
