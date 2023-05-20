/**
 *       @brief I2C Library
 *          Target Platform:    EK-TM4C123GXL
 *          Target uC:          TM4C123GH6PM
 *          Hardware:           2k ohm pullup on SDA and SCL
 *          Clock:              40Mhz
 *       @author Prithvi Bhat (with reference from Jason Losh's library)
 **/

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "gpio.h"
#include "i2c.h"
#include "wait.h"

#define SYSTEM_CLOCK_SPEED 400000000    // 40MHz

#define I2C0SCL PORTB,2                 // I2C0 Cock pin
#define I2C0SDA PORTB,3                 // I2C0 Data pin

#define I2C1SCL PORTA,6                 // I2C1 Cock pin
#define I2C1SDA PORTA,7                 // I2C1 Data pin

#define I2C2SCL PORTE,4                 // I2C2 Cock pin
#define I2C2SDA PORTE,5                 // I2C2 Data pin

#define I2C3SCL PORTD,0                 // I2C3 Cock pin
#define I2C3SDA PORTD,1                 // I2C3 Data pin

typedef enum
{
    PB2,
    PB3,
    PA6,
    PA7,
    PE4,
    PE5,
    PD0,
    PD1
} i2cPorts_t;

/**
*      @brief Function to initialise I2C registers and ports
*      @param i2cChannel I2C channel to be initialzed
*      @param mode I2C operating mode
**/
void initI2c(i2cChannel_t i2cChannel, i2cMode_t mode)
{
    SYSCTL_RCGCI2C_R |= SYSCTL_RCGCI2C_R0;                      // Enable clock to I2C module
    _delay_cycles(3);                                           // Delay to sync

/**
 *      @brief Macro to configure I2C registers and ports
 *      @param channel I2C channel to initialize
 *      @param sclPin MCU pin associated with clock for I2C channel
 *      @param sdaPin MCU pin associated with data for I2C channel
 **/
#ifndef i2cConfig
#define i2cConfig(channel, sclPin, sdaPin)                                        \
    ({                                                                            \
        selectPinPushPullOutput(channel##SCL);                                    \
        setPinAuxFunction(channel##SCL, GPIO_PCTL_##sclPin##_##channel##SCL);     \
        selectPinOpenDrainOutput(channel##SDA);                                   \
        setPinAuxFunction(channel##SDA, GPIO_PCTL_##sdaPin##_##channel##SDA);     \
        I2C0_MCR_R = 0;                                                           \
        /* ((System Clock/(2*(SCL_LP + SCL_HP)*SCL_CLK))-1;) */                   \
        I2C0_MTPR_R = ((SYSTEM_CLOCK_SPEED / (2 * (6 + 4) * (mode * 1000))) - 1); \
        I2C0_MCR_R = I2C_MCR_MFE;                                                 \
        I2C0_MCS_R = I2C_MCS_STOP;                                                \
        break;                                                                    \
    })
#endif

    switch (i2cChannel)
    {
        case I2C0:
        {
            enablePort(PORTB);          // Enable port
            i2cConfig(I2C0, PB2, PB3);  // Configure I2C registers
        }

        case I2C1:
        {
            enablePort(PORTA);          // Enable port
            i2cConfig(I2C1, PA6, PA7);  // Configure I2C registers
        }

        case I2C2:
        {
            enablePort(PORTE);          // Enable port
            i2cConfig(I2C2, PE4, PE5);  // Configure I2C registers
        }

        case I2C3:
        {
            enablePort(PORTD);          // Enable port
            i2cConfig(I2C3, PD0, PD1);  // Configure I2C registers
        }
    }
#undef i2cConfig
}

/**
 *      @brief Function to write a byte into I2C devices
 *      @param i2cChannel Channel on which the device is connected
 *      @param devAddress slave device address
 *      @param regAddress register in the slave device (pass 0xFF if single register device)
 *      @param data byte data to be written
 **/
void writeI2cByte(i2cChannel_t i2cChannel, uint8_t devAddress, uint8_t regAddress, uint8_t data)
{

/**
 *      @brief Macro to write byte into an I2C device
 *      @param channel i2c channel to write into
 *      @param devAddress slave device address
 *      @param data data to be written
 **/
#ifndef writeByte
#define writeByte(channel, devAddress, regAddress, data)    \
    ({                                                      \
        channel##_MSA_R = devAddress << 1;                  \
        if(regAddress != 0xFF)                              \
        {                                                   \
            channel##_MDR_R = regAddress;                   \
            channel##_MICR_R = I2C_MICR_IC;                 \
            channel##_MCS_R = I2C_MCS_START | I2C_MCS_RUN;  \
            while (!(channel##_MRIS_R & I2C_MRIS_RIS));     \
        }                                                   \
        channel##_MDR_R = data;                             \
        channel##_MICR_R = I2C_MICR_IC;                     \
        channel##_MCS_R = I2C_MCS_RUN | I2C_MCS_STOP;       \
        while (!(channel##_MRIS_R & I2C_MRIS_RIS));         \
        break;                                              \
    })
#endif

    switch (i2cChannel)
    {
        case(I2C0): writeByte(I2C0, devAddress, regAddress, data);
        case(I2C1): writeByte(I2C1, devAddress, regAddress, data);
        case(I2C2): writeByte(I2C2, devAddress, regAddress, data);
        case(I2C3): writeByte(I2C3, devAddress, regAddress, data);
    }
#undef writeByte
}

/**
 *      @brief Function to read a byte from I2C devices
 *      @param i2cChannel Channel on which the device is connected
 *      @param devAddress slave device address
 *      @param regAddress address of the register on the slave device
 *      @return uint8_t byte read from slave device
 **/
uint8_t readI2cByte(i2cChannel_t i2cChannel, uint8_t devAddress, uint8_t regAddress)
{
/**
 *      @brief Macro to read byte from a simple single register device
 *      @param channel i2c channel to read from
 *      @param devAddress slave device address
 **/
#ifndef readByte
#define readByte(channel, devAddress, regAddress)                       \
    ({                                                                  \
        if(regAddress != 0xFF)                                          \
        {                                                               \
            channel##_MSA_R = (devAddress << 1);                        \
            channel##_MDR_R = regAddress;                               \
            channel##_MICR_R = I2C_MICR_IC;                             \
            channel##_MCS_R = I2C_MCS_START | I2C_MCS_RUN;              \
            while (!(channel##_MRIS_R & I2C_MRIS_RIS));                 \
        }                                                               \
        channel##_MSA_R = (devAddress << 1) | 1;                        \
        channel##_MICR_R = I2C_MICR_IC;                                 \
        channel##_MCS_R = I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP;   \
        while (!(channel##_MRIS_R & I2C_MRIS_RIS));                     \
        return channel##_MDR_R;                                         \
        break;                                                          \
    })
#endif

    switch (i2cChannel)
    {
        case(I2C0): readByte(I2C0, devAddress, regAddress);
        case(I2C1): readByte(I2C1, devAddress, regAddress);
        case(I2C2): readByte(I2C2, devAddress, regAddress);
        case(I2C3): readByte(I2C3, devAddress, regAddress);
    }
#undef readByte
}

/**
 *      @brief Function to write an array of data into registers on an I2C device
 *      @param i2cChannel Channel on which the device is connected
 *      @param devAddress slave device address
 *      @param regAddress address of the register on the slave device
 *      @param data array of data to be written in
 *      @param size size of data to be written
 **/
void writeI2cArray(i2cChannel_t i2cChannel, uint8_t devAddress, uint8_t regAddress, const uint8_t data[], uint8_t size)
{
/**
 *      @brief Macro to write an array of data into an I2C device
 *      @param channel i2c channel to write into
 *      @param devAddress slave device address
 *      @param regAddress address of the register on the slave device
 *      @param data array of data to be written in
 *      @param size size of data to be written
 **/
#ifndef writeArray
#define writeArray(channel, devAddress, regAddress, data, size)             \
    ({                                                                      \
        /* Send address and register number */                              \
        channel##_MSA_R = devAddress << 1;                                  \
        channel##_MDR_R = regAddress;                                       \
        if (!size)                                                          \
        {                                                                   \
            channel##_MICR_R = I2C_MICR_IC;                                 \
            channel##_MCS_R = I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP;   \
            while (!(channel##_MRIS_R & I2C_MRIS_RIS));                     \
        }                                                                   \
        else                                                                \
        {                                                                   \
            channel##_MICR_R = I2C_MICR_IC;                                 \
            channel##_MCS_R = I2C_MCS_START | I2C_MCS_RUN;                  \
            while (!(channel##_MRIS_R & I2C_MRIS_RIS));                     \
            /* Write size-1 bytes with ack */                               \
            for (uint8_t i = 0; i < size - 1; i++)                          \
            {                                                               \
                channel##_MDR_R = data[i];                                  \
                channel##_MICR_R = I2C_MICR_IC;                             \
                channel##_MCS_R = I2C_MCS_RUN;                              \
                while (!(channel##_MRIS_R & I2C_MRIS_RIS));                 \
            }                                                               \
            /* Last byte of write with nack */                              \
            channel##_MDR_R = data[size-1];                                 \
            channel##_MICR_R = I2C_MICR_IC;                                 \
            channel##_MCS_R = I2C_MCS_RUN | I2C_MCS_STOP;                   \
            while (!(channel##_MRIS_R & I2C_MRIS_RIS));                     \
        }                                                                   \
        break;                                                              \
    })
#endif

    switch (i2cChannel)
    {
        case(I2C0): writeArray(I2C0, devAddress, regAddress, data, size);
        case(I2C1): writeArray(I2C1, devAddress, regAddress, data, size);
        case(I2C2): writeArray(I2C2, devAddress, regAddress, data, size);
        case(I2C3): writeArray(I2C3, devAddress, regAddress, data, size);
    }

#undef writeArray
}

/**
 *      @brief Function to read an array of data from registers on an I2C device
 *      @param i2cChannel Channel on which the device is connected
 *      @param devAddress slave device address
 *      @param regAddress address of the register on the slave device
 *      @param data array of to store read data
 *      @param size size of data to be read
 **/
void readI2cArray(i2cChannel_t i2cChannel, uint8_t devAddress, uint8_t regAddress, uint8_t data[], uint8_t size)
{
    uint8_t i = 0;

#ifndef readArray
#define readArray(channel, devAddress, regAddress, data, size)              \
    i = 0;                                                                  \
    ({                                                                      \
        channel##_MSA_R = devAddress << 1;                                  \
        channel##_MDR_R = regAddress;                                       \
        channel##_MICR_R = I2C_MICR_IC;                                     \
        channel##_MCS_R = I2C_MCS_START | I2C_MCS_RUN;                      \
        while ((channel##_MRIS_R & I2C_MRIS_RIS) == 0);                     \
        if (size == 1)                                                      \
        {                                                                   \
            /* devAddress and read one byte */                              \
            channel##_MSA_R = (devAddress << 1) | 1;                        \
            channel##_MICR_R = I2C_MICR_IC;                                 \
            channel##_MCS_R = I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP;   \
            while (!(channel##_MRIS_R & I2C_MRIS_RIS));                     \
            data[i++] = channel##_MDR_R;                                    \
        }                                                                   \
        else if (size > 1)                                                  \
        {                                                                   \
            /* devAddress and first byte of read with ack */                \
            channel##_MSA_R = (devAddress << 1) | 1;                        \
            channel##_MICR_R = I2C_MICR_IC;                                 \
            channel##_MCS_R = I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_ACK;    \
            while (!(channel##_MRIS_R & I2C_MRIS_RIS));                     \
            data[i++] = channel##_MDR_R;                                    \
            /* Read size-2 bytes with ack */                                \
            while (i < size-1)                                              \
            {                                                               \
                channel##_MICR_R = I2C_MICR_IC;                             \
                channel##_MCS_R = I2C_MCS_RUN | I2C_MCS_ACK;                \
                while (!(channel##_MRIS_R & I2C_MRIS_RIS));                 \
                data[i++] = channel##_MDR_R;                                \
            }                                                               \
            /* Last byte of read with nack */                               \
            channel##_MICR_R = I2C_MICR_IC;                                 \
            channel##_MCS_R = I2C_MCS_RUN | I2C_MCS_STOP;                   \
            while (!(channel##_MRIS_R & I2C_MRIS_RIS));                     \
            data[i++] = channel##_MDR_R;                                    \
        }                                                                   \
    })
#endif

    switch (i2cChannel)
    {
        case(I2C0): readArray(I2C0, devAddress, regAddress, data, size);
        case(I2C1): readArray(I2C1, devAddress, regAddress, data, size);
        case(I2C2): readArray(I2C2, devAddress, regAddress, data, size);
        case(I2C3): readArray(I2C3, devAddress, regAddress, data, size);
    }
#undef readArray

}

/**
 *      @brief Function to determine if I2C device is connected and alive on a channel
 *      @param i2cChannel channel to probe on
 *      @param devAddress device address to look for
 *      @return true if device is connected and alive
 *      @return false if device is disconnected or dead
 **/
bool pollI2cDevice(i2cChannel_t i2cChannel, uint8_t devAddress)
{
/**
 *      @brief Macro to poll for a device
 *      @param channel i2c channel to read from
 *      @param devAddress slave device address
 **/
#ifndef pollDevice
#define pollDevice(channel, devAddress)                                 \
    ({                                                                  \
        channel##_MSA_R = (devAddress << 1) | 1;                        \
        channel##_MICR_R = I2C_MICR_IC;                                 \
        channel##_MCS_R = I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP;   \
        while (!(channel##_MRIS_R & I2C_MRIS_RIS));                     \
        return !(channel##_MCS_R & I2C_MCS_ERROR);                      \
    })
#endif

    switch (i2cChannel)
    {
        case(I2C0): pollDevice(I2C0, devAddress);
        case(I2C1): pollDevice(I2C1, devAddress);
        case(I2C2): pollDevice(I2C2, devAddress);
        case(I2C3): pollDevice(I2C3, devAddress);
    }
}