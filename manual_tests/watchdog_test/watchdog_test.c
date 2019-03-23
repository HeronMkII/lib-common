#include <watchdog/watchdog.h>

//simulates a timeout

//set up watchdog timer

int main(void)
{
  init_uart();
  print("STARTING PROGRAM\n");

  WDT_ENABLE_SYS_RESET(WDTO_2S);

  //set up an infinite loop
  for(int i = 0; i<5; i++)
  {
    _delay_ms(1500);
    WDT_ENABLE_SYS_RESET(WDTO_2S);
    print("LOOP%d",i);
  }


  while(1){

    }


    print("FAILED");

  



  //Alternates LED_PORT between states (i.e. ISR triggering twice will get back to initial state)

  //LED_PORT ^= (1 << LED_PIN);
  //print("LED_PORT: %d\n", LED_PORT);


}
