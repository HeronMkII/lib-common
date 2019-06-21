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
// -----------------------------------------------------------------------------


void print_bool(char* str, bool val) {
    if (val) {
        print("%s: true\n");
    }
}


int main() {
    init_uart();
    init_uptime();
    init_can();
    print("\n\n\nStarting hearbeat reset test\n");

    if (SELF_ID == HB_OBC) {
        hb_ping_period_s = 30;
    } else if (SELF_ID == HB_EPS) {
        hb_ping_period_s = 40;
    }

    print("Initializing heartbeat...\n");
    init_hb(SELF_ID);
    print("Done init\n");

    print("hb_self_id = ");
    if (hb_self_id == HB_OBC) {
        print("OBC");
    } else if (hb_self_id == HB_EPS) {
        print("EPS");
    } else if (hb_self_id == HB_PAY) {
        print("PAY");
    }
    print("\n");

    print("hb_ping_period_s = %lu\n", hb_ping_period_s);

    print("Starting main loop\n");

    while (1) {
        print_bool("hb_send_obc_ping", hb_send_obc_ping);
        print_bool("hb_send_eps_ping", hb_send_eps_ping);
        print_bool("hb_send_pay_ping", hb_send_pay_ping);
        print_bool("hb_received_obc_resp", hb_received_obc_resp);
        print_bool("hb_received_eps_resp", hb_received_eps_resp);
        print_bool("hb_received_pay_resp", hb_received_pay_resp);       
        print_bool("hb_send_obc_resp", hb_send_obc_resp);
        print_bool("hb_send_eps_resp", hb_send_eps_resp);
        print_bool("hb_send_pay_resp", hb_send_pay_resp);

        run_hb();
    }
    
    return 0;
}
