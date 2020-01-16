/*
Full test of the heartbeat system
*/

#include <heartbeat/heartbeat.h>
#include <uart/uart.h>

// -----------------------------------------------------------------------------
// Change these values before re-uploading to a new board

// NOTE: Change this variable before re-compiling and re-uploading to match the
// subsystem of the board you are uploading to
// #define SELF_ID HB_OBC
// #define SELF_ID HB_EPS
#define SELF_ID HB_PAY

// Uncomment to ignore received pings and not respond
// #define IGNORE_PINGS

// Uncomment to overwrite hb_ping_period_s (if IGNORE_PINGS is enabled, recommended to set this high so it doesn't reset the other board)
#define PING_PERIOD 5

// Uncomment for more debug logging
#define DEBUG_LOG
// -----------------------------------------------------------------------------


void print_bool(char* str, bool val) {
    if (val) {
        print("%s: true\n", str);
    }
}


int main(void) {
    init_uart();
    print("\n\n");
    init_uptime();
    init_can();
    print("Starting heartbeat test\n");

    // optional override for ping period
#ifdef PING_PERIOD
    hb_req_period_s = PING_PERIOD;
#endif

#ifndef IGNORE_PINGS
    init_hb(SELF_ID);
    print("Initialized heartbeat\n");
#else
    print("Skipped initializing heartbeat\n");
#endif

    print("Self: %u (%s)\n", self_hb_dev->id, self_hb_dev->name);
    print("hb_req_period_s = %lu\n", hb_req_period_s);
    print("Starting main loop\n");

    uint32_t last_uptime = uptime_s;

    while (1) {
        run_hb();

#ifdef DEBUG_LOG
        // Every 5 seconds, print collected count and reason for each device
        if (uptime_s > last_uptime + 5){
            last_uptime = uptime_s;

            for (uint8_t i = 0; i < HB_NUM_DEVS; i++) {
                hb_dev_t* dev = (hb_dev_t*) all_hb_devs[i];
                print("%s: count = %lu, reason = %u\n", dev->name, dev->restart_count, dev->restart_reason);
            }

            print("Stored count: %lu\n", restart_count);
            print("Stored reason: %u\n", restart_reason);
        }
#endif

    }

    return 0;
}
