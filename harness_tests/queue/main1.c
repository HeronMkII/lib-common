#include <test/test.h>
#include <queue/queue.h>

#if MAX_QUEUE_SIZE != 5
#undef MAX_QUEUE_SIZE
#define MAX_QUEUE_SIZE 5
#endif

queue_t queue;

uint8_t r[] = { 0xf3, 0x25, 0xe3, 0x1d, 0x79, 0xff, 0x00, 0xaa };
uint8_t s[] = { 0xaa, 0xbb, 0xcc, 0x11, 0x22, 0x33, 0x44, 0x55 };
uint8_t t[] = { 0x01, 0xf2, 0x03, 0xb4, 0xc5, 0x06, 0xa7, 0x08 };
uint8_t u[] = { 0xe6, 0x12, 0x89, 0x2b, 0x58, 0x60, 0x00, 0x37 };
uint8_t v[] = { 0x91, 0x29, 0x37, 0x47, 0x56, 0xe6, 0xf2, 0xf7 };

uint8_t* w[] = { r, s, t, u, v };

void init_queue_test() {
    init_queue(&queue);

    // Verify that the queue is initialized correctly
    ASSERT_EQ(queue.head, 0);
    ASSERT_EQ(queue.tail, 0);

    // Verify that the queue is empty
    for (uint8_t i = 0; i < MAX_QUEUE_SIZE; i++) {
        for (uint8_t j = 0; j < QUEUE_DATA_SIZE; j++) {
            ASSERT_EQ(queue.content[i][j], 0);
        }
    }
}

// This should be executed first
void enqueue_simple() {
    uint8_t succ = enqueue(&queue, r);
    ASSERT_TRUE(succ);

    ASSERT_EQ(queue.head, 0);
    ASSERT_EQ(queue.tail, 1);

    for (uint8_t j = 0; j < QUEUE_DATA_SIZE; j++) {
        ASSERT_EQ(queue.content[0][j], r[j]);
    }

    succ = enqueue(&queue, s);
    ASSERT_TRUE(succ);

    ASSERT_EQ(queue.head, 0);
    ASSERT_EQ(queue.tail, 2);

    for (uint8_t j = 0; j < QUEUE_DATA_SIZE; j++) {
        ASSERT_EQ(queue.content[1][j], s[j]);
    }
}

// This should be executed second
void peek_queue_simple() {
    uint8_t data[QUEUE_DATA_SIZE] = { 0 };

    uint8_t succ = peek_queue(&queue, data);
    ASSERT_TRUE(succ);

    ASSERT_EQ(queue.head, 0);
    ASSERT_EQ(queue.tail, 2);

    for (uint8_t j = 0; j < QUEUE_DATA_SIZE; j++) {
        ASSERT_EQ(data[j], r[j]);
    }

    for (uint8_t j = 0; j < QUEUE_DATA_SIZE; j++) {
        ASSERT_EQ(queue.content[0][j], r[j]);
    }
}

// This should be executed third
void dequeue_simple() {
    uint8_t data[QUEUE_DATA_SIZE] = { 0 };

    uint8_t succ = dequeue(&queue, data);
    ASSERT_TRUE(succ);

    ASSERT_EQ(queue.head, 1);
    ASSERT_EQ(queue.tail, 2);

    for (uint8_t j = 0; j < QUEUE_DATA_SIZE; j++) {
        ASSERT_EQ(data[j], r[j]);
    }

    for (uint8_t j = 0; j < QUEUE_DATA_SIZE; j++) {
        ASSERT_EQ(queue.content[0][j], 0);
    }

    succ = dequeue(&queue, data);
    ASSERT_TRUE(succ);

    ASSERT_EQ(queue.head, 2);
    ASSERT_EQ(queue.tail, 2);

    for (uint8_t j = 0; j < QUEUE_DATA_SIZE; j++) {
        ASSERT_EQ(data[j], s[j]);
    }

    for (uint8_t j = 0; j < QUEUE_DATA_SIZE; j++) {
        ASSERT_EQ(queue.content[1][j], 0);
    }

    succ = dequeue(&queue, data);
    ASSERT_FALSE(succ);
}

void is_empty_test() {
    uint8_t empty = queue_empty(&queue);
    ASSERT_TRUE(empty);

    enqueue(&queue, r);
    empty = queue_empty(&queue);
    ASSERT_FALSE(empty);

    dequeue(&queue, NULL); // throw away the dequeued item
    empty = queue_empty(&queue);
    ASSERT_TRUE(empty);
}

void is_full_test() {
    for (uint8_t i = 0; i < 5; i++) {
        enqueue(&queue, w[i]);
    }

    uint8_t succ = enqueue(&queue, r);
    ASSERT_FALSE(succ);

    uint8_t full = queue_full(&queue);
    ASSERT_TRUE(full);
}

void mixed_test() {
    dequeue(&queue, NULL);

    uint8_t empty = queue_empty(&queue);
    ASSERT_FALSE(empty);

    for (uint8_t j = 0; j < QUEUE_DATA_SIZE; j++) {
        ASSERT_EQ(queue.content[0][j], 0);
    }

    uint8_t succ = enqueue(&queue, r);
    ASSERT_TRUE(succ);

    uint8_t full = queue_full(&queue);
    ASSERT_TRUE(full);

    for (uint8_t i = 0; i < MAX_QUEUE_SIZE; i++) {
        for (uint8_t j = 0; j < QUEUE_DATA_SIZE; j++) {
            ASSERT_EQ(queue.content[i][j], w[(i + 1) % MAX_QUEUE_SIZE][j]);
        }
    }

    for (uint8_t i = 0; i < MAX_QUEUE_SIZE; i++) {
        dequeue(&queue, NULL);
    }

    empty = queue_empty(&queue);
    ASSERT_TRUE(empty);

    ASSERT_EQ(queue.head, MAX_QUEUE_SIZE);
    ASSERT_EQ(queue.tail, MAX_QUEUE_SIZE);

    succ = enqueue(&queue, r);
    ASSERT_TRUE(succ);

    ASSERT_EQ(queue.head, 0);
    ASSERT_EQ(queue.tail, 1);
}

test_t t1 = { .name = "init_queue", .fn = init_queue_test };
test_t t2 = { .name = "simple enqueue", .fn = enqueue_simple };
test_t t3 = { .name = "simple peek queue", .fn = peek_queue_simple };
test_t t4 = { .name = "simple dequeue", .fn = dequeue_simple };
test_t t5 = { .name = "queue_empty", .fn = is_empty_test };
test_t t6 = { .name = "queue_full", .fn = is_full_test };
test_t t7 = { .name = "mixed enqueue/dequeue", .fn = mixed_test };

test_t* suite[7] = { &t1, &t2, &t3, &t4, &t5, &t6, &t7 };

int main() {
    run_tests(suite, 7);
    return 0;
}
