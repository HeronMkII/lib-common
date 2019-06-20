/* This file includes 5 tests to verify the functionality of the watchdog timer.
    In interrupt-mode, the while loop breaks when an interrupt is triggered (see ISR in watchdog.c).
    The test is then able to complete, and the time taken to run is verified in the test suite member.
    NOTE: Times are dependent on the voltage supplied to the 32M1 (see data sheet) and are not exact.
          Some variation in the time member is normal.
    Times for the non-interrupt mode are not tested directly due to test harness limitations, but have been
    previously shown to work in other testing environments (i.e. func_tests). Only the register values for
    these modes are verified here */

#include <uart/uart.h>
#include <watchdog/watchdog.h>
#include <test/test.h>

volatile uint8_t test_count = 0;

void test_cb(void) {
    test_count++;
}

void set_callback_test(void) {
    set_wdt_cb(test_cb);
    // Compare function pointers - casting 16-bit address to uint16_t should be OK
    ASSERT_EQ((uint16_t) wdt_cb, (uint16_t) test_cb);
}

void interrupt_test(void) {
    /*Enables interrupt-mode watchdog timer for 1s*/
    WDT_OFF();
    WDT_ENABLE_INTERRUPT(WDTO_1S); //wdt_enable_interrupt(WDTO_1S);
    uint8_t count = test_count;
    while(count == test_count){
      //do nothing (should be changed when WDT goes off)
    }
    //verify that interrupt occurs by checking the count
    ASSERT_EQ(test_count, count + 1);
    ASSERT_EQ(wdt_int_count, count + 1);
}

void disable_test(void) {
    /*Disables watchdog timer and tests register values*/
    WDT_OFF();
    //assert: wdt disabled, interrupts enabled
    ASSERT_EQ(WDTCSR, 0);
    ASSERT_EQ(MCUSR & 0x08, 0x00); //WDRF disabled
}

void set_2s_timeout(void) {
    /*Sets timeout to 2s, tests time to exit loop*/
    WDT_OFF();
    WDT_ENABLE_INTERRUPT(WDTO_2S);
    uint8_t count = test_count;
    while(count == test_count){
      //do nothing (should be changed when WDT goes off)
    }
    ASSERT_EQ(test_count, count + 1);
}

void set_250ms_timeout(void) {
    /*Sets timeout to 250ms, tests time to exit loop*/
    WDT_OFF();
    WDT_ENABLE_INTERRUPT(WDTO_250MS);
    uint8_t count = test_count;
    while(count == test_count){
      //do nothing (should be changed when WDT goes off)
    }
    ASSERT_EQ(test_count, count + 1);
}

void init_test(void){
    /*Does not test board reset directly due to test harness limitations, but verifies
    that register values are correct*/
    WDT_OFF();

    WDT_ENABLE_BOTH(WDTO_1S);
    ASSERT_EQ(WDTCSR, _BV(WDE) | _BV(WDIE) | _BV(WDP2) | _BV(WDP1));
    WDT_OFF();

    WDT_ENABLE_SYS_RESET(WDTO_1S);
    ASSERT_EQ(WDTCSR, _BV(WDE) | _BV(WDP2) | _BV(WDP1));
    WDT_OFF();

    WDT_ENABLE_INTERRUPT(WDTO_1S);
    ASSERT_EQ(WDTCSR, _BV(WDIE) | _BV(WDP2) | _BV(WDP1));
    WDT_OFF();
}

/* Times are not exact, but are determined through testing. Some variance is normal. */
test_t t1 = { .name = "Set callback", .fn = set_callback_test };
test_t t2 = { .name = "Interrupt Mode (1.0s)", .fn = interrupt_test, .time_min = 1.217 };
test_t t3 = { .name = "Disable WDT", .fn = disable_test };
test_t t4 = { .name = "Set 2.0s timeout", .fn = set_2s_timeout, .time_min = 2.32};
test_t t5 = { .name = "Set 0.25s timeout", .fn = set_250ms_timeout, .time_min = 0.373};
test_t t6 = { .name = "Initialization Test", .fn = init_test};

test_t* suite[] = { &t1, &t2, &t3, &t4, &t5, &t6 };

int main(void) {
    //ensure that the watchdog timer is disabled before running tests
    WDT_OFF();
    run_tests(suite, sizeof(suite) / sizeof(suite[0]));
    print("TESTS COMPLETE\n");
    return 0;
}
