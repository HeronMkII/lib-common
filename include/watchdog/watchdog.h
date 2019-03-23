#include <stdint.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <spi/spi.h>
#include <uart/uart.h>

#define LED_PIN PB4
#define LED_DDR DDRB
#define LED_PORT PORTB

/* Macros written in assembly are used to guarantee that the WDTCSR register is reset within 4 clock cycles of
    WDCE and WDE being set, as per the data sheet. Functions contain more overhead and take longer to execute,
    in addition to not running properly with regards to the watchdog timer (even un-optimized)*/

/* The following constants are pre-defined for use in the watchdog timer in ascending order
    - WDTO_15MS (0), WDTO_30MS (1), WDTO_60MS (2), WDTO_120MS (3), WDTO_250MS (4),
      WDTO_500MS (5), WDTO_1S (6), WDTO_2S (7), WDTO_4S (8), WDTO_8S (9)*/

/* wdt_enable(timeout) function is included in the <avr/wdt.h> header file and used for system reset,
    but for consistency reasons the following three macros are used instead (code is nearly-identical).*/

/* Functions are written in both assembly (more optimal to use) and C (for greater clarity)*/

/*
All functions have same structure:
  1) Disable interrupts
  2) Resets watchdog timer
  3) Sets change bits to enable modification of WDTCSR register
  4) Configures appropriate timeout and mode
  5) Re-enable interrupts
*/


typedef void(*watchdog_function_t)(void);



/* Triggers interrupts upon timeout */
 #define WDT_ENABLE_INTERRUPT(timeout)\
__asm__ __volatile__ ( \
    "in __tmp_reg__,__SREG__" "\n\t" \
    "cli" "\n\t" \
    "wdr" "\n\t" \
    "sts %0,%1" "\n\t" \
    "out __SREG__,__tmp_reg__" "\n\t" \
    "sts %0,%2" "\n\t" \
    : /* no outputs */  \
    : "M" (_SFR_MEM_ADDR(WDTCSR)), \
    "r" (_BV(WDCE) | _BV(WDE)), \
    "r" ((uint8_t) ((timeout & 0x08 ? _WD_PS3_MASK : 0x00) | \
        _BV(WDIE) | (timeout & 0x07)) ) \
    : "r0"  \
)
/*#define WDT_ENABLE_INTERRUPT(timeout)\
    MCUSR = 0;\
    cli();\
    wdt_reset();\
    WDTCSR = _BV(WDCE) | _BV(WDE);\
    //sets last 3 bits for timeout// \
    WDTCSR = _BV(WDIE) | (timeout % 8) | ((uint8_t)(timeout/8) << WDP3);\
    sei();
*/

/* Sends interrupt on initial timeout, then resets board on second timeout */
#define WDT_ENABLE_BOTH(timeout)\
__asm__ __volatile__ ( \
    "in __tmp_reg__,__SREG__" "\n\t" \
    "cli" "\n\t" \
    "wdr" "\n\t" \
    "sts %0,%1" "\n\t" \
    "out __SREG__,__tmp_reg__" "\n\t" \
    "sts %0,%2" "\n\t" \
    : /* no outputs */  \
    : "M" (_SFR_MEM_ADDR(WDTCSR)), \
    "r" (_BV(WDCE) | _BV(WDE)), \
    "r" ((uint8_t) ((timeout & 0x08 ? _WD_PS3_MASK : 0x00) | \
        _BV(WDE) | _BV(WDIE) | (timeout & 0x07)) ) \
    : "r0"\
)

/*#define WDT_ENABLE_BOTH(timeout)\
    MCUSR = 0;\
    cli();\
    wdt_reset();\
    WDTCSR = _BV(WDCE) | _BV(WDE);\
    //sets last 3 bits for timeout//
    WDTCSR = _BV(WDIE) | _BV(WDE) | (timeout % 8) | ((uint8_t)(timeout/8) << WDP3);\
    sei();*/

/* Resets board upon timeout */
#define WDT_ENABLE_SYS_RESET(timeout)\
__asm__ __volatile__ ( \
    "in __tmp_reg__,__SREG__" "\n\t" \
    "cli" "\n\t" \
    "wdr" "\n\t" \
    "sts %0,%1" "\n\t" \
    "out __SREG__,__tmp_reg__" "\n\t" \
    "sts %0,%2" "\n\t" \
    : /* no outputs */  \
    : "M" (_SFR_MEM_ADDR(WDTCSR)), \
    "r" (_BV(WDCE) | _BV(WDE)), \
    "r" ((uint8_t) ((timeout & 0x08 ? _WD_PS3_MASK : 0x00) | \
        _BV(WDE) | (timeout & 0x07)) ) \
    : "r0"  \
)

/*#define WDT_ENABLE_SYS_RESET(timeout)\
    MCUSR = 0;\
    cli();\
    wdt_reset();\
    WDTCSR = _BV(WDCE) | _BV(WDE);\
    //sets last 3 bits for timeout//
    WDTCSR = _BV(WDE) | (timeout % 8) | ((uint8_t)(timeout/8) << WDP3);\
    sei();
*/
/* wdt_reset() is included in the <avr/wdt.h> header file and prevents watchdog timer from timing out*/

/* wdt_disable() is included in the <avr/wdt.h> header file and turns off the watchdog timer
 on newer devices (i.e. 32M1), the WDT_OFF() macro defined below must be used instead*/

/* Turns watchdog timer off*/
#define WDT_OFF()\
    cli();\
    wdt_reset();\
    MCUSR &= ~(1 << WDRF);\
    WDTCSR |= (1 << WDCE) | ( 1 << WDE);\
    WDTCSR = 0x00;\
    sei();

/*
    void wdt_off(void){
        cli();
        wdt_reset();
        MCUSR &= ~(1 << WDRF);
        WDTCSR |= (1 << WDCE) | ( 1 << WDE);
        WDTCSR = 0x00;
        sei();
    }*/
