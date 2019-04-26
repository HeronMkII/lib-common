/*
This program tests initialization of the restart count (read EEPROM, increment,
write EEPROM) and continuous updating of the OBC uptime.
*/

#include "../../src/uptime.h"

uint8_t counts[6] = {0};

void print_counts(void) {
    print("uptime: %lu\n", uptime_s);
    print("counts:");
    for (uint8_t i = 0; i < 6; i++) {
        print(" %u", counts[i]);
    }
    print("\n");
}

void func0(void) {
    counts[0] += 1;
}

void func1(void) {
    counts[1] += 1;
}

void func2(void) {
    counts[2] += 1;
}

void func3(void) {
    counts[3] += 1;
}

void func4(void) {
    counts[4] += 1;
}

void func5(void) {
    counts[5] += 1;
}

void wait(void) {
    print_counts();
    print("Waiting 5 seconds...\n");
    _delay_ms(5000);
    print("Done waiting\n");
    print_counts();
}

int main(void) {
    init_uart();

    print("\n\n\nStarting uptime test\n\n");
    rtc_date_t date = { .yy = 03, .mm = 11, .dd = 21 };
    rtc_time_t time = { .hh = 07, .mm = 03, .ss = 59 };
    init_uptime(date, time);
    print("Initialized uptime\n");

    print("restart_count = %lu\n", restart_count);
    print("restart_date = %u:%u:%u\n", restart_date.yy, restart_date.mm, restart_date.dd);
    print("restart_time = %u:%u:%u\n", restart_time.hh, restart_time.mm, restart_time.ss);
    print("uptime_s = %lu\n", uptime_s);

    wait();

    add_uptime_callback(func0);
    print("Added func0\n");

    wait();

    add_uptime_callback(func1);
    add_uptime_callback(func2);
    add_uptime_callback(func3);
    print("Added func1, func2, func3\n");

    wait();

    add_uptime_callback(func4);
    print("Added func4\n");

    wait();

    add_uptime_callback(func5);
    print("Added func5 - should not be called\n");

    wait();

    print("Done test\n");
}
