#include <uart/uart.h>
#include <queue/queue.h>

void print_queue_data(queue_t* queue){
    print("Size: %u\n", queue->size);
    print("Head: %u\n", queue->head);
    print("Tail: %u\n\n", queue->tail);
}

int main(void) {
    init_uart();
    print("\n\nUART initialized\n");

    queue_t queue;
    init_queue(&queue);
    print("Queue initialized\n");
    print_queue_data(&queue);

    // Enqueue a bit of data
    uint8_t fail = enqueue(&queue, (uint8_t*)"Hello!");
    if (!fail) {
        print("Enqueue successful\n");
        print("Content: %s\n", queue.content[queue.head]);
        print_queue_data(&queue);
    }

    // Dequeue a bit of data
    uint8_t data[QUEUE_DATA_SIZE] = { 0 };
    fail = dequeue(&queue, data);
    if (!fail){
        print("Dequeue successful\n");
        print("Content: %s\n", data);
        print_queue_data(&queue);
    }

    // Test the queue to maximum size
    uint8_t enqueue_data[QUEUE_DATA_SIZE] = { 0 };
    for (uint8_t i = 0; i < MAX_QUEUE_SIZE + 1; i++){
        enqueue_data[0] = i;
        fail = enqueue(&queue, enqueue_data);
        if (!fail){
            print("Enqueue successful\n");
            // Add 1 because an item was enqueued and dequeued before,
            // so the head now starts at 1
            print("Content: %u\n", queue.content[(i + 1) % MAX_QUEUE_SIZE][0]);
            print_queue_data(&queue);
        } else {
            print("Enqueue failed\n");
            print("Content: %u\n", i);
            print_queue_data(&queue);
        }
    }

    // Now dequeue the elements we added
    for (uint8_t i = 0; i < MAX_QUEUE_SIZE + 1; i++){
        uint8_t fail = dequeue(&queue, data);
        if (!fail){
            print("Dequeue successful\n");
            print("Content: %u\n", data[0]);
            print_queue_data(&queue);
        } else {
            print("Dequeue failed\n");
            print_queue_data(&queue);
        }
    }
}
