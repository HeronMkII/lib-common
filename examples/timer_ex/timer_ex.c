#include <uart/uart.h>
#include <uart/log.h>
#include <timer/timer.h>
#include <timer/timer2.h>
#include <assert/assert.h>

//Tests timers

int count1 = 0;//current timer 1 iteration
int count2 = 0;//current timer 2 iteration

void timer1_func(){
  count1 ++;
  print("TMR1: %d",count1);
}
void timer2_func(){
  count2 ++;
  print("TMR2: %d",count2);
}
/*
uint8_t is_register_high(){

}

uint8_t is_pin_high(){

}*/

int main() {
  init_uart();
  print("UART initialized\n");
  //print("1 %f 2 %f 1T %f 2T %f \n", MAX_TIME, MAX_TIME2, T, T2);

  init_timer(1, timer1_func);//increases count every 0.1 minutes
  init_timer2(1, timer2_func);

  //ASSERT
  //Checking set_vars for struct
  /*set_vars(1, timer1_func);
  //Delay = 1*60L*1000L = 60000
  ASSERT(timer.int == 60000 / MAX_TIME));
  ASSERT(timer.count == ((60000 - (timer.ints * MAX_TIME)) / T) + ROUND);

  //Checking registers
  init_timer2(1, timer2_func);
  // set timer to CTC mode - using OCR1A
  uint8_t TCC_copy = TCCR0A;
  TCC_copy &= ~(_BV(WGM00) | _BV(WGM01));//only registers that we care about
  ASSERT(TCC_copy == _BV(WGM00));//only register to be high
*/
  while(1);
  return 0;
}
