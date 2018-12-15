/*
Author: Brytni Richards

Tests timers by incrementing a global variable specific for both timers every
minute or 5 minutes
*/

#include <uart/uart.h>
#include <timer/timer.h>

int count1 = 0; //current timer 1 iteration
int count2 = 0; //current timer 2 iteration

void timer1_func() {
    count1++;
    print("Timer 1: %d\n", count1);
}
void timer2_func() {
    count2++;
    print("Timer 2: %d\n", count2);
}

int main() {
    init_uart();
    print("UART initialized\n");

    init_timer_16bit(1, timer1_func);//increases count every 1 minute
    init_timer_8bit(5, timer2_func);//increases count every 5 minutes
    print("Timers started\n");

    while (1) {};
    return 0;
}
