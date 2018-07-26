#include <test/test.h>
#include <stack/stack.h>

stack_t stack;

uint8_t r[] = { 0xfe, 0x25, 0xe3, 0x1d, 0x73, 0xff, 0x00, 0xa7 };
uint8_t s[] = { 0xaa, 0xbb, 0xac, 0x41, 0xd2, 0x33, 0xe4, 0x55 };
uint8_t t[] = { 0x07, 0xf2, 0x03, 0xb4, 0xc5, 0x06, 0xa2, 0x08 };
uint8_t u[] = { 0xe6, 0xff, 0x89, 0x2b, 0x58, 0x60, 0x00, 0x37 };
uint8_t v[] = { 0x91, 0x29, 0x3f, 0x47, 0x56, 0xe6, 0xf2, 0xf1 };

uint8_t* w[] = { r, s, t, u, v };

void init_stack_test() {
    init_stack(&stack);

    // Verify that the stack is initialized correctly
    ASSERT_EQ(stack.index, 0);

    // Verify that the stack is empty
    for (uint8_t i = 0; i < MAX_STACK_SIZE; i++) {
        for (uint8_t j = 0; j < STACK_DATA_SIZE; j++) {
            ASSERT_EQ(stack.content[i][j], 0);
        }
    }
}

void push_simple() {
    uint8_t fail = push(&stack, r);
    ASSERT_FALSE(fail);

    ASSERT_EQ(stack.index, 1);

    for (uint8_t j = 0; j < STACK_DATA_SIZE; j++) {
        ASSERT_EQ(stack.content[0][j], r[j]);
    }

    fail = push(&stack, s);
    ASSERT_FALSE(fail);

    ASSERT_EQ(stack.index, 2);

    for (uint8_t j = 0; j < STACK_DATA_SIZE; j++) {
        ASSERT_EQ(stack.content[1][j], s[j]);
    }
}

void pop_simple() {
    uint8_t data[STACK_DATA_SIZE] = { 0 };

    uint8_t fail = pop(&stack, data);
    ASSERT_FALSE(fail);

    ASSERT_EQ(stack.index, 1);

    for (uint8_t j = 0; j < STACK_DATA_SIZE; j++) {
        ASSERT_EQ(data[j], s[j]);
    }

    for (uint8_t j = 0; j < STACK_DATA_SIZE; j++) {
        ASSERT_EQ(stack.content[1][j], 0);
    }

    fail = pop(&stack, data);
    ASSERT_FALSE(fail);

    ASSERT_EQ(stack.index, 0);

    for (uint8_t j = 0; j < STACK_DATA_SIZE; j++) {
        ASSERT_EQ(data[j], r[j]);
    }

    for (uint8_t j = 0; j < STACK_DATA_SIZE; j++) {
        ASSERT_EQ(stack.content[0][j], 0);
    }

    fail = pop(&stack, data);
    ASSERT_TRUE(fail);
}

void is_empty_test() {
    uint8_t empty = stack_empty(&stack);
    ASSERT_TRUE(empty);

    push(&stack, r);
    empty = stack_empty(&stack);
    ASSERT_FALSE(empty);

    pop(&stack, NULL); // throw away the popped item
    empty = stack_empty(&stack);
    ASSERT_TRUE(empty);
}

void is_full_test() {
    for (uint8_t i = 0; i < 5; i++) {
        push(&stack, w[i]);
    }

    uint8_t fail = push(&stack, r);
    ASSERT_TRUE(fail);

    uint8_t full = stack_full(&stack);
    ASSERT_TRUE(full);
}

void mixed_test() {
    pop(&stack, NULL);

    uint8_t empty = stack_empty(&stack);
    ASSERT_FALSE(empty);

    for (uint8_t j = 0; j < STACK_DATA_SIZE; j++) {
        ASSERT_EQ(stack.content[MAX_STACK_SIZE - 1][j], 0);
    }

    uint8_t fail = push(&stack, r);
    ASSERT_FALSE(fail);

    uint8_t full = stack_full(&stack);
    ASSERT_TRUE(full);

    for (uint8_t i = 0; i < MAX_STACK_SIZE - 1; i++) {
        for (uint8_t j = 0; j < STACK_DATA_SIZE; j++) {
            ASSERT_EQ(stack.content[i][j], w[i][j]);
        }
    }

    for (uint8_t j = 0; j < STACK_DATA_SIZE; j++) {
        ASSERT_EQ(stack.content[MAX_STACK_SIZE - 1][j], r[j]);
    }

    for (uint8_t i = 0; i < MAX_STACK_SIZE; i++) {
        pop(&stack, NULL);
    }

    empty = stack_empty(&stack);
    ASSERT_TRUE(empty);

    ASSERT_EQ(stack.index, 0);

    fail = push(&stack, r);
    ASSERT_FALSE(fail);

    ASSERT_EQ(stack.index, 1);
}

test_t t1 = { .name = "init_stack", .fn = init_stack_test };
test_t t2 = { .name = "simple push", .fn = push_simple };
test_t t3 = { .name = "simple pop", .fn = pop_simple };
test_t t4 = { .name = "stack_empty", .fn = is_empty_test };
test_t t5 = { .name = "stack_full", .fn = is_full_test };
test_t t6 = { .name = "mixed push/pop", .fn = mixed_test };

test_t* suite[6] = { &t1, &t2, &t3, &t4, &t5, &t6 };

int main() {
    run_tests(suite, 6);
    return 0;
}
