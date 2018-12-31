#ifndef STACK_H
#define STACK_H

#include <stdint.h>
#include <stdlib.h> // for NULL

// Maximum number of elements each stack can store
#define MAX_STACK_SIZE 5
// Number of bytes per element
#define STACK_DATA_SIZE 8

// Stack type
typedef struct {
    // Ending index of stack, points to next index to populate
    uint8_t index;
    // Stack data, static array dimensions
    uint8_t content[MAX_STACK_SIZE][STACK_DATA_SIZE];
} stack_t;
// NOTE: index is always equal to the stack's size

void init_stack(stack_t* stack);
uint8_t stack_full(stack_t* stack);
uint8_t stack_empty(stack_t* stack);
uint8_t push_stack(stack_t* stack, const uint8_t* data);
uint8_t peek_stack(stack_t* stack, uint8_t* data);
uint8_t pop_stack(stack_t* stack, uint8_t* data);

#endif // STACK_H
