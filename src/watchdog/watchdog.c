#include <watchdog/watchdog.h>
#ifndef F_CPU
#define F_CPU 8000000UL
#endif

/*volatile ints can be changed at any time (i.e. during ISR)*/
volatile int LED_ON = 0;
volatile int interrupt_count = 0;

/* default callback function */
void cb(void){

}


/* Global variable specifiying callback function */
watchdog_function_t wdt_function = cb;


/* Watchdog interrupt handler: Interrupt triggers when WDIE is set */
ISR(WDT_vect){
  wdt_function();
}
