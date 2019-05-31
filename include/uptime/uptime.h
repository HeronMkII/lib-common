#ifndef UPTIME_H
#define UPTIME_H

#include <stdint.h>

#include <avr/eeprom.h>

#include <timer/timer.h>
#include <uart/uart.h>
#include <watchdog/watchdog.h>

// EEPROM address for storing number of resets
#define RESTART_COUNT_EEPROM_ADDR ((uint32_t*) 0x60)
// EEPROM address for storing reason for last reset
#define RESTART_REASON_EEPROM_ADDR ((uint32_t*) 0x64)

// Number of seconds between timer callbacks
#define UPTIME_TIMER_PERIOD 1

// Number of functions to be called from the same timer
#define UPTIME_NUM_CALLBACKS 5

// Possible reasons for reset

// Watchdog timeout in normal operation (8 seconds)
#define UPTIME_RESTART_REASON_WDT_TIMEOUT   0x01
// Received and executed a reset command (from ground station, either directly
// to OBC or forwarded by OBC)
#define UPTIME_RESTART_REASON_RESET_CMD     0x02
// Have not received a command for some amount of time (e.g. 6 hours)
#define UPTIME_RESTART_REASON_NO_CMD        0x03
// Was not able to record the reason for reset
// Generally when the external reset pin is driven low
// This is also the default value in EEPROM
#define UPTIME_RESTART_REASON_UNKNOWN       EEPROM_DEF_DWORD

typedef void(*uptime_fn_t)(void);

extern uint32_t restart_count;
extern volatile uint32_t uptime_s;
extern uint32_t restart_reason;

void init_uptime(void);
void update_restart_count(void);
uint8_t add_uptime_callback(uptime_fn_t callback);

void write_restart_reason(uint32_t reason);
void reset_self_mcu(uint32_t reason);

#endif
