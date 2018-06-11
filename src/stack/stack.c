/*
    AUTHORS: Shimi Smith, Siddharth Mahendraker, J. W. Sheridan

    A stack implementation which does not allocate heap memory.
*/

#include <stack/stack.h>

void init_stack(stack_t* stack) {
    stack->size = 0;
    stack->index = 0;
    for (uint8_t i = 0; i < MAX_STACK_SIZE; i++) {
        for (uint8_t j = 0; j < STACK_DATA_SIZE; j++) {
            stack->content[i][j] = 0x00;
        }
    }
}

uint8_t is_full(stack_t* stack) {
    return (stack->size == MAX_STACK_SIZE);
}

uint8_t is_empty(stack_t* stack) {
    return (stack->size == 0);
}

uint8_t push(stack_t* stack, uint8_t* data) {
    if (is_full(stack)) {
        return 1;
    } else {
        uint8_t index = stack->index;
        for (uint8_t i = 0; i < STACK_DATA_SIZE; i++) {
            (stack->content)[index][i] = data[i];
        }
        stack->index+= 1;
        stack->size += 1;
        return 0;
    }
}

uint8_t pop(stack_t* stack, uint8_t* data) {
    if (is_empty(stack)) {
        return 1;
    } else {
        stack->index -= 1;
        uint8_t index = stack->index;
        for (uint8_t i = 0; i < STACK_DATA_SIZE; i++) {
            data[i] = (stack->content)[index][i];
        }
        stack->size -= 1;
        return 0;
    }
}
