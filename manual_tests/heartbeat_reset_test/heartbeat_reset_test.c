/*
Just test the ability for each MCU to trigger the resets on the other two MCUs.
*/

#include <heartbeat/heartbeat.h>
#include <uart/uart.h>

// NOTE: Change this variable before re-compiling and re-uploading to match the
// subsystem of the board you are uploading to
uint8_t id = HB_OBC;

void print_cmds(void) {
    switch (id) {
        case HB_OBC:
            print("1. Reset EPS\n");
            print("2. Reset PAY\n");
            break;
        case HB_EPS:
            print("1. Reset OBC\n");
            print("2. Reset PAY\n");
            break;
        case HB_PAY:
            print("1. Reset OBC\n");
            print("2. Reset EPS\n");
            break;
        default:
            break;
    }
}

uint8_t uart_cb(const uint8_t* data, uint8_t len) {
    uint8_t c = data[0];

    if (c == 'h') {
        print_cmds();
        return 1;
    }

    else if (c == '1') {
        switch (id) {
            case HB_OBC:
                print("Resetting EPS\n");
                send_hb_reset(HB_EPS);
                break;
            case HB_EPS:
                print("Resetting OBC\n");
                send_hb_reset(HB_OBC);
                break;
            case HB_PAY:
                print("Resetting OBC\n");
                send_hb_reset(HB_OBC);
                break;
            default:
                break;
        }
    }

    else if (c == '2') {
        switch (id) {
            case HB_OBC:
                print("Resetting PAY\n");
                send_hb_reset(HB_PAY);
                break;
            case HB_EPS:
                print("Resetting PAY\n");
                send_hb_reset(HB_PAY);
                break;
            case HB_PAY:
                print("Resetting EPS\n");
                send_hb_reset(HB_EPS);
                break;
            default:
                break;
        }
    }

    else {
        print("Invalid command\n");
    }

    return 1;
}

int main() {
    init_uart();
    print("\n\n\nStarting hearbeat reset test\n");
    set_uart_rx_cb(uart_cb);
    print("Set UART RX CB\n");

    print("Self ID = ");
    if (id == HB_OBC) {
        print("OBC");
    } else if (id == HB_EPS) {
        print("EPS");
    } else if (id == HB_PAY) {
        print("PAY");
    }
    print("\n");

    print("Initializing heartbeat...\n");
    init_hb(id);
    print("Done init\n");

    print_cmds();
    print("Press 'h' at any time to list the commands\n");
    while (1) {}

    return 0;
}
