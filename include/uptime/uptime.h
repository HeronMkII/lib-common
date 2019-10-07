#ifndef UPTIME_H
#define UPTIME_H

#include <stdint.h>

#include <avr/eeprom.h>

#include <timer/timer.h>
#include <uart/uart.h>
#include <watchdog/watchdog.h>

// EEPROM address for storing number of resets
#define RESTART_COUNT_EEPROM_ADDR   0x10
// EEPROM address for storing reason for last reset
#define RESTART_REASON_EEPROM_ADDR  0x14

// Number of seconds between timer callbacks
#define UPTIME_TIMER_PERIOD 1

// Number of functions to be called from the same timer
#define UPTIME_NUM_CALLBACKS 5


// Number in seconds, equal to 1 day
#define CMD_TIMER_DEF_PERIOD    (60UL * 60UL * 24UL)
// Number in seconds
#define CMD_TIMER_CB_INTERVAL   1


// Possible reasons for reset

// Watchdog timeout in normal operation (8 seconds)
#define UPTIME_RESTART_REASON_WDT_TIMEOUT   0x01
// Received and executed a reset command (from ground station, either directly
// to OBC or forwarded by OBC)
#define UPTIME_RESTART_REASON_RESET_CMD     0x02
// Have not received a command for some amount of time (e.g. 6 hours)
#define UPTIME_RESTART_REASON_CMD_TIMER     0x03
//Watchdog System Reset
#define UPTIME_RESTART_REASON_WDRF          0x04 //WDRF  is 00001000 or 0x08
//Brown-out Reset
#define UPTIME_RESTART_REASON_BORF          0x05 //BORF  is 00000100 or 0x04
//External Reset
#define UPTIME_RESTART_REASON_EXTRF         0x06 //EXTRF is 00000010 or 0x02
//Power-on Reset
#define UPTIME_RESTART_REASON_PORF          0x07 //PORF  is 00000001 or 0x01
// Was not able to record the reason for reset
// Generally when the external reset pin is driven low
// This is also the default value in EEPROM
#define UPTIME_RESTART_REASON_UNKNOWN       EEPROM_DEF_DWORD

typedef void(*uptime_fn_t)(void);

extern uint32_t restart_count;
extern uint32_t restart_reason;
extern volatile uint32_t uptime_s;

extern volatile uint32_t cmd_timer_count_s;
extern uint32_t cmd_timer_period_s;

void init_uptime(void);
void update_restart_count(void);
uint8_t add_uptime_callback(uptime_fn_t callback);

void init_cmd_timer(void);
void restart_cmd_timer(void);
void cmd_timer_cb(void);

void write_restart_reason(uint32_t reason);
void reset_self_mcu(uint32_t reason);

#endif
