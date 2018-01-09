#include <uart/uart.h>
#include <uart/log.h>
#include <queue/queue.h>

int main(void) {
    init_uart();
    print("UART initialized\n");

    queue_t queue;
    init_queue(queue);

    for (uint8_t i = 0; i < 10; i++) {
        enqueue(queue, (uint8_t*)"Hello!");
    }

    while (!is_empty(queue)) {
        uint8_t data[8] = {0};
        dequeue(queue, data);
        print("%s\n", (char *) data);
    }
}
