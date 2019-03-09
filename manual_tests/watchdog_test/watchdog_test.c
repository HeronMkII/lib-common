#include <watchdog/watchdog.h>

//simulates a timeout


volatile int interrupt_count = 0;

//set up watchdog timer

int main(void)
{
  print("WATCHDOG TIMER INTERRUPT\n");

  //set up an infinite loop
  while(1){
    if (interrupt_count == 25) {
      while(1){};
    }
    interrupt_count++;
    print("%dTH INTERRUPT\n", interrupt_count);
    //reset watchdog timer
    wdt_reset();
    
  }



  //Alternates LED_PORT between states (i.e. ISR triggering twice will get back to initial state)
  LED_PORT ^= (1 << LED_PIN);
  print("LED_PORT: %d\n", LED_PORT);


}
