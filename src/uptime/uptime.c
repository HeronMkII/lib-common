/*
Functionality to track a global uptime (in seconds since last restart) across
the microcontroller (MCU) using the 8-bit timer. Also tracks the number of times
the MCU has restarted using EEPROM.

This library basically multiplexes a set of timer callbacks onto a single timer
(the 8-bit timer). You can add multiple callbacks, which will all be called
at the same time (in order) every second. They can read the `uptime_s` variable
to decide what to do.

This library also has the capability for the microcontroller to reset itself if
it wants to. It uses EEPROM to keep track of the reason for the most recent reset.
*/

#include <uptime/uptime.h>

// Variables modified inside the timer interrupt must be volatile
// Note that 1 billion seconds is about 31.7 years (reasoning for using 32-bit
// uptime_s variable)

// Number of times the MCU has started up, i.e. how many times the program has
// started from the beginning (includes 1 for the first time)
uint32_t restart_count = 0;
// The restart reason loaded from EEPROM on startup (when init_uptime() is called)
// The EEPROM will be cleared after setting this value
uint32_t restart_reason = UPTIME_RESTART_REASON_UNKNOWN;
// Uptime (in seconds) - since most recent restart
volatile uint32_t uptime_s = 0;


// No-op default callbacks
void uptime_fn_nop(void) {}
// Array of timer callbacks for uptime timer callback
uptime_fn_t uptime_callbacks[UPTIME_NUM_CALLBACKS] = {uptime_fn_nop};

volatile uint32_t cmd_timer_count_s = 0;
uint32_t cmd_timer_period_s = CMD_TIMER_DEF_PERIOD;


void uptime_timer_cb(void);
void uptime_wdt_cb(void);

void cmd_timer_cb(void);


void init_uptime(void) {
    // Update restart count
    update_restart_count();

    // Read restart reason
    restart_reason = eeprom_read_dword(RESTART_REASON_EEPROM_ADDR);
    //If there's no restart reason, read the MCUSR for a restart reason
    if (restart_reason == EEPROM_DEF_DWORD) {
        if (MCUSR & _BV(WDRF)) restart_reason = UPTIME_RESTART_REASON_WDRF;
        if (MCUSR & _BV(BORF)) restart_reason = UPTIME_RESTART_REASON_BORF;
        if (MCUSR & _BV(EXTRF)) restart_reason = UPTIME_RESTART_REASON_EXTRF;
        if (MCUSR & _BV(PORF)) restart_reason = UPTIME_RESTART_REASON_PORF;
    }
    MCUSR = 0;
    // Clear reset reason (set EEPROM to default value)
    write_restart_reason(UPTIME_RESTART_REASON_UNKNOWN);
    // Set the callback function for WDT reset
    set_wdt_cb(uptime_wdt_cb);

    // Set all callbacks to no-op initially, just in case
    for (uint8_t i = 0; i < UPTIME_NUM_CALLBACKS; i++) {
        uptime_callbacks[i] = uptime_fn_nop;
    }

    // Initialize timer to go off at regular intervals
    start_timer_8bit(UPTIME_TIMER_PERIOD, uptime_timer_cb);
}

void update_restart_count(void) {
    // Read the restart count stored in EEPROM
    restart_count = eeprom_read_dword(RESTART_COUNT_EEPROM_ADDR);
    if (restart_count == EEPROM_DEF_DWORD) {
        restart_count = 0;
    }

    // Increment the restart count and write it back to EEPROM
    restart_count++;
    eeprom_write_dword(RESTART_COUNT_EEPROM_ADDR, restart_count);
}

// Adds a callback to the next available spot in the array
// Returns - 1 for success, 0 for failure (no spots left)
uint8_t add_uptime_callback(uptime_fn_t callback) {
    for (uint8_t i = 0; i < UPTIME_NUM_CALLBACKS; i++) {
        if (uptime_callbacks[i] == uptime_fn_nop) {
            uptime_callbacks[i] = callback;
            return 1;
        }
    }

    return 0;
}

// This timer should be called repeatedly (every 1 second) to keep track of uptime
void uptime_timer_cb(void) {
    // Update uptime
    uptime_s += UPTIME_TIMER_PERIOD;

    // print("uptime timer cb\n");
    // print("uptime_s = %lu\n", uptime_s);

    // Call all of the callback functions
    for (uint8_t i = 0; i < UPTIME_NUM_CALLBACKS; i++) {
        uptime_callbacks[i]();
    }
}


// Use the 16-bit timer to isolate it from uptime functionality as a redundancy measure
void init_cmd_timer(void) {
    start_timer_16bit(CMD_TIMER_CB_INTERVAL, cmd_timer_cb);
}

void restart_cmd_timer(void) {
    // Just in case there are any problems writing a 32-bit register
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        cmd_timer_count_s = 0;
    }
}

void cmd_timer_cb(void) {
    cmd_timer_count_s += (uint32_t) CMD_TIMER_CB_INTERVAL;
    if (cmd_timer_count_s >= cmd_timer_period_s) {
        reset_self_mcu(UPTIME_RESTART_REASON_CMD_TIMER);
        // Program should stop here and restart from the beginning
    }
}


void write_restart_reason(uint32_t reason) {
    eeprom_write_dword(RESTART_REASON_EEPROM_ADDR, reason);
}

void uptime_wdt_cb(void) {
    write_restart_reason(UPTIME_RESTART_REASON_WDT_TIMEOUT);
}

/*
Intentionally times out the watchdog timer to reset the microcontroller running
this program.
NOTE: The program will not continue after calling this function. It will restart
from the beginning.
*/
void reset_self_mcu(uint32_t reason) {
    // Only enable the system reset, not the interrupt
    // If we use the interrupt, it will write the restart reason as an
    // uninteniontal WDT timeout
    // Write the restart reason as whatever we intentionally specify
    write_restart_reason(reason);
    WDT_ENABLE_SYS_RESET(WDTO_15MS);
    _delay_ms(100);
}
