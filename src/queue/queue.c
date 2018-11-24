/*
    AUTHORS: J. W. Sheridan, Shimi Smith, Siddharth Mahendraker

    A queue implementation which does not allocate heap memory.
    
    A queue is a data structure that operates under the First in First Out 
    principle. This means that the first element that is inserted into this structure
    will be the first one to be removed. Elements are inserted using enqueue and removed
    using dequeue.
    
    Use-cases (examples):
    - Data that can be reprsented under the FIFO principle
    - Storing data that is asynchronously transferred.
    
*/

#include <queue/queue.h>
/*
Initizing queue with 0x00 for a size of MAX_QUEUE_SIZE

@param queue_t pointer named queue
*/

void init_queue(queue_t* queue) {
    queue->head = 0;
    queue->tail = 0;
    for (uint8_t i = 0; i < MAX_QUEUE_SIZE; i++) {
        for (uint8_t j = 0; j < QUEUE_DATA_SIZE; j++) {
            queue->content[i][j] = 0x00;
        }
    }
}

/*
Checks whether if queue is full

@param queue_t pointer named queue
@return Returns 1 if the queue has reached maximum capacity,
0 otherwise
*/
uint8_t queue_full(queue_t* queue) {
    return ((queue->tail - queue->head) == MAX_QUEUE_SIZE);
}
/*
Checks whether if the queue is empty

@param queue_t pointer queue
@return Returns 1 if there are no more elements in the queue,
0 otherwise
*/
uint8_t queue_empty(queue_t* queue) {
    return ((queue->tail - queue->head) == 0);
}

/*
Shifts all the elements in queue left

@param queue_t pointer queue
*/
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

/*
Inserts data at the end of the queue

@param queue_t pointer queue
@param uint8_t pointer data
@return Return 1 if data has been added to queue,
0 otherwise
*/
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

/*
Removes the first element of the queue
and stores it in pointer data

@param queue_t pointer queue
@param uint8_t pointer data
@return Returns 0 if queue is empty, 1
otherwise
*/
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
