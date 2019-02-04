/*
 Author: Shimi Smith
 This is a timer that runs a given function every x minutes
 https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8209-8-bit%20AVR%20ATmega16M1-32M1-64M1_Datasheet.pdf
*/

#include <timer/timer.h>

static timer_t timer16;
static timer_t timer8;

void init_timer_16bit(uint8_t seconds, cmd_fn_t cmd) {
    uint32_t delay = seconds * 1000L;  // delay in ms
    timer16.ints = ((delay / MAX_TIME_16BIT));//number of interrupts
    timer16.remainder = ((delay - (timer16.ints * MAX_TIME_16BIT)) / PERIOD) + ROUND;//counter value
    timer16.cmd = cmd; //command to run

    // set timer to CTC mode - using OCR1Au
    TCCR1A &= ~(_BV(WGM10) | _BV(WGM11));
    TCCR1B |= _BV(WGM12);
    TCCR1B &= ~_BV(WGM13);

    // set timer to use internal clock with prescaler of 1024
    TCCR1B |= _BV(CS12) | _BV(CS10);
    TCCR1B &= ~_BV(CS11);

    // disable use of output compare pins so that they can be used normally
    TCCR1A &= ~(_BV(COM1A1) | _BV(COM1A0) | _BV(COM1B1) | _BV(COM1B0));

    TCNT1 = 0;  // initialize counter at 0
    OCR1A = 0xFFFF;  // set compare value

    // enable output compare interupt
    TIMSK1 |= _BV(OCIE1A);
    sei(); // enable global interrupts
}

void init_timer_8bit(uint8_t seconds, cmd_fn_t cmd) {
    uint32_t delay = seconds * 1000L;  // delay in ms
    timer8.ints = ((delay / MAX_TIME_8BIT)); // number of interrupts
    timer8.remainder = ((delay - (timer8.ints * MAX_TIME_8BIT)) / PERIOD) + ROUND; //counter value
    timer8.cmd = cmd; //command to run

    // set timer to CTC mode - using OCR0A
    TCCR0A &= ~_BV(WGM00);
    TCCR0A |= _BV(WGM01);
    TCCR0B &= ~_BV(WGM02);

    // set timer to use internal clock with prescaler of 1024
    TCCR0B |= _BV(CS02) | _BV(CS00);
    TCCR0B &= ~_BV(CS01);

    // disable use of output compare pins so that they can be used as normal pins
    TCCR0A &= ~(_BV(COM0A1) | _BV(COM0A0) | _BV(COM0B1) | _BV(COM0B0));

    TCNT0 = 0;  // initialize 8 bit counter at 0
    OCR0A = 0xFF;  // set compare value

    // enable output compare interupt
    TIMSK0 |= _BV(OCIE0A);
    sei(); // enable global interrupts
}

void stop_timer_16bit()
{
    //stop timer by clearing bits
    TCCR1B &= ~(1 << CS10);
    TCCR1B &= ~(1 << CS11);
    TCCR1B &= ~(1 << CS12);
}

void stop_timer_8bit()
{
    // stop timer by clearing bits
    TCCR0B &= ~(1 << CS00);
    TCCR0B &= ~(1 << CS01);
    TCCR0B &= ~(1 << CS02);
}

// Counts the number of interrupts that have occured for the 16 bit timer
volatile uint32_t counter16 = 0;

// Counts the number of interrupts that have occured for the 8 bit timer
volatile uint32_t counter8 = 0;

// This ISR occurs when TCNT1 is equal to OCR1A for a 16-bit timer
// Timer 1 compare match A handler
ISR(TIMER1_COMPA_vect) {
    counter16 += 1; //counting number of interrupts
    if (counter16 == timer16.ints) {
        OCR1A = timer16.remainder;
    }
    else if (counter16 >= timer16.ints + 1) {
        // the desired time has passed
        counter16 = 0; // reset the number of interrupts to 0
        OCR1A = 0xFFFF; //set compare value
        (timer16.cmd)();
    }
}

// This ISR occurs when TCNT0 is equal to OCR0A for a 8-bit timer
// Timer 0 compare match A handler
ISR(TIMER0_COMPA_vect) {
    counter8 += 1; //counting number of interrupts
    if (counter8 == timer8.ints) {
        OCR0A = timer8.remainder;
    }
    else if (counter8 >= timer8.ints + 1) {
        // the desired time has passed
        counter8 = 0; // reset the number of interrupts to 0
        OCR0A = 0xFF; //set compare value
        (timer8.cmd)();
    }
}
