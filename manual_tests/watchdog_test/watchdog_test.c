#include <uart/uart.h>
#include <utilities/utilities.h>
#include <watchdog/watchdog.h>

//simulates a timeout

//set up watchdog timer

int main(void)
{
  init_uart();
  WDT_OFF();
  print("STARTING PROGRAM\n");

  WDT_ENABLE_SYS_RESET(WDTO_2S);

  for(int i = 0; i<5; i++)
  {
    _delay_ms(1500);
    WDT_ENABLE_SYS_RESET(WDTO_2S);
    print("LOOP%d\n",i);
  }

  int counter = 0;

  //set up an infinite loop
  while(1){
      counter++;
      print("%d\n",counter);
      _delay_ms(100);
    }


    print("FAILED\n");
}
