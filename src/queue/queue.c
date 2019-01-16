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

    Need to use atomic blocks (which temporarily prevent interrupts) because
    queue operations must complete without interference. For example, say we are
    in the middle of dequeue (copying array contents) when we receive an
    interrupt that adds a new command (calls enqueue). This will modify
    head/tail, making the dequeue command fail because it is referring to a
    different index in the array.

    The compiler seems not to properly recognize return statements inside the
    atomic blocks. It gives the warning
    "control reaches end of non-void function [-Wreturn-type]". To silence this,
    just put `return 0` outside the atomic block where necessary. The code
    should never actually reach there.
*/

#include <queue/queue.h>

/*
Initizing queue with 0x00 for a size of MAX_QUEUE_SIZE

@param queue_t* queue - queue to initialize
*/
void init_queue(queue_t* queue) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        queue->head = 0;
        queue->tail = 0;
        for (uint8_t i = 0; i < MAX_QUEUE_SIZE; i++) {
            for (uint8_t j = 0; j < QUEUE_DATA_SIZE; j++) {
                queue->content[i][j] = 0x00;
            }
        }
    }
}

/*
queue - queue to get the size of
Returns - the size of the queue (number of 8-byte elements)
*/
uint8_t queue_size(queue_t* queue) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        return (queue->tail - queue->head);
    }

    return 0;
}

/*
Checks whether the queue is full

@param queue_t* queue - queue to check
@return 1 if the queue has reached maximum capacity, 0 otherwise
*/
uint8_t queue_full(queue_t* queue) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        return (queue_size(queue) == MAX_QUEUE_SIZE);
    }

    return 0;
}

/*
Checks whether the queue is empty

@param queue_t* queue - queue to check
@return 1 if there are no elements in the queue, 0 otherwise
*/
uint8_t queue_empty(queue_t* queue) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        return (queue_size(queue) == 0);
    }

    return 0;
}

/*
Shifts all the elements in queue left (to start at index 0)

@param queue_t* queue - queue to operate on
*/
void shift_queue_left(queue_t* queue) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        for (uint8_t i = queue->head; i < queue->tail; i++) {
            for (uint8_t j = 0; j < QUEUE_DATA_SIZE; j++) {
                queue->content[i - queue->head][j] = queue->content[i][j];
                queue->content[i][j] = 0x00;
            }
        }

        queue->tail = (queue->tail) - (queue->head);
        queue->head = 0;
    }
}

/*
Inserts new data at the end of the queue

@param queue_t* queue - queue to insert into
@param const uint8_t* data - pointer to 8-byte array to insert (copy) into the queue
@return 1 if data has been added to queue, 0 otherwise
*/
uint8_t enqueue(queue_t* queue, const uint8_t* data) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if (queue_full(queue)) {
            return 0;
        }

        if (queue->tail == MAX_QUEUE_SIZE) {
            shift_queue_left(queue);
        }
        for (uint8_t i = 0; i < QUEUE_DATA_SIZE; i++) {
            queue->content[queue->tail][i] = data[i];
        }
        queue->tail += 1;
        return 1;
    }

    return 0;
}

/*
Peeks (gets) the first element of the queue without removing it

@param queue_t* queue - queue to peek an element from
@param uint8_t* data - pointer to 8-byte array that this function will populate
@return 1 if data is valid from queue, 0 otherwise
*/
uint8_t peek_queue(queue_t* queue, uint8_t* data) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if (queue_empty(queue)) {
            return 0;
        }

        if (data != NULL) {
            for (uint8_t i = 0; i < QUEUE_DATA_SIZE; i++) {
                data[i] = queue->content[queue->head][i];
            }
        }

        return 1;
    }

    return 0;
}

/*
Removes and returns the first element of the queue

@param queue_t* queue - queue to remove an element from
@param uint8_t* data - pointer to 8-byte array that this function will populate
@return 1 if data has been removed from queue, 0 otherwise
*/
uint8_t dequeue(queue_t* queue, uint8_t* data) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if (queue_empty(queue)) {
            return 0;
        }

        // Get the element
        peek_queue(queue, data);

        // Remove the element
        for (uint8_t i = 0; i < QUEUE_DATA_SIZE; i++) {
            queue->content[queue->head][i] = 0x00;
        }
        queue->head += 1;

        return 1;
    }

    return 0;
}
