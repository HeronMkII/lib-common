#include <assert/assert.h>

// DO NOT USE outside of `assert.h`
bool __assert_print_on_pass = false;

uint32_t assert_num_passed = 0;
uint32_t assert_num_failed = 0;

void assert_print_results() {
    print("--------------\n");
    print("ASSERT RESULTS: %d passed, %d failed\n", assert_num_passed, assert_num_failed);

    if (assert_num_failed > 0) {
        print("NOT WORKING\n");
    } else {
        print("OK\n");
    }
}
