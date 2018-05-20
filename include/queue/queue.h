/*
    AUTHORS: Siddharth Mahendraker, Shimi Smith
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_SIZE 50
#define DATA_SIZE 8

typedef struct {
	uint8_t size;
    uint8_t index;
    uint8_t content[MAX_SIZE][DATA_SIZE];
} queue_t;

void init_queue(queue_t*);
uint8_t is_full(queue_t*);
uint8_t is_empty(queue_t*);

uint8_t enqueue(queue_t*, uint8_t*);
uint8_t dequeue(queue_t*, uint8_t*);
