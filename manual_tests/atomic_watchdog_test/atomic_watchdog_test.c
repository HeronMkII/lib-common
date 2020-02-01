#include <uart/uart.h>
#include <utilities/utilities.h>
#include <watchdog/watchdog.h>

// Tests the behavior of the watchdog timer when interrupts are disabled

//set up watchdog timer

void cb(void) {
  print("Interrupted\n");
}

int main(void)
{
  init_uart();
  WDT_OFF();  // Should occur at start of program to clear the timeout
  print("\nSTARTING PROGRAM\n");
  set_wdt_cb(cb);

  WDT_ENABLE_INTERRUPT(WDTO_2S);

  // Set up an infinite state inside an atomic block.
  /*
   *  Recorded behavior:
   *   If system reset is enabled, watchdog resets system after timeout is reached. Atomic block has no 
   *    effect on the reset.
   *  
   *  Interrupts are disabled.
   */

  // ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    while(1)
    {
      print("delay\n");
      _delay_ms(200);
    }
  // }

  print("FAILED\n");
}
