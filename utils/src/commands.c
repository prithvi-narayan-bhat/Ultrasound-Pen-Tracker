/**
*      @file commands.c
*      @author Prithvi Bhat
*      @brief File that handles functions relating terminal commands
*      @date 2022-12-04
**/

#include "eeprom.h"
#include "eeprom_memory_map.h"
#include "strings.h"
#include "commands.h"
#include "timer.h"
#include "tm4c123gh6pm.h"
#include <stdio.h>
#include "wait.h"
#include "lcd.h"
#include <stdlib.h>
#include <math.h>

#define CONVERSION_CONSTANT 0.008575        // ((1 / (40e6)) * 1000 * 343) to convert time register value to mm
#define ASSERT(value)       if (value <= 1 || value > MAX_AVERAGES)   value = 1;

// Global Variables
double g_average_A, g_average_B, g_average_C;
uint32_t g_distance_A, g_distance_B, g_distance_C;
bool g_values_acceptable = false;

/**
*      @brief Macro to load PWM values into appropriate register
**/
#define LOAD_PWM(load, sleep)               \
    ({                                      \
        PWM1_0_LOAD_R = load;               \
        PWM1_0_CMPB_R = PWM1_0_LOAD_R / 2;  \
        waitMicrosecond(sleep);             \
    })

