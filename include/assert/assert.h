#ifndef ASSERT_H
#define ASSERT_H

#include <stdint.h>
#include <stdbool.h>
#include <uart/log.h>

// Assumes `init_uart()` has already been called

// You can add `#define ASSERT_DISABLE` before `#include <assert/assert.h>`
// to remove ASSERT from the executable

// You can set `assert_print_on_pass = true`
// to enable printing a message when an ASSERT passes
// (false by default)

extern bool assert_print_on_pass;


#ifdef ASSERT_DISABLE
#define ASSERT(condition)

#else
#define ASSERT(condition) \
    if (condition) { \
        if (assert_print_on_pass) { \
            print("ASSERT PASSED: %s, Line %d\n", __FILE__, __LINE__); \
        } \
        assert_num_passed++; \
    } \
    else { \
        print("ASSERT FAILED: %s, Line %d\n", __FILE__, __LINE__); \
        assert_num_failed++; \
    }

#endif


// For counting the number of assertions that passed and failed
extern uint32_t assert_num_passed;
extern uint32_t assert_num_failed;

// Prints the number of passed/failed assertions
// (generally used at the end of a test program)
void assert_print_results();

#endif
