/*
Authors: Shimi Smith, Matthew Silverman

Runs an 8-bit and 16-bit timer.

For 8-bit timer, to compensate for the differences in 16-bit timer,
I use a 16-bit int to store the number of interrupts required. timer_16bit cannot
handle more than 35 minutes (which is the same as timer_8bit).
*/

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef F_CPU
#define F_CPU 8000000
#endif

#define PRESCALER 1024.0
#define T ((PRESCALER / F_CPU) * 1000.0)
// timer clock period (ms)
#define MAX_TIME_16BIT (T * 0xFFFF)
// the maximum time the 16 bit timer can hold until it overflows (ms)
#define MAX_TIME_8BIT (T * 0xFF)
// the maximum time the 8 bit timer can hold until it overflows (ms)

#define ROUND 0.5  // used to round double to int

typedef void(*cmd_fn_t)(void);

void init_timer_16bit(uint8_t minutes, cmd_fn_t cmd);
void init_timer_8bit(uint8_t minutes, cmd_fn_t cmd);

// This struct holds important variables for the 8-bit timer
typedef struct {
    uint16_t ints;
    // the number of interrupts that will occur to achieve the desired time
	uint16_t count;
    // the value of the timer counter after the desired time has ellapsed
	cmd_fn_t cmd;
    // The command to run once the desired time has passed
} timer_t;

