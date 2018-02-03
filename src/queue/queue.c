/*
    AUTHORS: J. W. Sheridan, Shimi Smith, Siddharth Mahendraker

    A queue implementation which does not allocate memory to the heap, and is not a stack.
*/

#include <include/queue.h>

void init_queue(queue_t* queue) {
    queue->size = 0;
    queue->index = 0;
    queue->head = 0;
    queue->tail = 0;
}
uint8_t queue_full(queue_t* queue) {
    return (queue->head == (queue->tail + 1) % 50);
}
uint8_t queue_empty(queue_t* queue) {
    return (queue->head == queue->tail);
}
uint8_t enqueue(queue_t* queue, uint8_t* data) {
    if(queue_full(queue)) {
        return 1;
    } else {
        uint8_t index = queue->index;
        for (uint8_t i = 0; i < DATA_SIZE; i++) {
            (queue->content)[index][i] = data[i];
        }
        queue->index += 1;
        queue->size += 1;
        if(!queue->tail == 49){
            queue->tail++;
        } else {
            queue->tail = 0;
        }
        return 0;
    }
}
uint8_t encircle(queue_t* queue, uint8_t* data) {
    if(queue_full(queue)) {
        if(queue->head == 49){
            queue->head = 0;
        } else {
            queue->head += 1;
        }
        return 1;
    } else {
        uint8_t index = queue->index;
        for (uint8_t i = 0; i < DATA_SIZE; i++) {
            (queue->content)[index][i] = data[i];
        }
        queue->index += 1;
        queue->size += 1;
        if(!queue->tail == 49){
            queue->tail++;
        } else {
            queue->tail = 0;
        }
        return 0;
    }
}
uint8_t dequeue(queue_t* queue, uint8_t* data) {
    if(is_empty2(queue)) {
        return 1;
    } else {
        queue->index -= 1;
        uint8_t index = queue->index;
        for (uint8_t i = 0; i < DATA_SIZE; i++) {
            data[i] = (queue->content)[index][i];
        }
        queue->size -= 1;
        queue->tail = (queue->tail + 49)%50;
        return 0;
    }
}