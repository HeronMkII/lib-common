/*
    AUTHORS: J. W. Sheridan, Shimi Smith, Siddharth Mahendraker

    A queue implementation which does not allocate heap memory.
*/

#include <queue/queue.h>

void init_queue(queue_t* queue) {
    queue->head = 0;
    queue->tail = 0;
    for (uint8_t i = 0; i < MAX_QUEUE_SIZE; i++) {
        for (uint8_t j = 0; j < QUEUE_DATA_SIZE; j++) {
            queue->content[i][j] = 0x00;
        }
    }
}

uint8_t queue_full(queue_t* queue) {
    return ((queue->tail - queue->head) == MAX_QUEUE_SIZE);
}

uint8_t queue_empty(queue_t* queue) {
    return ((queue->tail - queue->head) == 0);
}

void shift_left(queue_t* queue) {
    for (uint8_t i = queue->head; i < queue->tail; i++) {
        for (uint8_t j = 0; j < QUEUE_DATA_SIZE; j++) {
            queue->content[i - (queue->head)][j] = queue->content[i][j];
            queue->content[i][j] = 0x00;
        }
    }

    queue->tail = (queue->tail) - (queue->head);
    queue->head = 0;
}

uint8_t enqueue(queue_t* queue, const uint8_t* data) {
    if (queue_full(queue)) {
        return 0;
    } else {
        if (queue->tail == MAX_QUEUE_SIZE) {
            shift_left(queue);
        }
        uint8_t index = queue->tail;
        for (uint8_t i = 0; i < QUEUE_DATA_SIZE; i++) {
            (queue->content)[index][i] = data[i];
        }
        queue->tail += 1;
        return 1;
    }
}

uint8_t dequeue(queue_t* queue, uint8_t* data) {
    if (queue_empty(queue)) {
        return 0;
    } else {
        uint8_t index = queue->head;
        for (uint8_t i = 0; i < QUEUE_DATA_SIZE; i++) {
            if (data != NULL) {
                data[i] = (queue->content)[index][i];
            }
            (queue->content)[index][i] = 0x00;
        }
        queue->head += 1;
        return 1;
    }
}
