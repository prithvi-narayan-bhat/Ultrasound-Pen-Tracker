/**
 *      @file main.c
 *      @author Prithvi Bhat
 *      @brief Main Driver code for the Ultrasound Pen
 *               All peripherals and I/O are handled by GPIOs on Ports A and D as tabulated below
 *                   |--------------|--------------|-------|
 *                   | Peripheral   | Direction    | Pin   |
 *                   |--------------|--------------|-------|
 *                   | Buzzer       | Output       | PD01  |
 *                   | IR Sensor    | Input        | PD06  |
 *                   | Audio Ch 1   | Input        | PC04  |
 *                   | Audio Ch 2   | Input        | PC05  |
 *                   | Audio Ch 3   | Input        | PC06  |
 *                   |--------------|--------------|-------|
 **/

#include "clock.h"
#include "gpio.h"
#include "tm4c123gh6pm.h"
#include "eeprom.h"
#include "timer.h"
#include "nvic.h"
#include "uart0.h"
#include "wait.h"
#include "strings.h"
#include <stdio.h>
#include "eeprom_memory_map.h"
#include "commands.h"
#include <string.h>
#include "i2c0_lcd.h"

#define IS_COMMAND(string, count)       if(isCommand(&user_data, string, count))
#define RESET                           (NVIC_APINT_R = (NVIC_APINT_VECTKEY | NVIC_APINT_SYSRESETREQ))
#define ASSERT(value)                   if(value >= 0)

// Global Variables
uint32_t g_timer_A_accumulated = 0, g_timer_B_accumulated = 0, g_timer_C_accumulated = 0, count = 0;
uint32_t g_timer_A_FIFO[MAX_FIFO_SIZE], g_timer_B_FIFO[MAX_FIFO_SIZE], g_timer_C_FIFO[MAX_FIFO_SIZE];
bool ir_in, sA_in, sB_in, sC_in;

// Pin Macros
#define IR_IN       		PORTA,6		            // Input pin for IR signal
#define US_A_IN		        PORTC,4		            // Input pin for comparator output from Sensor 1
#define US_B_IN 		    PORTC,5		            // Input pin for comparator output from Sensor 2
#define US_C_IN 		    PORTC,6		            // Input pin for comparator output from Sensor 3
#define BUZZ_OUT            PORTD,1                 // Output pin for buzzer

/**
 *      @brief Function to initialize all necessary hardware on the device
 **/
void init_TM4C_hardware(void)
{
    initSystemClockTo40Mhz(); 		                // Initialize system clock

    enablePort(PORTC);                              // Initialize clocks on PORT C
    enablePort(PORTD); 				                // Initialize clocks on PORT D
    enablePort(PORTF);                              // Initialize clocks on PORT F

    selectPinPushPullOutput(LED_R);
    selectPinPushPullOutput(LED_B);
    selectPinPushPullOutput(LED_G);
    selectPinPushPullOutput(BUZZ_OUT);

    setPinAuxFunction(BUZZ_OUT, GPIO_PCTL_PD1_M1PWM1);

    initLcd();                                      // Initialise I2C display device
    initEeprom(); 					                // Initialize MCU to use EEPROM
    pwm_init();                                     // Initialise PWM

    initUart0();                                    // Initialise UART0
    setUart0BaudRate(115200, 40e6);                 // Set UART baud rate and clock

    disableNvicInterrupt(INT_GPIOD);				// Disable the interrupt using its vector
    disableNvicInterrupt(INT_GPIOA);                // Disable the interrupt using its vector

    disablePinInterrupt(IR_IN);					    // Disable Interrupt to configure

    selectPinDigitalInput(IR_IN);				    // Set Pin to input
    selectPinInterruptFallingEdge(IR_IN);		    // Initialize interrupt to trigger on rising edge
    enablePinPullup(IR_IN);

    clearPinInterrupt(IR_IN);					    // Clear any older, stray interrupts
    enablePinInterrupt(IR_IN);					    // Initialize Interrupt

    enableNvicInterrupt(INT_GPIOD);					// Enable interrupt after all configurations are complete
    enableNvicInterrupt(INT_GPIOA);                 // Enable interrupt after all configurations are complete

    timer_init();                                   // Initialise timers
}

/**
 *      @brief ISR for when the MCU receives input from the comparator for Ultrasound Sensor A
 **/
void sA_interrupt_handler(void)
{
    g_timer_A_FIFO[g_timer_A_accumulated++] = WTIMER0_TAV_R;    // Read timer register
    WTIMER0_CTL_R &= ~TIMER_CTL_TAEN;                           // Disable timer
    WTIMER0_TAV_R = 0;                                          // Reset Register
    WTIMER0_ICR_R |= TIMER_ICR_CAECINT;                         // Reset Timer interrupt
    sA_in = true;                                               // Set flag for feedback
}

