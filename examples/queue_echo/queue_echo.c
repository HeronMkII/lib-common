#include <uart/uart.h>
#include <uart/log.h>
#include <queue/queue.h>

void print_queue_data(queue_t* queue){
    print("Size: %d\n", queue->size);
    print("Head: %d\n", queue->head);
    print("Tail: %d\n\n", queue->tail);
}

int main(void) {
    init_uart();
    print("UART initialized\n");

    queue_t queue;
    init_queue(&queue);
    print("Queue Initialized\n");
    print_queue_data(&queue);

    // Enqueue a bit of data
    uint8_t fail = enqueue(&queue, (uint8_t*)"Hello!");
    if (!fail){
        print("Enqueue successful\n");
        print("Content: %s\n", queue.content[queue.head]);
        print_queue_data(&queue);
    }

    // Dequeue a bit of data
    uint8_t data[QUEUE_DATA_SIZE] = {0};
    fail = dequeue(&queue, data);
    if (!fail){
        print("Dequeue successful\n");
        print("Content: %s\n", data);
        print_queue_data(&queue);
    }

    // Test the queue to maximum size
    uint8_t enqueue_data[QUEUE_DATA_SIZE] = {0};
    for (uint8_t i = 0; i < MAX_QUEUE_SIZE + 1; i++){
        enqueue_data[0] = i;
        fail = enqueue(&queue, enqueue_data);
        if (!fail){
            print("Enqueue successful\n");
            print("Content: %d\n", i);
            print_queue_data(&queue);
        } else {
            print("Enqueue failed\n");
            print("Content: %d\n", i);
            print_queue_data(&queue);
        }
    }

    // Now dequeue the elements we added
    for (uint8_t i = 0; i < MAX_QUEUE_SIZE + 1; i++){
        uint8_t fail = dequeue(&queue, data);
        if (!fail){
            print("Dequeue successful\n");
            print("Content: %d\n", data[0]);
            print_queue_data(&queue);
        } else {
            print("Dequeue failed\n");
            print_queue_data(&queue);
        }
    }
}
