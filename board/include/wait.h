// Wait functions
// Jason Losh

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

#ifndef WAIT_H_
#define WAIT_H_

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

void waitMicrosecond(uint32_t us);
void _delay_cycles(uint32_t cycles);

#endif