/**
 *      @brief ISR for when the MCU receives input from the comparator for Ultrasound Sensor B
 **/
void sB_interrupt_handler(void)
{
    g_timer_B_FIFO[g_timer_B_accumulated++] = WTIMER0_TBV_R;    // Read timer register
    WTIMER0_CTL_R &= ~TIMER_CTL_TBEN;                           // Disable timer
    WTIMER0_TBV_R = 0;                                          // Reset Register
    WTIMER0_ICR_R |= TIMER_ICR_CBECINT;                         // Reset Timer interrupt
    sB_in = true;                                               // Set flag for feedback
}

/**
 *      @brief ISR for when the MCU receives input from the comparator for Ultrasound Sensor C
 **/
void sC_interrupt_handler(void)
{
    g_timer_C_FIFO[g_timer_C_accumulated++] = WTIMER1_TAV_R;    // Read timer register
    WTIMER1_CTL_R &= ~TIMER_CTL_TAEN;                           // Disable timer
    WTIMER1_TAV_R = 0;                                          // Reset Register
    WTIMER1_ICR_R |= TIMER_ICR_CAECINT;                         // Reset Timer interrupt
    sC_in = true;                                               // Set flag for feedback
}

/**
 *      @brief Watchdog timer ISR
 **/
void timeout_interrupt_handler(void)
{
    WTIMER3_CTL_R &= ~TIMER_CTL_TAEN;                           // Disable timer
    WTIMER3_ICR_R |= TIMER_ICR_TATOCINT;                        // Reset Timer interrupt

    WTIMER0_CTL_R &= ~TIMER_CTL_TAEN;                           // Disable timer
    WTIMER0_ICR_R |= TIMER_ICR_CAECINT;                         // Reset Timer interrupt
    WTIMER0_TAV_R = 0;

    WTIMER0_CTL_R &= ~TIMER_CTL_TBEN;                           // Disable timer
    WTIMER0_ICR_R |= TIMER_ICR_CBECINT;                         // Reset Timer interrupt
    WTIMER0_TBV_R = 0;

    WTIMER1_CTL_R &= ~TIMER_CTL_TAEN;                           // Disable timer
    WTIMER1_ICR_R |= TIMER_ICR_CAECINT;                         // Reset Timer interrupt
    WTIMER1_TAV_R = 0;

    timer_init();                                               // Re-initialise timer as failsafe

    if ((ir_in && !(sA_in && sB_in && sC_in)))
    {
        LED_TIMEOUT;
        beep_now(BEEP_ERROR);
    }

    if (ir_in)
    {
        LED_IR_SENSOR;
        beep_now(BEEP_IR_INT);
        ir_in = false;
    }

    if (sA_in)
    {
        LED_SENSOR_A;
        beep_now(BEEP_US_A_INT);
        sA_in = false;
    }

    if (sB_in)
    {
        LED_SENSOR_B;
        beep_now(BEEP_US_B_INT);
        sB_in = false;
    }

    if (sC_in)
    {
        LED_SENSOR_C;
        beep_now(BEEP_US_C_INT);
        sC_in = false;
    }

    enableNvicInterrupt(INT_GPIOD);                             // Enable interrupts on PORTD to capture IR

    WTIMER3_CTL_R &= ~TIMER_CTL_TAEN;                           // Disable timer
    WTIMER3_ICR_R |= TIMER_ICR_TATOCINT;                        // Reset Timer interrupt
}

