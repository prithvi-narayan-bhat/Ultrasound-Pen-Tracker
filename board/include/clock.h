/**
 *       @brief Clock Library
 *           Target Platform:   EK-TM4C123GXL
 *           Target uC:         TM4C123GH6PM
 *           Hardware:          16MHz XOSC
 *       @author Prithvi Bhat (with references from Jason Losh's Library)
 **/

#ifndef CLOCK_H_
#define CLOCK_H_

typedef enum
{
    SYS_CLK_40MHZ = 0,
} sysClk_t;

void initSystemClock(sysClk_t sysClk);

#endif
