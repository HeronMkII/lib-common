/*
 Authors: Shimi Smith, Matthew Silverman
 This is a timer that runs a given function every x minutes
*/

#include <timer/timer2.h>

static Timer2 timer2;

void set_vars2(uint8_t minutes, Command cmd){
    uint32_t delay = minutes * 60L * 1000L;  // delay in ms
    timer2.ints = ((delay / MAX_TIME2));
    timer2.count = ((delay - (timer2.ints * MAX_TIME2)) / T2) + ROUND2;
    timer2.cmd = cmd;
}

void init_timer2(uint8_t minutes, Command cmd){
    set_vars2(minutes, cmd);

    // set timer to CTC mode - using OCR0A
    TCCR0A &= ~_BV(WGM00);
    TCCR0A |= _BV(WGM01);
    TCCR0B &= ~_BV(WGM02);

    // set timer to use internal clock with prescaler of 1024
    TCCR0B |= _BV(CS02) | _BV(CS00);
    TCCR0B &= ~_BV(CS01);

    // disable use of Output compare pins so that they can be used as normal pins
    TCCR0A &= ~(_BV(COM0A1) | _BV(COM0A0) | _BV(COM0B1) | _BV(COM0B0));

    TCNT0 = 0;  // initialize counter at 0
    OCR0A = 0xFF;  // set compare value

    // enable output compare interupt
    TIMSK0 |= _BV(OCIE0A);
    sei(); // enable global interrupts
}

volatile uint64_t counter2 = 0;  // the number of interrupts that have occured
// This ISR occurs when TCNT0 is equal to OCR0A
ISR(TIMER0_COMPA_vect){

    counter2++;

    if(counter2 == timer2.ints){
        OCR0A = timer2.count;
    }
    else if(counter2 >= timer2.ints + 1){  // the desired time has passed
        counter2 = 0;
        OCR0A = 0xFF;

        (*(timer2.cmd))();

    }
}
