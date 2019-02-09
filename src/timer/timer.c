/*
Timer
Authors: Shimi Smith, Matthew Silverman, Ambrose Man

This contains two timers that each run a given function repeatedly at some
interval. Runs an 8-bit (Timer 0) and 16-bit (Timer 1) timer.

Datasheet: https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8209-8-bit%20AVR%20ATmega16M1-32M1-64M1_Datasheet.pdf

For 8-bit timer, to compensate for the differences in 16-bit timer,
I use a 16-bit int to store the number of interrupts required. timer_16bit cannot
handle more than 35 minutes (which is the same as timer_8bit).

Here's a good website to get an idea of timers in microcontrollers:
https://www.newbiehack.com/TimersandCountersDefaultandBasicUsage.aspx
*/

#include <timer/timer.h>

// Default no-operation callback function
void timer_fn_nop(void) {}

timer_t timer16 = {
    .max_time_ints = 0,
    .remainder_time = 0,
    .cmd = timer_fn_nop
};
timer_t timer8 = {
    .max_time_ints = 0,
    .remainder_time = 0,
    .cmd = timer_fn_nop
};

/*
Starts the 16-bit timer to call a function repeatedly at some time interval.
seconds - number of seconds between function calls
cmd - function to call
*/
void start_timer_16bit(uint16_t seconds, timer_fn_t cmd) {
    // delay in ms
    uint32_t delay = seconds * 1000L;
    //number of interrupts
    timer16.max_time_ints = delay / MAX_TIME_16BIT;
    uint32_t remaining_delay = delay - (timer16.max_time_ints * MAX_TIME_16BIT);
    //remaining counter value
    timer16.remainder_time = (remaining_delay / PERIOD) + ROUND;

    //command to run
    timer16.cmd = cmd;

    // set timer to CTC mode - using OCR1A
    TCCR1A &= ~(_BV(WGM10) | _BV(WGM11));
    TCCR1B |= _BV(WGM12);
    TCCR1B &= ~_BV(WGM13);

    // set timer to use internal clock with prescaler of 1024
    TCCR1B |= _BV(CS12) | _BV(CS10);
    TCCR1B &= ~_BV(CS11);

    // disable use of output compare pins so that they can be used normally
    TCCR1A &= ~(_BV(COM1A1) | _BV(COM1A0) | _BV(COM1B1) | _BV(COM1B0));

    // initialize counter at 0
    TCNT1 = 0;
    // set compare value
    OCR1A = 0xFFFF;

    // enable output compare interupt
    TIMSK1 |= _BV(OCIE1A);
    // enable global interrupts
    sei();
}

/*
Starts the 8-bit timer to call a function repeatedly at some time interval.
seconds - number of seconds between function calls
cmd - function to call
*/
void start_timer_8bit(uint16_t seconds, timer_fn_t cmd) {
    // delay in ms
    uint32_t delay = seconds * 1000L;
    // number of interrupts
    timer8.max_time_ints = delay / MAX_TIME_8BIT;
    uint32_t remaining_delay = delay - (timer8.max_time_ints * MAX_TIME_8BIT);
    //remaining counter value
    timer8.remainder_time = (remaining_delay / PERIOD) + ROUND;

    //command to run
    timer8.cmd = cmd;

    // set timer to CTC mode - using OCR0A
    TCCR0A &= ~_BV(WGM00);
    TCCR0A |= _BV(WGM01);
    TCCR0B &= ~_BV(WGM02);

    // set timer to use internal clock with prescaler of 1024
    TCCR0B |= _BV(CS02) | _BV(CS00);
    TCCR0B &= ~_BV(CS01);

    // disable use of output compare pins so that they can be used as normal pins
    TCCR0A &= ~(_BV(COM0A1) | _BV(COM0A0) | _BV(COM0B1) | _BV(COM0B0));

    // initialize 8 bit counter at 0
    TCNT0 = 0;
    // set compare value
    OCR0A = 0xFF;

    // enable output compare interupt
    TIMSK0 |= _BV(OCIE0A);
    // enable global interrupts
    sei();
}

/*
Stops the 16-bit timer so it will stop calling the command function.
*/
void stop_timer_16bit(void) {
    //stop timer by clearing bits
    TCCR1B &= ~(1 << CS10);
    TCCR1B &= ~(1 << CS11);
    TCCR1B &= ~(1 << CS12);
}

/*
Stops the 8-bit timer so it will stop calling the command function.
*/
void stop_timer_8bit(void) {
    // stop timer by clearing bits
    TCCR0B &= ~(1 << CS00);
    TCCR0B &= ~(1 << CS01);
    TCCR0B &= ~(1 << CS02);
}


// Counts the number of interrupts that have occured for the 16 bit timer
volatile uint32_t timer16_int_counter = 0;

// Counts the number of interrupts that have occured for the 8 bit timer
volatile uint32_t timer8_int_counter = 0;

// This ISR occurs when TCNT1 is equal to OCR1A for a 16-bit timer
// Timer 1 compare match A handler
ISR(TIMER1_COMPA_vect) {
    timer16_int_counter += 1; //counting number of interrupts

    if (timer16_int_counter == timer16.max_time_ints) {
        OCR1A = timer16.remainder_time;
    }
    else if (timer16_int_counter >= timer16.max_time_ints + 1) {
        // the desired time has passed
        timer16_int_counter = 0; // reset the number of interrupts to 0
        OCR1A = 0xFFFF; //set compare value
        (timer16.cmd)();
    }
}

// This ISR occurs when TCNT0 is equal to OCR0A for a 8-bit timer
// Timer 0 compare match A handler
ISR(TIMER0_COMPA_vect) {
    timer8_int_counter += 1; //counting number of interrupts

    if (timer8_int_counter == timer8.max_time_ints) {
        OCR0A = timer8.remainder_time;
    }
    else if (timer8_int_counter >= timer8.max_time_ints + 1) {
        // the desired time has passed
        timer8_int_counter = 0; // reset the number of interrupts to 0
        OCR0A = 0xFF; //set compare value
        (timer8.cmd)();
    }
}
