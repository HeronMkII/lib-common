/*
Full test of the heartbeat system
*/

#include <heartbeat/heartbeat.h>
#include <uart/uart.h>

// -----------------------------------------------------------------------------
// Change these values before re-uploading to a new board

// NOTE: Change this variable before re-compiling and re-uploading to match the
// subsystem of the board you are uploading to
#define SELF_ID HB_OBC
// #define SELF_ID HB_EPS
// #define SELF_ID HB_PAY

// Uncomment to ignore received pings and not respond
#define IGNORE_PINGS true

// Uncomment to overwrite hb_ping_period_s (if IGNORE_PINGS is enabled, recommended to set this high so it doesn't reset the other board)
#define PING_PERIOD 100

// Uncomment for more debug logging
 #define DEBUG_LOG false
// -----------------------------------------------------------------------------


void print_bool(char* str, bool val) {
    if (val) {
        print("%s: true\n", str);
    }
}


int main() {
    init_uart();
    init_uptime();
    init_can();
    print("\n\n\nStarting heartbeat test\n");

    if (SELF_ID == HB_OBC) {
        hb_ping_period_s = 15;
    } else if (SELF_ID == HB_EPS) {
        hb_ping_period_s = 20;
    } else if (SELF_ID == HB_PAY) {
        hb_ping_period_s = 25;
    }

    // optional override for ping period
    if (PING_PERIOD){
        hb_ping_period_s = PING_PERIOD;
    }

    if (!IGNORE_PINGS){
        print("Initializing heartbeat...\n");
        init_hb(SELF_ID);
        print("Done init\n");
    }
    else {
        print("Skipped initializing heartbeat\n");
    }

    print("hb_self_id = ");
    if (hb_self_id == HB_OBC) {
        print("OBC\n");
    } else if (hb_self_id == HB_EPS) {
        print("EPS\n");
    } else if (hb_self_id == HB_PAY) {
        print("PAY\n");
    }

    print("hb_ping_period_s = %lu\n", hb_ping_period_s);

    print("Starting main loop\n");


    while (1) {

        // set flags for reset data requests
        if (uptime_s % 5 == 0 && hb_self_id != HB_OBC){
            hb_send_obc_rdata_req = true;
        }
        if (uptime_s % 5 == 1 && hb_self_id != HB_EPS){
            hb_send_eps_rdata_req = true;
        }
        if (uptime_s % 5 == 2 && hb_self_id != HB_PAY){
            hb_send_pay_rdata_req = true;
        }

        run_hb();
        if (DEBUG_LOG){
            print_bool("hb_send_obc_ping", hb_send_obc_ping);
            print_bool("hb_send_eps_ping", hb_send_eps_ping);
            print_bool("hb_send_pay_ping", hb_send_pay_ping);

            print_bool("hb_received_obc_resp", hb_received_obc_resp);
            print_bool("hb_received_eps_resp", hb_received_eps_resp);
            print_bool("hb_received_pay_resp", hb_received_pay_resp);

            print_bool("hb_send_obc_resp", hb_send_obc_resp);
            print_bool("hb_send_eps_resp", hb_send_eps_resp);
            print_bool("hb_send_pay_resp", hb_send_pay_resp);

            print_bool("hb_received_obc_rdata_req", hb_received_obc_rdata_req);
            print_bool("hb_received_eps_rdata_req", hb_received_eps_rdata_req);
            print_bool("hb_received_pay_rdata_req", hb_received_pay_rdata_req);

            print_bool("hb_send_obc_rdata_resp", hb_send_obc_rdata_resp);
            print_bool("hb_send_eps_rdata_resp", hb_send_eps_rdata_resp);
            print_bool("hb_send_pay_rdata_resp", hb_send_pay_rdata_resp);
        }
    }

    return 0;
}
