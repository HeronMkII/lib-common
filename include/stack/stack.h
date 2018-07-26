#ifndef STACK_H
#define STACK_H

/*
    AUTHORS: Siddharth Mahendraker, Shimi Smith, J. W. Sheridan
*/

#include <stdint.h>
#include <stdlib.h> // for NULL

#define MAX_STACK_SIZE 5
#define STACK_DATA_SIZE 8

typedef struct {
    uint8_t index;
    uint8_t content[MAX_STACK_SIZE][STACK_DATA_SIZE];
} stack_t;
// NOTE: index is always equal to the stack's size

void init_stack(stack_t*);
uint8_t stack_full(stack_t*);
uint8_t stack_empty(stack_t*);
uint8_t push(stack_t*, const uint8_t*);
uint8_t pop(stack_t*, uint8_t*);

#endif // STACK_H
