#include <uart/uart.h>
#include <uart/log.h>
#include <queue/queue.h>

int main(void) {
    init_uart();
    print("UART initialized\n");

    queue_t queue;
    init_queue(&queue);

    for (uint8_t i = 0; i < 10; i++) {
        uint8_t fail = enqueue(&queue, (uint8_t*) "Hello!");
        if (!fail) {
            print("Enqueue successful\n");
            print("Size: %d\n", queue.size);
            print("Content: %s\n", queue.content[i]);
        }
    }

    while (!is_empty(&queue)) {
        uint8_t data[8] = {0};
        uint8_t fail = dequeue(&queue, data);
        if (!fail) {
            print("Dequeue successful\n");
            print("Size: %d\n", queue.size);
            print("Content: %s\n", data);
        }
    }
}
