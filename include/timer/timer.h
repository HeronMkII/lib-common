#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include <utilities/utilities.h>


//prescaler factor - divide F_CPU (main clock, 8MHz) by 1024
#define PRESCALER 1024.0
// timer clock period (ms)
#define PERIOD ((PRESCALER / F_CPU) * 1000.0)
//counter maximum (16-bit) p.127
#define MAX_TIME_16BIT (PERIOD * 0xFFFF)
//counter maximum (8 bit) p.127
#define MAX_TIME_8BIT (PERIOD * 0xFF)
// used to round double to int
#define ROUND 0.5

typedef void(*timer_fn_t)(void);

// This struct holds important variables for the 8-bit (and 16-bit) timer
typedef struct {
    // the number of interrupts (counting down from the maximum time)
    // that will occur to achieve the desired time, not including remainder time
    uint16_t max_time_ints;
    // remamining timer counter value after the desired time has ellapsed
    uint16_t remainder_time;
    // The command to run once the desired time has passed
    timer_fn_t cmd;
    // Counts the number of interrupts that have occured for the timer
    volatile uint16_t int_count;
} timer_t;

void start_timer_16bit(uint16_t seconds, timer_fn_t cmd);
void start_timer_8bit(uint16_t seconds, timer_fn_t cmd);

void stop_timer_16bit(void);
void stop_timer_8bit(void);

#endif
