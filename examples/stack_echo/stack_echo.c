#include <uart/uart.h>
#include <stack/stack.h>

void print_stack_data(stack_t* stack) {
    print("Size: %u\n", stack->size);
}

int main(void) {
    init_uart();
    print("\n\nUART initialized\n");

    stack_t stack;
    init_stack(&stack);
    print("Stack initialized\n");
    print_stack_data(&stack);

    // Push a bit of data
    uint8_t fail = push(&stack, (uint8_t*)"Hello!");
    if (!fail) {
        print("Push successful\n");
        print("Content: %s\n", (char*)stack.content[0]);
        print_stack_data(&stack);
    }

    // Pop a bit of data
    uint8_t data[STACK_DATA_SIZE] = { 0 };
    fail = pop(&stack, data);
    if (!fail) {
        print("Pop successful\n");
        print("Content: %s\n", (char*)data);
        print_stack_data(&stack);
    }

    // Test the stack to maximum size
    uint8_t push_data[STACK_DATA_SIZE] = { 0 };
    for (uint8_t i = 0; i < MAX_STACK_SIZE + 1; i++) {
        push_data[0] = i;
        fail = push(&stack, push_data);
        if (!fail) {
            print("Push successful\n");
            print("Content: %u\n", stack.content[i][0]);
            print_stack_data(&stack);
        } else {
            print("Push failed\n");
            print_stack_data(&stack);
        }
    }

    // Now pop the elements we added
    for (uint8_t i = 0; i < MAX_STACK_SIZE + 1; i++) {
        uint8_t fail = pop(&stack, data);
        if (!fail) {
            print("Pop successful\n");
            print("Content: %u\n", data[0]);
            print_stack_data(&stack);
        } else {
            print("Dequeue failed\n");
            print_stack_data(&stack);
        }
    }
}
