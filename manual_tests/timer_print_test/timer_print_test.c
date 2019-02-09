/*
Author: Brytni Richards

Tests timers by incrementing a global variable specific for both timers every
minute or 5 minutes
*/

#include <uart/uart.h>
#include <timer/timer.h>

volatile uint32_t count1 = 0; //current timer 1 iteration
volatile uint32_t count2 = 0; //current timer 2 iteration

void timer1_func() {
    count1++;
    print("Timer 1 triggered: %lu\n", count1);
}
void timer2_func() {
    count2++;
    print("Timer 2 triggered: %lu\n", count2);
}

int main() {
    init_uart();
    print("\n\nStarting test\n");

    start_timer_16bit(1, timer1_func);//increases count every 1 second
    print("Started timer 1: every 1 second\n");
    start_timer_8bit(11, timer2_func);//increases count every 15 seconds
    print("Started timer 2: every 11 seconds\n");

    // Stop when we hit some arbitrary conditions
    while (1) {
        if (count1 >= 40 && count2 >= 3) {
            print("Found stop condition\n");
            break;
        }
    }

    stop_timer_16bit();
    stop_timer_8bit();
    print("Stopped both timers\n");
    print("Waiting 30 seconds...");
    _delay_ms(30000);

    start_timer_16bit(5, timer1_func);
    print("Started timer 1: every 5 second\n");
    start_timer_8bit(2, timer2_func);
    print("Started timer 2: every 2 seconds\n");

    print("Test done: timers continue forever\n");

    while(1) {}

    return 0;
}
