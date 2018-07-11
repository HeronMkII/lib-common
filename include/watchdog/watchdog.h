#ifndef WATCHDOG_H
#define WATCHDOG_H


/*
 * watchdog timer
 * look at ch 11.8 of
 * http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8209-8-bit%20AVR%20ATmega16M1-32M1-64M1_Datasheet.pdf
 * for more detailed instruction and register spec
 */

/*
 * 3 MODES OF WATCHDOG TIMER
 * _WD_CONTROL_REG register has 2 field which are WDE and WDIE
 * system reset mode is enabled when WDE bit is set to 1 and
 * interrupt mode is enabled when WDIE bit is set to 1
 *
 * 1. system_reset mode: reset system
 * 2. interrupt mode: call interrupt handler
 * 3. both enabled: call interrupt handler, then automatically switch watchdog timer
 *                  to system reset mode
 *
 * It is often requried to use the third mode (enable both reset and interrupt)
 * to do some necessary operation before reset
 * (for example saving critical parameters)
 */

/*
 * STEPS TO CHANGE WATCHDOG SETUP
 * 1. write logic 1 to WDCE and WDE in _WD_CONTROL_REG
 * 2.
 */

// TODO: check WDP value

#include <stdint.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <spi/spi.h>
#include <uart/uart.h>
#include <uart/log.h>
//#include "global_header.h"

#define LED_PIN     PB4
#define LED_DDR     DDRB
#define LED_PORT    PORTB
#define TIMEOUT     WDTO_1S

#define wdt_clean_up() { MCUSR = 0;	wdt_disable(); }
//#define WDTCR _WD_CONTROL_REG

enum wdt_mode {
    SYS_RESET,
    INTERRUPTS,
    BOTH
};
enum test_method {
    PRINTING,
    LED_OUTPUT
};

/*
 * function: init_wdt
 *
 * initialize watchdog timer
 * steps: 1. disalbe interrupts
 *        2. clean up MCU Status Register (MCUSR)
 *           -> MCUSR has flags indicate type of reset that occurred
 *              and therefore requried to be cleaned before initializing wdt
 *           NOTE: currently entire MCUSR is getting wiped out but it might be
 *                 better if only reset WDRF bit, which act as a flag to
 *                 indicate if a system reset occurred due to watchdog timer
 *        3. set WDCE and WDE bit of _WD_CONTROL_REG to 1 which will enable
 *           to write on _WD_CONTROL_REG
 *        4. modify _WD_CONTROL_REG based on input parameters
 *
 *
 * params: wm will decide wich one of three mode watchdog timer will be using
 *         timeout will decide prescaler timeout value which controls how long
 *         the watchdog timer will wait before reseting the system
 *
 * NOTE: currently for testing purpose, the reset frequency will be forced to 1s
 *       regardless of the input parameters value, which should get fixed
 *       fix shouldn't be hard to implement
 */
void init_wdt(enum wdt_mode wm, uint8_t timeout);

/*
 * function: disable_wdt
 *
 * disable watchdog timer
 *
 */
void disable_wdt(void);

/*
 * function: wdt_set_timeout_value
 *
 * change prescaler timeout value which will change the reset frequrency
 * of watchdog timer
 *
 */
void wdt_set_timeout_value(uint8_t timeout);
/*
 * function: get_prescaler
 *
 * retrives prescaler timer value which determines how long watchdog timer will
 * wait for wdt_reset to get called before restarting the system
 *
 * mostly used for debugging
 *
 * return: prescaler timer value
 */
uint32_t get_prescaler(void);


#endif
