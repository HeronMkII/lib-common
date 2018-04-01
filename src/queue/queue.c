/*
    AUTHORS: J. W. Sheridan, Shimi Smith, Siddharth Mahendraker

    A queue implementation which does not allocate heap memory.
*/

#include <queue/queue.h>

void init_queue(queue_t* queue) {
    queue->size = 0;
    queue->head = 0;
    queue->tail = 0;
    for (uint8_t i = 0; i < MAX_QUEUE_SIZE; i++){
        for (uint8_t j = 0; j < QUEUE_DATA_SIZE; j++){
            queue->content[i][j] = 0x00;
        }
    }
}

uint8_t is_full(queue_t* queue) {
    return (queue->size == MAX_QUEUE_SIZE);
}

uint8_t is_empty(queue_t* queue) {
    return (queue->size == 0);
}

uint8_t enqueue(queue_t* queue, uint8_t* data) {
    if (is_full(queue)) {
        return 1;
    } else {
        uint8_t index = queue->tail;
        for (uint8_t i = 0; i < QUEUE_DATA_SIZE; i++) {
            (queue->content)[index][i] = data[i];
        }
        queue->size += 1;
        if (!(queue->tail == MAX_QUEUE_SIZE-1)) {
            queue->tail++;
        } else {
            queue->tail = 0;
        }
        return 0;
    }
}

uint8_t dequeue(queue_t* queue, uint8_t* data) {
    if (is_empty(queue)) {
        return 1;
    } else {
        uint8_t index = queue->head;
        for (uint8_t i = 0; i < QUEUE_DATA_SIZE; i++) {
            data[i] = (queue->content)[index][i];
        }
        queue->size -= 1;
        if (!(queue->head == MAX_QUEUE_SIZE-1)) {
            queue->head += 1;
        } else {
            queue->head = 0;
        }
        return 0;
    }
}