/**
*      @brief ISR for when MCU receives signal from the IR receiver
*       * Starts three timers simultaneously corresponding to sensors
*       * Starts one watchdog timers
**/
void ir_interrupt_handler(void)
{
    LED_CLEAR;
    clearPinInterrupt(IR_IN);                   // Clear  interrupt to be able to exit ISR and capture next interrupt

    timer_init();                               // Re-initialise timer as failsafe

    // Accept only a max of five values in FIFO to average
    // Reset count and flush FIFO
    if (g_timer_A_accumulated >= count||
        g_timer_B_accumulated >= count||
        g_timer_C_accumulated >= count
    )
    {
        g_timer_A_accumulated = 0;
        g_timer_B_accumulated = 0;
        g_timer_C_accumulated = 0;
        memset(g_timer_A_accumulated, 0, sizeof(g_timer_A_accumulated));
        memset(g_timer_B_accumulated, 0, sizeof(g_timer_B_accumulated));
        memset(g_timer_C_accumulated, 0, sizeof(g_timer_C_accumulated));
    }

    WTIMER0_CTL_R &= ~TIMER_CTL_TAEN;           // Stop timer 0 - Sensor A
    WTIMER0_CTL_R &= ~TIMER_CTL_TBEN;           // Stop timer 0 - Sensor B
    WTIMER1_CTL_R &= ~TIMER_CTL_TAEN;           // Stop timer 1 - Sensor C
    WTIMER3_CTL_R &= ~TIMER_CTL_TAEN;           // Stop timer 3 - Watchdog

    WTIMER0_TAV_R = 0;                          // Reset timer to 0 before starting
    WTIMER0_TBV_R = 0;                          // Reset timer to 0 before starting
    WTIMER1_TAV_R = 0;                          // Reset timer to 0 before starting

    WTIMER0_CTL_R |= TIMER_CTL_TAEN;            // Start timer 0 - Sensor A
    WTIMER0_CTL_R |= TIMER_CTL_TBEN;            // Start timer 0 - Sensor B
    WTIMER1_CTL_R |= TIMER_CTL_TAEN;            // Start timer 1 - Sensor C
    WTIMER3_CTL_R |= TIMER_CTL_TAEN;            // Start timer 3 - Watchdog
    ir_in = true;                               // Set flag for feedback
}

/**
 *      @brief Main, driver function
 **/
void main(void)
{
    init_TM4C_hardware();
    char string[100];

    string_data_t *user_data;
    uint16_t character_count;
    uint8_t i;

    count = readEeprom(TC_AVG);

    if (count <= 0)     count = 1;              // Capture only one value if nothing specified by user

    if (readEeprom(0x00) == 0xFF)               // Coordinates have not previously been written into EEPROM
    {
        putsUart0("Sensor coordinates missing!\r\n\r\n");
    }

    while (1)
    {
        string_input_get(&user_data);           // Read user input
        string_parse(&user_data);               // Parse user input

        IS_COMMAND("sensor", 4)                 // Compare and act on user input
        {
            // Store sensor coordinates in EEPROM
            update_sensor_coordinates (
                                        getFieldString(&user_data, 1),
                                        (int32_t)getFieldInteger(&user_data, 2),
                                        (int32_t)getFieldInteger(&user_data, 3)
                                    );
            putsUart0("Assuming input coordinates are in mm\r\n\r\n");
            continue;
        }

        IS_COMMAND("reset", 1)
        {
            for (i = 0; i < MAX_FIFO_SIZE; i++)
            {
                g_timer_A_FIFO[i] = 0;
                g_timer_B_FIFO[i] = 0;
                g_timer_C_FIFO[i] = 0;
            }
            g_timer_A_accumulated = 0;          // Reset All values
            g_timer_B_accumulated = 0;          // Reset All values
            g_timer_C_accumulated = 0;          // Reset All values

            RESET;                              // Reset System
            continue;
        }

        IS_COMMAND("distance", 1)
        {
            calculate_distance(g_timer_A_FIFO, g_timer_B_FIFO, g_timer_C_FIFO, true); // Output distance
            continue;
        }

        IS_COMMAND("average", 2)
        {
            uint32_t average = (uint32_t)getFieldInteger(&user_data, 1);

            if (average > MAX_AVERAGES)
            {
                fprintf(string, "ERROR! Max average of %d\r\n\r\n", MAX_AVERAGES);
                putsUart0(string);
            }
            else
            {
                writeEeprom(TC_AVG, (uint32_t)average); // Write the number of averages into eeprom
                putsUart0("Averager updated\r\n\r\n");
                count = average;
            }

            continue;
        }

        IS_COMMAND("beep", 4)                           // Update beep tones
        {
            int32_t type = (int32_t)getFieldInteger(&user_data, 1);
            int32_t load = (int32_t)getFieldInteger(&user_data, 2);
            int32_t per1 = (int32_t)getFieldInteger(&user_data, 3);

            write_beep(type, load, per1);
            // write_beep(0, 1, 1);
            // write_beep(1, 2, 2);
            // write_beep(2, 3, 2);
            // write_beep(3, 4, 2);
            // write_beep(4, 5, 3);

            putsUart0("Beep tones updated\r\n\r\n");
        }

        IS_COMMAND("variance", 1)
        {
            calculate_variance(g_timer_A_FIFO, g_timer_B_FIFO, g_timer_C_FIFO); // Output Variance
        }

        IS_COMMAND("coord", 1)
        {
            calculate_coordinates();
        }

        IS_COMMAND("fix", 3)
        {
            int32_t x_fix = (int32_t)getFieldInteger(&user_data, 1);
            int32_t y_fix = (int32_t)getFieldInteger(&user_data, 2);

            update_fix(x_fix, y_fix);

            putsUart0("Fix values updated\r\n");
        }
    }
}
