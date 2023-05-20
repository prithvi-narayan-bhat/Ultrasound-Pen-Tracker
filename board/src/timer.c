/**
*      @file timer.c
*      @author Prithvi Bhat
*      @brief Timer Utility for TM4C123GH6PM
**/

#include "timer.h"
#include "tm4c123gh6pm.h"
#include <inttypes.h>
#include "gpio.h"
#include "nvic.h"
#include "wait.h"

#define US_A_IN                 PORTC, 4
#define US_B_IN                 PORTC, 5
#define US_C_IN                 PORTC, 6

#define TIMER_START_VALUE       0
#define TIMER_MAX_VALUE         40000000
#define TIMER_VALUE_READ_MASK   0x0000FFFF

/**
 *      @brief Initialize timer registers
 **/
void timer_init()
{
    SYSCTL_RCGCWTIMER_R |= SYSCTL_RCGCWTIMER_R0;    // Enable and provide clock to the timer
    _delay_cycles(3);                               // Delay for sync

    // Timer 0A for Sensor A
    selectPinAnalogInput(US_A_IN);
    enablePinPullup(US_A_IN);
    setPinAuxFunction(US_A_IN, GPIO_PCTL_PC4_WT0CCP0);
    GPIO_PORTC_DEN_R |= 16;

    WTIMER0_CTL_R       &= ~TIMER_CTL_TAEN;         // Disable timer before configuring
    WTIMER0_CFG_R       = TIMER_CFG_16_BIT;         // Select 32 bit wide counter
    WTIMER0_TAMR_R      |= TIMER_TAMR_TACMR;        // Configure as edge timer
    WTIMER0_TAMR_R      |= TIMER_TAMR_TAMR_CAP;     // Configure for capture mode
    WTIMER0_TAMR_R      |= TIMER_TAMR_TACDIR;       // Direction = Up-counter
    WTIMER0_CTL_R       |= TIMER_CTL_TAEVENT_NEG;   // Configure to capture from negative edge
    WTIMER0_IMR_R       |= TIMER_IMR_CAEIM;         // Configure to trigger interrupts trigger
    WTIMER0_TAV_R       = 0;

    enableNvicInterrupt(INT_WTIMER0A);              // Enable timer interrupt
    _delay_cycles(3);                               // Delay for sync

    // Timer 0B for Sensor B
    selectPinAnalogInput(US_B_IN);
    enablePinPullup(US_B_IN);
    setPinAuxFunction(US_B_IN, GPIO_PCTL_PC5_WT0CCP1);
    GPIO_PORTC_DEN_R |= 32;

    WTIMER0_CTL_R       &= ~TIMER_CTL_TBEN;         // Disable timer before configuring
    WTIMER0_CFG_R       = TIMER_CFG_16_BIT;         // Select 16 bit wide counter
    WTIMER0_TBMR_R      |= TIMER_TBMR_TBCMR;        // Configure as edge timer
    WTIMER0_TBMR_R      |= TIMER_TBMR_TBMR_CAP;     // Configure for capture mode
    WTIMER0_TBMR_R      |= TIMER_TBMR_TBCDIR;       // Direction = Up-counter
    WTIMER0_CTL_R       |= TIMER_CTL_TBEVENT_NEG;   // Configure to capture from negative edge
    WTIMER0_IMR_R       |= TIMER_IMR_CBEIM;         // Configure to trigger interrupts trigger
    WTIMER0_TBV_R       = 0;

    enableNvicInterrupt(INT_WTIMER0B);              // Enable timer interrupt
    _delay_cycles(3);                               // Delay for sync

    // Timer 1A for Sensor C
    SYSCTL_RCGCWTIMER_R |= SYSCTL_RCGCWTIMER_R1;    // Enable and provide clock to the timer
    _delay_cycles(3);                               // Delay for sync

    selectPinAnalogInput(US_C_IN);
    enablePinPullup(US_C_IN);
    setPinAuxFunction(US_C_IN, GPIO_PCTL_PC6_WT1CCP0);
    GPIO_PORTC_DEN_R |= 64;

    WTIMER1_CTL_R       &= ~TIMER_CTL_TAEN;         // Disable timer before configuring
    WTIMER1_CFG_R       = TIMER_CFG_16_BIT;         // Select 16 bit wide counter
    WTIMER1_TAMR_R      |= TIMER_TAMR_TACMR;        // Configure as edge timer
    WTIMER1_TAMR_R      |= TIMER_TAMR_TAMR_CAP;     // Configure for capture mode
    WTIMER1_TAMR_R      |= TIMER_TAMR_TACDIR;       // Direction = Up-counter
    WTIMER1_CTL_R       |= TIMER_CTL_TAEVENT_NEG;   // Configure to capture from negative edge
    WTIMER1_IMR_R       |= TIMER_IMR_CAEIM;         // Configure to trigger interrupts trigger
    WTIMER1_TAV_R       = 0;

    enableNvicInterrupt(INT_WTIMER1A);              // Enable timer interrupt
    _delay_cycles(3);                               // Delay for sync

    // Timer 3 for Watchdog
    SYSCTL_RCGCWTIMER_R |= SYSCTL_RCGCWTIMER_R3;    // Enable and provide clock to the timer
    _delay_cycles(3);                               // Delay for sync

    WTIMER3_CTL_R       &= ~TIMER_CTL_TAEN;         // Disable timer before configuring
    WTIMER3_CFG_R       = TIMER_CFG_32_BIT_TIMER;   // Select One shot counter mode
    WTIMER3_TAMR_R      |= TIMER_TAMR_TAMR_1_SHOT;  // Configure as one shot mode
    WTIMER3_TAILR_R     = 90000;
    WTIMER3_IMR_R       |= TIMER_IMR_TATOIM;        // Configure to trigger interrupts on match

    enableNvicInterrupt(INT_WTIMER3A);              // Enable timer interrupt

    _delay_cycles(3);                               // Delay for sync
}

