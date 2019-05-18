#include <watchdog/watchdog.h>

/* default callback function */
void _wdt_cb_nop(void){
}

/* Global variable specifiying callback function */
wdt_cb_t wdt_cb = _wdt_cb_nop;

/*
How many times the interrupt has occurred (restarts from 0 when the
microcontroller resets)
volatile ints can be changed at any time (i.e. during ISR)
*/
volatile uint32_t wdt_int_count = 0;

void set_wdt_cb(wdt_cb_t cb) {
    wdt_cb = cb;
}

/* Watchdog interrupt handler: Interrupt triggers when WDIE is set */
ISR(WDT_vect){
    wdt_int_count++;
    wdt_cb();
}
