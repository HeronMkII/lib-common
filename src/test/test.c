#include <test/test.h>
#include <uart/uart.h>

#define F_CPU 8000000UL
#include <util/delay.h>
#include <util/atomic.h>

#define START_LEN 7
#define COUNT_LEN 7

#define COUNT_MSG "COUNT\r\n"
#define END_MSG "END\r\n"
#define START_TEST_MSG "START\r\n"
#define DONE_TEST_MSG "DONE\r\n"

test_t** test_suite;
uint8_t test_num;

volatile uint8_t curr_test = 0;

void run_test(test_t*);

// FIXME: this code is very precarious
uint8_t test_start_cb(const uint8_t* data, uint8_t len) {
    char* start = "START\r\n";

    if (len < START_LEN) return 0;

    uint8_t flag = 1;
    for (uint8_t i = 0; i < START_LEN - 1; i++) {
        if (data[i] != start[i]) {
            flag = 0;
            break;
        }
    }

    if (flag == 1) {
        run_test(test_suite[curr_test]);
        curr_test += 1;
        return START_LEN;
    } else {
        clear_rx_buffer();
        return 0;
    }
}

uint8_t test_count_cb(const uint8_t* data, uint8_t len) {
    char* count = "COUNT\r\n";

    if (len < COUNT_LEN) return 0;

    uint8_t flag = 1;
    for (uint8_t i = 0; i < COUNT_LEN - 1; i++) {
        if (data[i] != count[i]) {
            flag = 0;
            break;
        }
    }

    if (flag == 1) {
        print("%d\r\n", test_num);
        register_callback(test_start_cb);
    }

    clear_rx_buffer();
    return 0;
}

void run_tests(test_t** suite, uint8_t len) {
    init_uart();

    test_suite = suite;
    test_num = len;

    register_callback(test_count_cb);

    __attribute__((unused)) uint8_t curr_cpy;
    // GCC magic to make the compiler shut up
    do {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            curr_cpy = curr_test;
        }
    } while (curr_test != len);

    print("END\r\n");
}

void run_test(test_t* test) {
    print("TEST NAME: %s\r\n", test->name);
    (test->fn)();
    _delay_ms(100);
    print("DONE\r\n");
}

