/*
Authors: Brytni Richards

Tests timers by incrementing a global variable specific for both timers every minute
*/

#include <uart/uart.h>
#include <uart/log.h>
#include <timer/timer.h>
#include <timer/timer2.h>
#include <assert/assert.h>

int count1 = 0;//current timer 1 iteration
int count2 = 0;//current timer 2 iteration

void timer1_func(){
  count1 ++;
  print("TMR1:%d, ",count1);
}
void timer2_func(){
  count2 ++;
  print("TMR2:%d, ",count2);
}

int main() {
  init_uart();
  print("UART initialized\n");

  init_timer(1, timer1_func);//increases count every 1 minute
  init_timer2(5, timer2_func);//increases count every 5 minutes
  print("Timers started\n");

  while(1);
  return 0;
}
