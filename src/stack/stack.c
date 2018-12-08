/*
    AUTHORS: Shimi Smith, Siddharth Mahendraker, J. W. Sheridan

    A stack implementation which does not allocate heap memory.

    A stack is a data structure that operates under the Last In-First Out
    principle (LIFO). This means that the last element to be put in the stack
    is the first element that is removed from it (think of a stack of coins).
    Elements are put into the stack using the push method, and removed from the
    stack using the pop method (from the top of the stack).
*/

#include <stack/stack.h>

/*
Initalizes the stack with 0x00 for a size of MAX_STACK_SIZE.

@param stack_t* stack - stack to initialize
*/
void init_stack(stack_t* stack) {
    //stack index pointer set to 0
    stack->index = 0;
    //loops through the stack elements and initalizes each element as 0x00.
    for (uint8_t i = 0; i < MAX_STACK_SIZE; i++) {
        for (uint8_t j = 0; j < STACK_DATA_SIZE; j++) {
            stack->content[i][j] = 0x00;
        }
    }
}

/*
Checks whether stack is full

@param stack_t* stack - stack to check
@return 1 if the stack has reached maximum capacity, 0 otherwise
*/
uint8_t stack_full(stack_t* stack) {
    return (stack->index == MAX_STACK_SIZE);
}

/*
Checks whether stack is empty

@param stack_t* stack - stack to check
@return 1 if the stack is empty, 0 otherwise
*/
uint8_t stack_empty(stack_t* stack) {
    return (stack->index == 0);
}

/*
Adds element to stack if possible.

@param stack_t* stack - stack to add the element to
@param const uint8_t* data - 8-byte array with data to add (copy) to the stack
@return 1 if stack is not empty, otherwise 0 if stack is full
*/
uint8_t push(stack_t* stack, const uint8_t* data) {
    // checks if stack is full, and return 0 if true
    if (stack_full(stack)) {
        return 0;
    }
    // adds integer pointed to by data to the stack, and returns 1
    else {
        uint8_t index = stack->index;
        for (uint8_t i = 0; i < STACK_DATA_SIZE; i++) {
            (stack->content)[index][i] = data[i];
        }
        stack->index += 1;
        return 1;
    }
}

/*
Removes element from top of stack if possible, and stores it in data

@param stack_t* stack - stack to remove an element from
@param uint8_t* data - 8-byte array that will be populated by this function with the data
@return 1 if successful, otherwise 0 if stack is empty
*/
uint8_t pop(stack_t* stack, uint8_t* data) {
    //checks if stack is empty and returns 0 if true
    if (stack_empty(stack)) {
        return 0;
    }
    else {
        stack->index -= 1;
        uint8_t index = stack->index;
        for (uint8_t i = 0; i < STACK_DATA_SIZE; i++) {
            if (data != NULL) {
                data[i] = (stack->content)[index][i];
            }
            (stack->content)[index][i] = 0x00;
        }
        return 1;
    }
}
