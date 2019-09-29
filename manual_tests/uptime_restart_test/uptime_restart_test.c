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
        case UPTIME_RESTART_REASON_CMD_TIMER:
            print("Cmd timer");
            break;
        case UPTIME_RESTART_REASON_WDRF:
            print("Watchdog System Reset");
            break;
        case UPTIME_RESTART_REASON_BORF:
            print("Brown-out Reset");
            break;
        case UPTIME_RESTART_REASON_EXTRF:
            print("External Reset");
            break;
        case UPTIME_RESTART_REASON_PORF:
            print("Power-on Reset");
            break;
        case UPTIME_RESTART_REASON_UNKNOWN:
            print("Unknown");
            break;
        default:
            print("INVALID");
            break;
    }
}

void print_info(void) {
    print("Restart reason: 0x%lx (", restart_reason);
    print_reason_string();
    print(")\n");

    print("restart_count = %lu\n", restart_count);
    print("restart_reason = %lu\n", restart_reason);
    print("uptime_s = %lu\n", uptime_s);

    print("cmd_timer_count_s = %lu\n", cmd_timer_count_s);
    print("cmd_timer_period_s = %lu\n", cmd_timer_period_s);
}

void print_cmds(void) {
    print("0. Print debug info\n");
    print("1. Reset (watchdog timeout)\n");
    print("2. Reset (reset command)\n");
    print("3. Restart command timer count\n");
    print("Press reset button: Reset (external)\n");
}

uint8_t uart_cb(const uint8_t* data, uint8_t len) {
    switch (data[0]) {
        case 'h':
            print_cmds();
            break;
        case '0':
            print_info();
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
            print("cmd_timer_count_s = %lu\n", cmd_timer_count_s);
            print("Restarting command timer count...\n");
            restart_cmd_timer();
            print("cmd_timer_count_s = %lu\n", cmd_timer_count_s);
            break;
        default:
            print("Invalid command\n");
            break;
    }

    return 1;
}

int main(void) {
    // Turn off watchdog
    // If we just restarted because the WDT timed out, it will immediately
    // activate the reset again if we don't turn it off
    WDT_OFF();

    init_uart();
    print("\n\n\nStarting uptime test\n\n");

    // Can change this as needed to test the command timer feature
    cmd_timer_period_s = 15;
    
    init_uptime();
    print("Initialized uptime\n");
    init_cmd_timer();
    print("Initialized command timer\n");

    print_info();
    
    set_uart_rx_cb(uart_cb);
    print_cmds();

    while (1) {}
}
