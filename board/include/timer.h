/**
*      @file timer.c
*      @author Prithvi Bhat
*      @brief Timer Utility for TM4C123GH6PM
**/

#ifndef TIMER_H
#define TIMER_H

#include <inttypes.h>

// Enumeration of all timers used in the project
typedef enum
{
    WTIMER_A = 0,
    WTIMER_B = 1,
    WTIMER_C = 2,
    WTIMER_W = 3,
} timers_t;

// Function Declarations
void timer_init(void);
void timer_start(void);
uint32_t timer_stop(timers_t timer);
void pwm_init(void);

#endif
