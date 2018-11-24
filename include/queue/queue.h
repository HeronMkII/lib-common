#ifndef QUEUE_H // (checker to prevent doubly including header file)
#define QUEUE_H

/*
    AUTHORS: J. W. Sheridan, Siddharth Mahendraker, Shimi Smith
*/

#include <stdint.h>
#include <stdlib.h> // for NULL

#define MAX_QUEUE_SIZE 5
#define QUEUE_DATA_SIZE 8

typedef struct {
    uint8_t head; // starting address of queue
    uint8_t tail; // end of queue
    uint8_t content[MAX_QUEUE_SIZE][QUEUE_DATA_SIZE]; // static array dimensions
} queue_t;
// NOTE: tail - head is always equal to the queue's size

void init_queue(queue_t*);
uint8_t queue_full(queue_t*);
uint8_t queue_empty(queue_t*);
uint8_t enqueue(queue_t*, const uint8_t*);
uint8_t dequeue(queue_t*, uint8_t*);

#endif // QUEUE_H
