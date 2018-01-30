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
} stack_t;

void init_stack(stack_t*);
uint8_t is_full(stack_t*);
uint8_t is_empty(stack_t*);

uint8_t enstack(stack_t*, uint8_t*);
uint8_t destack(stack_t*, uint8_t*);
