/*
    AUTHORS: J. W. Sheridan, Siddharth Mahendraker, Shimi Smith
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_QUEUE_SIZE 50
#define QUEUE_DATA_SIZE 8

typedef struct {
	uint8_t head;
	uint8_t tail;
 	uint8_t size;
	uint8_t index;
  	uint8_t content[MAX_QUEUE_SIZE][QUEUE_DATA_SIZE];
} queue_t;

void init_queue(queue_t*);
uint8_t queue_full(queue_t*);
uint8_t queue_empty(queue_t*);
uint8_t enqueue(queue_t*, uint8_t*);
uint8_t encircle(queue_t*, uint8_t*);
uint8_t dequeue(queue_t*, uint8_t*);