/*
    AUTHORS: Shimi Smith, Siddharth Mahendraker

    A queue implementation which does not allocate memory to the heap.
*/

#include <queue/queue.h>

void init_queue(queue_t* queue) {
    queue->size = 0;
    queue->index = 0;
}

uint8_t is_full(queue_t* queue) {
    return (queue->size == MAX_SIZE);
}

uint8_t is_empty(queue_t* queue) {
    return (queue->size == 0);
}

uint8_t enqueue(queue_t* queue, uint8_t* data) {
    if(is_full(queue)) {
        return 1;
    } else {
        uint8_t index = queue->index;
        for (uint8_t i = 0; i < DATA_SIZE; i++) {
            (queue->content)[index][i] = data[i];
        }
        queue->index+= 1;
        queue->size += 1;
        return 0;
    }
}

uint8_t dequeue(queue_t* queue, uint8_t* data) {
    if(is_empty(queue)) {
        return 1;
    } else {
        queue->index -= 1;
        uint8_t index = queue->index;
        for (uint8_t i = 0; i < DATA_SIZE; i++) {
            data[i] = (queue->content)[index][i];
        }
        queue->size -= 1;
        return 0;
    }
}
