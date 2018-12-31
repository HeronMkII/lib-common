#ifndef QUEUE_H // checker to prevent doubly including header file
#define QUEUE_H

#include <stdint.h>
#include <stdlib.h> // for NULL

// Maximum number of elements each queue can store
#define MAX_QUEUE_SIZE 5
// Number of bytes per element
#define QUEUE_DATA_SIZE 8

// Queue type
typedef struct {
    // Starting index of queue, points to first index stored
    uint8_t head;
    // Ending index of queue, points to next index to populate
    uint8_t tail;
    // Queue data, static array dimensions
    uint8_t content[MAX_QUEUE_SIZE][QUEUE_DATA_SIZE];
} queue_t;
// NOTE: tail - head is always equal to the queue's size

void init_queue(queue_t* queue);
uint8_t queue_full(queue_t* queue);
uint8_t queue_empty(queue_t* queue);
void shift_queue_left(queue_t* queue);
uint8_t enqueue(queue_t* queue, const uint8_t* data);
uint8_t peek_queue(queue_t* queue, uint8_t* data);
uint8_t dequeue(queue_t* queue, uint8_t* data);

#endif // QUEUE_H
