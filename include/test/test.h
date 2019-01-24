#ifndef TEST_H
#define TEST_H

#include <uart/uart.h>
#include <avr/io.h>
#include <stdint.h>

#define ASSERT_EQ(a, b) (print("AS EQ %d %d (%s) (%d)\r\n",\
    (a), (b), __FUNCTION__, __LINE__))

#define ASSERT_NEQ(a, b) (print("AS NEQ %d %d (%s) (%d)\r\n",\
    (a), (b), __FUNCTION__, __LINE__))

#define ASSERT_GREATER(a, b) (print("AS GT %d %d (%s) (%d)\r\n",\
    (a), (b), __FUNCTION__, __LINE__))

#define ASSERT_LESS(a, b) (print("AS LT %d %d (%s) (%d)\r\n",\
    (a), (b), __FUNCTION__, __LINE__))

#define ASSERT_TRUE(v) (print("AS TRUE %d (%s) (%d)\r\n",\
    (v), __FUNCTION__, __LINE__))

#define ASSERT_FALSE(v) (print("AS FALSE %d (%s) (%d)\r\n",\
    (v), __FUNCTION__, __LINE__))

#define ASSERT_FP_EQ(a, b) (print("AS FP EQ %.3f %.3f (%s) (%d)\r\n",\
    (float)(a), (float)(b), __FUNCTION__, __LINE__))

#define ASSERT_FP_NEQ(a, b) (print("AS FP NEQ %.3f %.3f (%s) (%d)\r\n",\
    (float)(a), (float)(b), __FUNCTION__, __LINE__))

#define ASSERT_FP_GREATER(a, b) (print("AS FP GT %.3f %.3f (%s) (%d)\r\n",\
    (float)(a), (float)(b), __FUNCTION__, __LINE__))

#define ASSERT_FP_LESS(a, b) (print("AS FP LT %.3f %.3f (%s) (%d)\r\n",\
    (float)(a), (float)(b), __FUNCTION__, __LINE__))

typedef void(*test_fn_t)(void);

typedef struct {
    char name[30];
    test_fn_t fn;
    float time;
} test_t;

void run_tests(test_t**, uint8_t);

void run_slave(void);

#endif // TEST_H
