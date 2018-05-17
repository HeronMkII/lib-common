#ifndef ASSERT_H
#define ASSERT_H

#include <stdint.h>
#include <stdbool.h>
#include <uart/log.h>

// Assumes `init_uart()` has already been called

// Macros that can optionally be defined before including `assert.h`:
// ASSERT_DISABLE - remove ASSERT from the executable
// ASSERT_PRINT_ON_PASS - print a message when an ASSERT passes

extern bool __assert_print_on_pass;
#ifdef ASSERT_PRINT_ON_PASS
    assert_print_on_pass = true;
#endif



#ifdef ASSERT_DISABLE
#define ASSERT(condition)

#else
#define ASSERT(condition) \
    if (condition) { \
        if (__assert_print_on_pass) { \
            print("ASSERT PASSED: %s, Line %d: %s\n", __FILE__, __LINE__, condition); \
        } \
        assert_num_passed++; \
    } \
    else { \
        print("ASSERT FAILED: %s, Line %d: %s\n", __FILE__, __LINE__, condition); \
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