/**
*      @brief Function to start all three timers
**/
void timer_start(void)
{
    WTIMER0_TAV_R = TIMER_START_VALUE;                      // Reset timer to 0 before starting
    WTIMER0_TBV_R = TIMER_START_VALUE;                      // Reset timer to 0 before starting
    WTIMER1_TAV_R = TIMER_START_VALUE;                      // Reset timer to 0 before starting

    WTIMER0_CTL_R |= TIMER_CTL_TAEN;                        // Start timer 0 - Sensor A
    WTIMER0_CTL_R |= TIMER_CTL_TBEN;                        // Start timer 0 - Sensor B
    WTIMER1_CTL_R |= TIMER_CTL_TAEN;                        // Start timer 1 - Sensor C
    WTIMER3_CTL_R |= TIMER_CTL_TAEN;                        // Start timer 3 - Watchdog
}

/**
*      @brief Function to stop timer, clear its interrupt and read the timer value at the instance of stopping
*      @param timer to stop and read
*      @return uint32_t timer register value
**/
uint32_t timer_stop(timers_t timer)
{
    uint32_t timer_val;
    switch (timer)
    {
        case WTIMER_A:
        {
            timer_val = WTIMER0_TAV_R;                      // Read timer register
            WTIMER0_ICR_R |= TIMER_ICR_CAECINT;             // Reset Timer interrupt
            WTIMER0_CTL_R &= ~(TIMER_CTL_TAEN);             // Disable timer
            return(timer_val);
        }

        case WTIMER_B:
        {
            timer_val = WTIMER0_TBV_R;                      // Read timer register
            WTIMER0_ICR_R |= TIMER_ICR_CBECINT;             // Reset Timer interrupt
            WTIMER0_CTL_R   &= ~(TIMER_CTL_TBEN);           // Disable timer
            return(timer_val);
        }

        case WTIMER_C:
        {
            timer_val = WTIMER1_TAV_R;                      // Read timer register
            WTIMER1_ICR_R |= TIMER_ICR_CAECINT;             // Reset Timer interrupt
            WTIMER1_CTL_R   &= ~(TIMER_CTL_TAEN);           // Disable timer
            return(timer_val);
        }

        case WTIMER_W:
        {
            WTIMER3_ICR_R |= (TIMER_ICR_TAMCINT | TIMER_ICR_TATOCINT);  // Reset Timer interrupt
            WTIMER3_CTL_R &= ~(TIMER_CTL_TAEN);                         // Disable timer
            return(WTIMER1_TAV_R);                                      // Read timer register
        }
    }
    return (timer_val);
}

/**
*      @brief Function to initialise PWM for buzzer M1-PWM3 (Generator 1 B)
**/
void pwm_init(void)
{
    SYSCTL_RCGCPWM_R    |= SYSCTL_RCGCPWM_R1;
    SYSCTL_RCGCGPIO_R   |= SYSCTL_RCGCGPIO_R3;

    _delay_cycles(3);

    SYSCTL_SRPWM_R      = 0;

    GPIO_PORTD_PCTL_R   &= GPIO_PCTL_PD1_M;
    GPIO_PORTD_PCTL_R   |= GPIO_PCTL_PD1_M1PWM1;

    SYSCTL_SRPWM_R      = SYSCTL_SRPWM_R1;
    SYSCTL_SRPWM_R      = 0;

    PWM1_0_CTL_R        = ~(PWM_0_CTL_ENABLE);
    PWM1_0_GENB_R       = PWM_1_GENB_ACTCMPBD_ZERO | PWM_1_GENB_ACTLOAD_ONE;
    PWM1_0_LOAD_R       = 10000;
    PWM1_0_CMPB_R       = PWM1_0_LOAD_R / 2;
    PWM1_0_CTL_R        = PWM_0_CTL_ENABLE;
    PWM1_ENABLE_R       = PWM_ENABLE_PWM1EN;
    PWM1_0_LOAD_R       = 0;
}
