/*
This program tests all reasons for resets/restarting the program, and that
the correct diagnostic information is written to EEPROM.
*/

#include <uptime/uptime.h>

void print_reason_string(void) {
    switch (restart_reason) {
        case UPTIME_RESTART_REASON_WDT_TIMEOUT:
            print("WDT timeout");
            break;
        case UPTIME_RESTART_REASON_RESET_CMD:
            print("Reset cmd");
            break;
        case UPTIME_RESTART_REASON_NO_CMD:
            print("No cmd");
            break;
        case UPTIME_RESTART_REASON_UNKNOWN:
            print("Unknown");
            break;
        default:
            print("INVALID");
            break;
    }
}

void print_reason(void) {
    print("Restart reason: 0x%x (", restart_reason);
    print_reason_string();
    print(")\n");
}

void print_cmds(void) {
    print("0. Print cause of last reset\n");
    print("1. Restart (watchdog timeout)\n");
    print("2. Restart (reset command)\n");
    print("3. Restart (no command)\n");
    print("Press reset button: Restart (unknown)\n");
}

uint8_t uart_cb(const uint8_t* data, uint8_t len) {
    switch (data[0]) {
        case 'h':
            print_cmds();
            break;
        case '0':
            print_reason();
            break;
        case '1':
            print("Letting watchdog time out (1 second)...\n");
            WDT_ENABLE_BOTH(WDTO_1S);
            _delay_ms(2000);
            break;
        case '2':
            print("Resetting from restart command...\n");
            reset_self_mcu(UPTIME_RESTART_REASON_RESET_CMD);
            break;
        case '3':
            print("Resetting from no command...\n");
            reset_self_mcu(UPTIME_RESTART_REASON_NO_CMD);
            break;
        default:
            print("Invalid command\n");
            break;
    }

    return 1;
}

int main(void) {
    init_uart();
    
    print("\n\n\nStarting uptime test\n\n");

    init_uptime();
    print("Initialized uptime\n");

    print("restart_count = %lu\n", restart_count);
    print("uptime_s = %lu\n", uptime_s);
    print("restart_reason = %lu\n", restart_reason);
    print_reason();

    set_uart_rx_cb(uart_cb);
    print_cmds();

    while (1) {}
}
