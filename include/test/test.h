#ifndef TEST_H
#define TEST_H

#include <uart/uart.h>
#include <avr/io.h>
#include <stdint.h>

#define ASSERT_EQ(a, b) (print("ASSERT EQ %d %d (%s) (%d)\r\n",\
    (a), (b), __FUNCTION__, __LINE__))

#define ASSERT_NEQ(a, b) (print("ASSERT NEQ %d %d (%s) (%d)\r\n",\
    (a), (b), __FUNCTION__, __LINE__))

#define ASSERT_GREATER(a, b) (print("ASSERT GREATER %d %d (%s) (%d)\r\n",\
    (a), (b), __FUNCTION__, __LINE__))

#define ASSERT_LESS(a, b) (print("ASSERT LESS %d %d (%s) (%d)\r\n",\
    (a), (b), __FUNCTION__, __LINE__))

#define ASSERT_TRUE(v) (print("ASSERT TRUE %d (%s) (%d)\r\n",\
    (v), __FUNCTION__, __LINE__))

#define ASSERT_FALSE(v) (print("ASSERT FALSE %d (%s) (%d)\r\n",\
    (v), __FUNCTION__, __LINE__))

typedef void(*test_fn_t)(void);

typedef struct {
    char name[30];
    test_fn_t fn;
    float time;
} test_t;

void run_tests(test_t**, uint8_t);

void run_slave(void);

#endif // TEST_H
