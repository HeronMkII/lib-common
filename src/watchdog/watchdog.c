#include <watchdog/watchdog.h>
#ifndef F_CPU
#define F_CPU 8000000UL
#endif

/*volatile ints can be changed at any time (i.e. during ISR)*/
volatile int LED_ON = 0;
volatile int interrupt_count = 0;

/* Watchdog interrupt handler: Interrupt triggers when WDIE is set */
ISR(WDT_vect){
  

  print("WATCHDOG TIMER INTERRUPT\n");
  interrupt_count++;
  print("%dTH INTERRUPT\n", interrupt_count);
  //Alternates LED_PORT between states (i.e. ISR triggering twice will get back to initial state)
  LED_PORT ^= (1 << LED_PIN);
  print("LED_PORT: %d\n", LED_PORT);
}
