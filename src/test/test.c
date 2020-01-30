#include <test/test.h>
#include <uart/uart.h>

#include <util/atomic.h>
#include <stdbool.h>
#include <stdlib.h>

#define COUNT_MSG "COUNT\r\n"
#define COUNT_LEN 7
#define START_MSG "START\r\n"
#define START_LEN 7
#define SEED_MSG "SEED \r\n" /* Expects "SEED" plus 4 additional digits */
#define SEED_LEN 11
#define KILL_MSG "KILL\r\n"
#define KILL_LEN 6

#define END_MSG "END\r\n"

volatile uint8_t count_cb_flag = 0;
volatile uint8_t start_cb_flag = 0;
volatile uint8_t seed_cb_flag = 0;
volatile uint16_t seed_cb_seed = 0; /* Need values between at least 0 and 10000 */
volatile uint8_t kill_cb_flag = 0;

bool test_enable_time = false;

void run_test(test_t*);

// TODO - refactor UART RX

uint8_t test_count_cb(const uint8_t* data, uint8_t len){
    char* count = COUNT_MSG;
    if (len < COUNT_LEN) return 0;

    count_cb_flag = 1;
    for (uint8_t i = 0; i < COUNT_LEN - 1; i++) {
        if (data[i] != count[i]) {
            count_cb_flag = 0;
            break;
        }
    }

    return COUNT_LEN;
}

uint8_t test_start_cb(const uint8_t* data, uint8_t len){
    char* start = START_MSG;
    if (len < START_LEN) return 0;

    /* Test to see if start message is here */
    start_cb_flag = 1;
    for (uint8_t i = 0; i < START_LEN - 1; i++) {
        /* Break if start msg not as expected*/
        if (data[i] != start[i]) {
            start_cb_flag = 0;
            break;
        }
    }

    /* If start flag is detected, run current test */
    return START_LEN;
}

uint8_t test_seed_cb(const uint8_t* data, uint8_t len) {
    char* seed = SEED_MSG;
    if (len < SEED_LEN) return 0;

    seed_cb_flag = 1;
    for (uint8_t i = 0; i < 4; i++) {
        if (data[i] != seed[i]) {
            seed_cb_flag = 0;
            break;
        }
    }
    
    if (seed_cb_flag) {
        // Parse seed integer from string and store it (if we detected the right prefix)
        seed_cb_seed = atoi((const char *)(&data[5]));
    }
    
    return SEED_LEN;
}

void run_tests(test_t** suite, uint8_t len) {
    init_uart();

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        clear_uart_rx_buf();
        count_cb_flag = 0;
        set_uart_rx_cb(test_count_cb);
    }
    // If the laptop does not send "COUNT\r\n" for 5 seconds, just continue
    for (uint16_t i = 0; i < 500 && !count_cb_flag; i++) {
        _delay_ms(10);
    }
    print("%u\r\n", len);

    for (int i = 0; i < len; i++){
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
            clear_uart_rx_buf();
            start_cb_flag = 0;
            set_uart_rx_cb(test_start_cb);
        }
        // Wait up to 100 ms
        for (uint16_t i = 0; i < 10 && !start_cb_flag; i++) {
            _delay_ms(10);
        }

        ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
            clear_uart_rx_buf();
            seed_cb_flag = 0;
            set_uart_rx_cb(test_seed_cb);
        }
        // Wait up to 100 ms
        for (uint16_t i = 0; i < 10 && !seed_cb_flag; i++) {
            _delay_ms(10);
        }
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
            // Seed both 16-bit and 32-bit randoms just in case
            srand(seed_cb_seed);
            srandom(seed_cb_seed);
            print("Random seed is %u\r\n", seed_cb_seed);
        }

        run_test(suite[i]);
    }

    /* This is unnecessary for the harness, but might be useful when
       running a test by hand over UART */
    print("DONE SUITE\r\n");
}

void run_test(test_t* test) {
    print("TEST NAME %s\r\n", test->name);
    if (test_enable_time == true) print("TIME MIN %f MAX %f\r\n", test->time_min, test->time_max);
    /* Calls function in test */
    (test->fn)();
    print("DONE TEST\r\n");
}


uint8_t slave_kill_cb(const uint8_t* data, uint8_t len) {
    char* kill = KILL_MSG;
    if (len < KILL_LEN) return 0;

    kill_cb_flag = 1;
    for (uint8_t i = 0; i < KILL_LEN - 1; i++) {
        if (data[i] != kill[i]) {
            kill_cb_flag = 0;
            break;
        }
    }

    return KILL_LEN;
}


void run_slave() {
    init_uart();

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        clear_uart_rx_buf();
        kill_cb_flag = 0;
        set_uart_rx_cb(slave_kill_cb);
    }
    while(!kill_cb_flag);
    print("DONE SUITE\r\n");
}
