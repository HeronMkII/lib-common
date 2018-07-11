#include "watchdog.h"

volatile int LED_ON = 0;
volatile int interrupt_count = 0;
char* wdt_mode_to_string = {"SYS RESET","INTERRUPT","BOTH"};


uint32_t get_prescaler(void){
    uint8_t fourth = (_WD_CONTROL_REG>>WDP3)%2;
    uint8_t third = (_WD_CONTROL_REG>>WDP2)%2;
    uint8_t second = (_WD_CONTROL_REG>>WDP1)%2;
    uint8_t first = (_WD_CONTROL_REG>>WDP0)%2;
    uint32_t prescaler = fourth*8+third*4+second*2+first;
    print("%d",_WD_CONTROL_REG);
    wdt_reset();
    return prescaler;
}


void init_wdt(enum wdt_mode wm, uint8_t timeout){
    cli(); /*disable interrupt*/
    //wdt_reset();
    //wdt_clean_up();
    wdt_disable();
    print("\n\nSTART\n\n");
    /*set wdt_mode and timeout*/
    switch(wm){
        case SYS_RESET:
            MCUSR = 0;
            /*clean _WD_CONTROL_REG values*/
            _WD_CONTROL_REG = (1<<WDCE) | (1<<WDE);
            /*set wdt mode and timeout value*/
            _WD_CONTROL_REG = (1<<WDE) | (1<<WDP2) | (1<<WDP1);
            print("%d",_WD_CONTROL_REG);
            break;
        case INTERRUPTS:
            MCUSR = 0;
            /*clean _WD_CONTROL_REG values*/
            _WD_CONTROL_REG = (1<<WDCE) | (1<<WDE);
            /*set wdt mode and timeout value*/
            _WD_CONTROL_REG = (1<<WDIE) | (1<<WDP2) | (1<<WDP1);
            break;
        case BOTH:
            MCUSR = 0;
            /*clean _WD_CONTROL_REG values*/
            _WD_CONTROL_REG = (1<<WDCE)|(1<<WDE);
            /*set wdt mode and timeout value*/
            _WD_CONTROL_REG = (1<<WDE)| (1<<WDIE) | (1<<WDP2) | (1<<WDP1);
            print("%d",_WD_CONTROL_REG);
            break;
    }
    wdt_reset();
    //print("1 %d   %d",_WD_CONTROL_REG,WDE);
    //wdt_set_timeout_value(timeout);
    sei(); /*enable interrupt*/


    //print("\nWDP%d %d %d %d\n", (_WD_CONTROL_REG>>WDP3) % 2,(_WD_CONTROL_REG>>WDP2) % 2,(_WD_CONTROL_REG>>WDP1) % 2,(_WD_CONTROL_REG>>WDP0) % 2);
    //print("\nINITIALZING\n");
    //print("\nWDT MODE IS %s",wdt_mode_to_string[wm]);
}

void disable_wdt(){
    print("DISABLE WDT\n");
    cli();
    wdt_reset();
    MCUSR &= ~(1<<WDRF); //TODO: might be 0?
    _WD_CONTROL_REG = (1<<WDCE)|(1<<WDE);
    _WD_CONTROL_REG = 0;
    sei();
}

void wdt_set_timeout_value(uint8_t timeout){
    //print("RESETING TIMEOUT VALUE");
    cli();
    wdt_reset();
    uint8_t old_wde = (_WD_CONTROL_REG>>WDE) % 2; // save old WDE value
    uint8_t old_wdie = (_WD_CONTROL_REG>>WDIE) % 2; // save old WDIE value
    wdt_reset();
    // parsing timeout to bits
    uint8_t wdp_array[3];
    uint8_t temp = timeout;
    uint8_t factor = 8;
    for (int i = 3; i >= 0; i--){
        wdp_array[i] = (int) temp/factor;
        temp = temp - wdp_array[i]*factor;
        factor = factor/2;
        wdt_reset();
    }
    print("dsfs %d %d %d %d\n", wdp_array[3],wdp_array[2],wdp_array[1],wdp_array[0]);
    wdt_reset();
    _WD_CONTROL_REG = (1<<WDCE)|(1<<WDE);
    _WD_CONTROL_REG = (old_wde<<WDE) | (old_wdie<<WDIE) | (wdp_array[3]<<WDP3)
                         | (wdp_array[2]<<WDP2) | (wdp_array[1]<<WDP1) | (wdp_array[0]<<WDP0);
    sei();
}

// watchdog interrupt handler
ISR(WDT_vect){
    print("WATCHDOG TIMER INTERRUPT\n");
    interrupt_count++;
    print("%dTH INTERRUPT\n", interrupt_count);
    LED_PORT ^= (1<<LED_PIN);
}
