/**
*      @file commands.c
*      @author Prithvi Bhat
*      @brief File that handles functions relating terminal commands
*      @date 2022-12-04
**/

#ifndef COMMANDS_H
#define COMMANDS_H

#include "inttypes.h"
#include "gpio.h"

#define LED_B           PORTF,2
#define LED_R           PORTF,1
#define LED_G           PORTF,3

#define MAX_AVERAGES    10
#define MAX_FIFO_SIZE   20

/**
*      @brief
       |---------------------------------|
       | R | G | B | Indication          |
       |---|---|---|---------------------|
       | 0 | 0 | 0 | CLEAR               |
       | 0 | 0 | 1 | IR interrupt        |
       | 0 | 1 | 1 | Sensor A interrupt  |
       | 1 | 0 | 1 | Sensor B interrupt  |
       | 1 | 1 | 0 | Sensor C interrupt  |
       | 1 | 0 | 0 | Timer timeout error |
       |---------------------------------|
*
**/
#define LED_CLEAR               \
    ({                          \
        setPinValue(LED_R, 0);  \
        setPinValue(LED_G, 0);  \
        setPinValue(LED_B, 0);  \
    })

#define LED_SENSOR_A            \
    ({                          \
        setPinValue(LED_R, 0);  \
        setPinValue(LED_G, 1);  \
        setPinValue(LED_B, 1);  \
    })

#define LED_SENSOR_B            \
    ({                          \
        setPinValue(LED_R, 1);  \
        setPinValue(LED_G, 0);  \
        setPinValue(LED_B, 1);  \
    })

#define LED_SENSOR_C            \
    ({                          \
        setPinValue(LED_R, 1);  \
        setPinValue(LED_G, 1);  \
        setPinValue(LED_B, 0);  \
    })

#define LED_TIMEOUT             \
    ({                          \
        setPinValue(LED_R, 1);  \
        setPinValue(LED_G, 0);  \
        setPinValue(LED_B, 0);  \
    })

#define LED_IR_SENSOR           \
    ({                          \
        setPinValue(LED_R, 0);  \
        setPinValue(LED_G, 0);  \
        setPinValue(LED_B, 1);  \
    })

/**
*      @brief Enumeration of Buzzer tone types
**/
typedef enum
{
    BEEP_IR_INT = 0,
    BEEP_US_A_INT,
    BEEP_US_B_INT,
    BEEP_US_C_INT,
    BEEP_ERROR,
    BEEP_START,
} beep_t;

void update_sensor_coordinates(char *sensor, uint32_t x, uint32_t y);
void calculate_distance(uint32_t *g_timer_A_FIFO, uint32_t *g_timer_B_FIFO, uint32_t *g_timer_C_FIFO, bool print);
void calculate_variance(uint32_t *g_timer_A_FIFO, uint32_t *g_timer_B_FIFO, uint32_t *g_timer_C_FIFO);
void write_beep(beep_t beep_type, uint32_t load, uint32_t per1);
void beep_now(beep_t beep_type);
void calculate_coordinates(void);
void update_fix(int32_t x_fix, int32_t y_fix);

#endif
