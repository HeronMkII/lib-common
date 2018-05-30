/*
Authors: Shimi Smith, Matthew Silverman
Taken from Shimi's initial timer.h

All values are similar, except this runs an 8-bit timer. To compensate,
I use a 16-bit int to store the number of interrupts required. Timer2 cannot
handle more than 35 minutes (which is the same as Timer1).
*/

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef F_CPU
#define F_CPU 8000000
#endif

#define PRESCALER2 1024.0
#define T2 ((PRESCALER2 / F_CPU) * 1000.0)  // timer clock period (ms) = 0.128
#define MAX_TIME2 (T2 * 0xFF)  // the maximum time the 8 bit timer can hold until it overflows (ms)
//32.64

#define ROUND2 0.5  // used to round double to int

typedef void (*Command)(void);

//functions
void set_vars2(uint8_t minutes, Command cmd);
void init_timer2(uint8_t minutes, Command cmd);

// This struct holds important variables for the timer
typedef struct Timer2{
	uint16_t ints;  // the number of interrupts that will occur to achieve the desired time
	uint16_t count;  // the value of the timer counter after the desired time has ellapsed
	Command cmd;  // The command to run once the desired time has passed
}Timer2;
