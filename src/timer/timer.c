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
    timer16.ints = ((delay / MAX_TIME_16BIT));
    timer16.count = ((delay - (timer16.ints * MAX_TIME_16BIT)) / T) + ROUND;
    timer16.cmd = cmd;

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
    timer8.ints = ((delay / MAX_TIME_8BIT));
    timer8.count = ((delay - (timer8.ints * MAX_TIME_8BIT)) / T) + ROUND;
    timer8.cmd = cmd;

    // set timer to CTC mode - using OCR0A
    TCCR0A &= ~_BV(WGM00);
    TCCR0A |= _BV(WGM01);
    TCCR0B &= ~_BV(WGM02);

    // set timer to use internal clock with prescaler of 1024
    TCCR0B |= _BV(CS02) | _BV(CS00);
    TCCR0B &= ~_BV(CS01);

    // disable use of Output compare pins so that they can be used as normal pins
    TCCR0A &= ~(_BV(COM0A1) | _BV(COM0A0) | _BV(COM0B1) | _BV(COM0B0));

    TCNT0 = 0;  // initialize 8 bit counter at 0
    OCR0A = 0xFF;  // set compare value

    // enable output compare interupt
    TIMSK0 |= _BV(OCIE0A);
    sei(); // enable global interrupts
}

// Counts the number of interrupts that have occured for the 16 bit timer
volatile uint32_t counter16 = 0;

// Counts the number of interrupts that have occured for the 8 bit timer
volatile uint32_t counter8 = 0;

// This ISR occurs when TCNT1 is equal to OCR1A
ISR(TIMER1_COMPA_vect) {
    counter16 += 1;
    if (counter16 == timer16.ints) {
        OCR1A = timer16.count;
    } else if (counter16 >= timer16.ints + 1) {
        // the desired time has passed
        counter16 = 0;
        OCR1A = 0xFFFF;
        (timer16.cmd)();
    }
}

// This ISR occurs when TCNT0 is equal to OCR0A

ISR(TIMER0_COMPA_vect) {
    // TODO: Should this happen in an atomic block?
    counter8 += 1;
    if (counter8 == timer8.ints) {
        OCR0A = timer8.count;
    } else if (counter8 >= timer8.ints + 1) {
        // the desired time has passed
        counter8 = 0;
        OCR0A = 0xFF;
        (timer8.cmd)();
    }
}

// TODO: write an ISR for 16 bit timer?
