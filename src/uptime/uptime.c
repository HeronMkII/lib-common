/*
Functionality to track a global uptime (in seconds since last restart) across
the microcontroller (MCU) using the 8-bit timer. Also tracks the number of times
the MCU has restarted using EEPROM.

This library basically multiplexes a set of timer callbacks onto a single timer
(the 8-bit timer). You can add multiple callbacks, which will all be called
at the same time (in order) every second. They can read the `uptime_s` variable
to decide what to do.
*/

#include <uptime/uptime.h>

// Variables modified inside the timer interrupt must be volatile
// Note that 1 billion seconds is about 31.7 years (reasoning for using 32-bit
// uptime_s variable)

// Number of times the MCU has started up, i.e. how many times the program has
// started from the beginning (includes 1 for the first time)
uint32_t restart_count = 0;
// Uptime (in seconds) - since most recent restart
volatile uint32_t uptime_s = 0;

// No-op default callbacks
void uptime_fn_nop(void) {}

// Array of timer callbacks for uptime timer callback
uptime_fn_t uptime_callbacks[UPTIME_NUM_CALLBACKS] = {uptime_fn_nop};

void uptime_timer_cb(void);


void init_uptime(void) {
    // Update restart count
    update_restart_count();

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
