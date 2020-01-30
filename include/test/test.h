#ifndef TEST_H
#define TEST_H

#include <uart/uart.h>
#include <utilities/utilities.h>
#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>

#define ASSERT_EQ(a, b) (print("AS EQ %ld %ld (%s) (%d)\r\n",\
    (int32_t)(a), (int32_t)(b), __FUNCTION__, (int16_t)__LINE__))

#define ASSERT_NEQ(a, b) (print("AS NEQ %ld %ld (%s) (%d)\r\n",\
    (int32_t)(a), (int32_t)(b), __FUNCTION__, (int16_t)__LINE__))

#define ASSERT_GREATER(a, b) (print("AS GT %ld %ld (%s) (%d)\r\n",\
    (int32_t)(a), (int32_t)(b), __FUNCTION__, (int16_t)__LINE__))

#define ASSERT_LESS(a, b) (print("AS LT %ld %ld (%s) (%d)\r\n",\
    (int32_t)(a), (int32_t)(b), __FUNCTION__, (int16_t)__LINE__))

#define ASSERT_TRUE(v) (print("AS TRUE %ld (%s) (%d)\r\n",\
    (int32_t)(v), __FUNCTION__, (int16_t)__LINE__))

#define ASSERT_FALSE(v) (print("AS FALSE %ld (%s) (%d)\r\n",\
    (int32_t)(v), __FUNCTION__, (int16_t)__LINE__))

#define ASSERT_FP_EQ(a, b) (print("AS FP EQ %.3f %.3f (%s) (%d)\r\n",\
    (float)(a), (float)(b), __FUNCTION__, (int16_t)__LINE__))

#define ASSERT_FP_NEQ(a, b) (print("AS FP NEQ %.3f %.3f (%s) (%d)\r\n",\
    (float)(a), (float)(b), __FUNCTION__, (int16_t)__LINE__))

#define ASSERT_FP_GREATER(a, b) (print("AS FP GT %.3f %.3f (%s) (%d)\r\n",\
    (float)(a), (float)(b), __FUNCTION__, (int16_t)__LINE__))

#define ASSERT_FP_LESS(a, b) (print("AS FP LT %.3f %.3f (%s) (%d)\r\n",\
    (float)(a), (float)(b), __FUNCTION__, (int16_t)__LINE__))

#define ASSERT_STR_EQ(a, b) (print("AS STR EQ %s %s (%s) (%d)\r\n",\
    (char*)(a), (char*)(b), __FUNCTION__, (int16_t)__LINE__))

typedef void(*test_fn_t)(void);

typedef struct {
    char name[30];
    test_fn_t fn;
    float time_min;
    float time_max;
} test_t;

void run_tests(test_t**, uint8_t);

void run_slave(void);

extern bool test_enable_time;

#endif // TEST_H
