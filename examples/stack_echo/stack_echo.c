#include <uart/uart.h>
#include <uart/log.h>
#include <stack/stack.h>

void print_stack_data(stack_t* stack){
    print("Size: %d\n", stack->size);
    print("Index: %d\n", stack->index);
}

int main(void) {
    init_uart();
    print("UART initialized\n");

    stack_t stack;
    init_stack(&stack);
    print("Stack Initialized\n");
    print_stack_data(&stack);

    // Enstack a bit of data
    uint8_t fail = enstack(&stack, (uint8_t*)"Hello!");
    if (!fail){
        print("Enstack successful\n");
        print("Content: %s\n", stack.content[stack.index]);
        print_stack_data(&stack);
    }

    // Destack a bit of data
    uint8_t data[STACK_DATA_SIZE] = {0};
    fail = destack(&stack, data);
    if (!fail){
        print("Destack successful\n");
        print("Content: %s\n", data);
        print_stack_data(&stack);
    }

    // Test the stack to maximum size
    uint8_t enstack_data[STACK_DATA_SIZE] = {0};
    for (uint8_t i = 0; i < MAX_STACK_SIZE + 1; i++){
        enstack_data[0] = i;
        fail = enstack(&stack, enstack_data);
        if (!fail){
            print("Enstack successful\n");
            print("Content: %d\n", i);
            print_stack_data(&stack);
        } else {
            print("Enstack failed\n");
            print("Content: %d\n", i);
            print_stack_data(&stack);
        }
    }

    // Now destack the elements we added
    for (uint8_t i = 0; i < MAX_STACK_SIZE + 1; i++){
        uint8_t fail = destack(&stack, data);
        if (!fail){
            print("Destack successful\n");
            print("Content: %d\n", data[0]);
            print_stack_data(&stack);
        } else {
            print("Destack failed\n");
            print_stack_data(&stack);
        }
    }
}