/**
*      @brief reverses a string 'str' of length 'len'
*      @param str string to reverse
*      @param len length of string
*      @ref https://tutorialspoint.dev/language/c/convert-floating-point-number-string
**/
void reverse(char *str, int len)
{
    int i = 0, j = len - 1, temp;
    while (i < j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

/**
*      @brief Converts an integer x to string
*      @param number to be converted
*      @param str string to store value in
*      @param digits number of digits required in output
*      @return int number of digits converted
*      @ref https://tutorialspoint.dev/language/c/convert-floating-point-number-string
**/
int intToStr(int number, char str[], int digits)
{
    int i = 0;

    while (number)
    {
        str[i++] = (number % 10) + '0';
        number = number / 10;
    }
    while (i < digits)   str[i++] = '0';                        // Add 0s at the beginning if number of digits exceeds the length of integer

    reverse(str, i);
    str[i] = '\0';

    return i;
}

/**
*      @brief Function to convert floating point number to string
*      @param number to be converted
*      @param destination string
*      @param float_length
*      @ref https://tutorialspoint.dev/language/c/convert-floating-point-number-string
**/
void ftoa(float number, char *destination, int float_length)
{
    int whole_number = (int)number;                             // Extract integer part

    float fractional_number = number - (float)whole_number;     // Extract floating part

    int i = intToStr(whole_number, destination, 0);             // Convert integer part to string

    if (float_length != 0)                                      // Check for display option after point
    {
        destination[i] = '.';                                   // Add decimal to string

        fractional_number = fractional_number * pow(10, float_length);
        intToStr((int)fractional_number, destination + i + 1, float_length);
    }
}

/**
*      @brief Function to beep
*      @param beep_type
**/
void beep_now(beep_t beep_type)
{
    uint32_t i = 1, count = 1;
    switch (beep_type)
    {
        case BEEP_IR_INT:
        {
            count = readEeprom(CONT_IR);
            for (i = 1; i <= count; i++)
            {
                LOAD_PWM(readEeprom(LOAD_IR), readEeprom(PER1_IR));
                LOAD_PWM(0, readEeprom(PER2_IR));
            }
            break;
        }

        case BEEP_US_A_INT:
        {
            count = readEeprom(CONT_A);
            for (i = 1; i <= count; i++)
            {
                LOAD_PWM(readEeprom(LOAD_A), readEeprom(PER1_A));
                LOAD_PWM(0, readEeprom(PER2_A));
            }
            break;
        }

        case BEEP_US_B_INT:
        {
            count = readEeprom(CONT_B);
            for (i = 1; i <= count; i++)
            {
                LOAD_PWM(readEeprom(LOAD_B), readEeprom(PER1_B));
                LOAD_PWM(0, readEeprom(PER2_B));
            }
            break;
        }

        case BEEP_US_C_INT:
        {
            count = readEeprom(CONT_C);
            for (i = 1; i <= count; i++)
            {
                LOAD_PWM(readEeprom(LOAD_C), readEeprom(PER1_C));
                LOAD_PWM(0, readEeprom(PER2_C));
            }
            break;
        }

        case BEEP_ERROR:
        {
            count = readEeprom(CONT_ERR);

            for (i = 1; i <= count; i++)
            {
                LOAD_PWM(readEeprom(LOAD_ERR), readEeprom(PER1_ERR));
                LOAD_PWM(0, readEeprom(PER2_ERR));
            }
            break;
        }

        case BEEP_START:
            LOAD_PWM(0, 100000);
    }
}


/**
*      @brief Function to update Sensor Coordinates as input by user
*      @param sensor One of three possibilities A, B or C
*      @param x coordinate
*      @param y coordinate
**/
void update_sensor_coordinates(char *sensor, uint32_t x, uint32_t y)
{
    switch (*sensor)
    {
        case 'A':
        case 'a':
        {
            writeEeprom(CRD_AX, x);                 // Write S1 x coordinate at address 0x00
            writeEeprom(CRD_AY, y);                 // Write S1 y coordinate at address 0x32
            break;
        }

        case 'B':
        case 'b':
        {
            writeEeprom(CRD_BX, x);                 // Write S2 x coordinates at address 0x64
            writeEeprom(CRD_BY, y);                 // Write S2 y coordinates at address 0x96
            break;
        }

        case 'C':
        case 'c':
        {
            writeEeprom(CRD_CX, x);                 // Write S3 x coordinates at address 0x128
            writeEeprom(CRD_CY, y);                 // Write S3 y coordinates at address 0x160
            break;
        }

        default:
        {
            putsUart0("ERROR! Invalid Sensor ID\r\nExpected format: \"sensor, A, 0, 0\"\r\n");
            return;
        }
    }

    putsUart0("Sensor coordinates updated in EEPROM\r\n");
}

/**
 *      @brief Function to calculate average readings and distance of the source of signal from each sensor
 *      @param g_timer_A_FIFO Array holding the value of timer values
 *      @param g_timer_B_FIFO Array holding the value of timer values
 *      @param g_timer_C_FIFO Array holding the value of timer values
 *      @param print boolean value that determines if distance must be printed or not
 **/
void calculate_distance(uint32_t *g_timer_A_FIFO, uint32_t *g_timer_B_FIFO, uint32_t *g_timer_C_FIFO, bool print)
{
    char string[100];

    uint32_t value_count = readEeprom(TC_AVG);
    ASSERT(value_count);                                                    // Failsafe to avoid divide by zero error

    uint8_t i;

    g_average_A = g_average_B = g_average_C = 0;

    for (i = 0; i < value_count; i++)
    {
        g_average_A = g_average_A + g_timer_A_FIFO[i];
        g_average_B = g_average_B + g_timer_B_FIFO[i];
        g_average_C = g_average_C + g_timer_C_FIFO[i];
    }

    g_average_A = g_average_A / (double)value_count;                        // Find average
    g_average_B = g_average_B / (double)value_count;                        // Find average
    g_average_C = g_average_C / (double)value_count;                        // Find average

    g_distance_A = (g_average_A * CONVERSION_CONSTANT);
    g_distance_B = (g_average_B * CONVERSION_CONSTANT);
    g_distance_C = (g_average_C * CONVERSION_CONSTANT);

    if (print)
    {
        sprintf(string, "Distance from Sensor A: %dmm\r\n", g_distance_A);      // Convert to string
        putsUart0(string);                                                      // Print

        sprintf(string, "Distance from Sensor B: %dmm\r\n", g_distance_B);      // Convert to string
        putsUart0(string);                                                      // Print

        sprintf(string, "Distance from Sensor C: %dmm\r\n\r\n", g_distance_C);  // Convert to string
        putsUart0(string);                                                      // Print
    }
}

/**
*      @brief Function to write Beep parameters into EEPROM
*      @param beep_type enum type of beep
*      @param load load value for PWM
*      @param per1 on time of PWM
*      @param per2 off time of PWM
*      @param count number of beeps to play in tone
**/
void write_beep(beep_t beep_type, uint32_t load, uint32_t per1)
{
    switch (beep_type)
    {
        case BEEP_IR_INT:
        {
            writeEeprom(LOAD_IR, (load * 10000));
            writeEeprom(PER1_IR, (per1 * 100000));
            writeEeprom(PER2_IR, 10000);
            writeEeprom(CONT_IR, 2);

            break;
        }

        case BEEP_US_A_INT:
        {
            writeEeprom(LOAD_A, (load * 10000));
            writeEeprom(PER1_A, (per1 * 100000));
            writeEeprom(PER2_A, 50000);
            writeEeprom(CONT_A, 3);

            break;
        }

        case BEEP_US_B_INT:
        {
            writeEeprom(LOAD_B, (load * 10000));
            writeEeprom(PER1_B, (per1 * 100000));
            writeEeprom(PER2_B, 50000);
            writeEeprom(CONT_B, 3);

            break;
        }

        case BEEP_US_C_INT:
        {
            writeEeprom(LOAD_C, (load * 10000));
            writeEeprom(PER1_C, (per1 * 100000));
            writeEeprom(PER2_C, 50000);
            writeEeprom(CONT_C, 3);

            break;
        }

        case BEEP_ERROR:
        {
            writeEeprom(LOAD_ERR, (load * 10000));
            writeEeprom(PER1_ERR, (per1 * 100000));
            writeEeprom(PER2_ERR, 100000);
            writeEeprom(CONT_ERR, 4);

            break;
        }
    }
}

/**
*      @brief Function to calculate the variance of a reading
*               Variance is calculated as follows
*                   (((mean - individual_reading) ^ 2) / number_of_readings)
*      @param g_timer_A_FIFO Array holding the value of timer values
*      @param g_timer_B_FIFO Array holding the value of timer values
*      @param g_timer_C_FIFO Array holding the value of timer values
**/
void calculate_variance(uint32_t *g_timer_A_FIFO, uint32_t *g_timer_B_FIFO, uint32_t *g_timer_C_FIFO)
{
    if (g_distance_A == 0 || g_distance_C == 0 || g_distance_C == 0)
    {
        calculate_distance(g_timer_A_FIFO, g_timer_B_FIFO, g_timer_C_FIFO, false);  // Get Average if not already available
    }

    uint32_t value_count = readEeprom(TC_AVG);                                      // Read value from EEPROM
    ASSERT(value_count);                                                            // Failsafe to avoid divide by zero error

    double variance_A = 0, variance_B = 0, variance_C = 0, numerator_A = 0, numerator_B = 0, numerator_C = 0;
    uint8_t i;
    char string[100];
    double bobA, bobB, bobC;

    for (i = 0; i < value_count; i++)
    {
        bobA = ((g_timer_A_FIFO[i] * CONVERSION_CONSTANT) - g_distance_A);
        numerator_A = (numerator_A + (bobA * bobA));

        bobB = ((g_timer_B_FIFO[i] * CONVERSION_CONSTANT) - g_distance_B);
        numerator_B = (numerator_B + (bobB * bobB));

        bobC = ((g_timer_C_FIFO[i] * CONVERSION_CONSTANT) - g_distance_C);
        numerator_C = (numerator_C + (bobC * bobC));
    }

    variance_A = numerator_A / value_count;
    variance_B = numerator_B / value_count;
    variance_C = numerator_C / value_count;

    if (variance_A <= 10 && variance_B <= 10 && variance_C <= 10)                   // Ensure variance conforms to acceptable range
    {
        g_values_acceptable = true;
    }

    sprintf(string, "Variance of Sensor A readings = %f\r\n", variance_A);          // Convert to string
    putsUart0(string);                                                              // Print

    sprintf(string, "Variance of Sensor B readings = %f\r\n", variance_B);          // Convert to string
    putsUart0(string);                                                              // Print

    sprintf(string, "Variance of Sensor C readings = %f\r\n\r\n", variance_C);      // Convert to string
    putsUart0(string);                                                              // Print
}

/**
*      @brief Function to calculate x, y coordinates
**/
void calculate_coordinates(void)
{
    char string[100];
    char stringx[50];
    char stringy[50];
    if (g_values_acceptable)
    {
        double x = 0, y = 0;

        int32_t D1 = (readEeprom(CRD_BY) - readEeprom(CRD_AY));
        int32_t D2 = (readEeprom(CRD_CX) - readEeprom(CRD_BX));

        if (D1 <= 0 || D1 >= 200)    D1 = 200;
        if (D2 <= 0 || D2 >= 300)    D2 = 300;

        y = (((D1 * D1) + (g_distance_B * g_distance_B) - (g_distance_A * g_distance_A)) / (2 * D1));
        x = (((D2 * D2) + (g_distance_B * g_distance_B) - (g_distance_C * g_distance_C)) / (2 * D2));

        x = x - (double)readEeprom(FIX_X);
        y = y - (double)readEeprom(FIX_Y);

        ftoa(x, stringx, 0);                                                        // Convert floating number to string
        ftoa(y, stringy, 0);                                                        // Convert floating number to string
        sprintf(string, "x,y: %0.0fmm, %0.0fmm\r\n\r\n", x, y);

        putsLcd(0, 0, stringx);                                                     // Display on LCD screen
        putsLcd(1, 0, stringy);                                                     // Display on LCD screen
        putsUart0(string);                                                          // Display on Terminal
    }
    else
    {
        putsUart0("Variance out of bounds\r\n\r\n");
    }
}

/**
*      @brief Function to update drift from true x, y values in the EEPROM
*      @param x_fix drift from true coordinate
*      @param y_fix drift from true coordinate
**/
void update_fix(int32_t x_fix, int32_t y_fix)
{
    writeEeprom(FIX_X, x_fix);
    writeEeprom(FIX_Y, y_fix);

    waitMicrosecond(1000000);
}
