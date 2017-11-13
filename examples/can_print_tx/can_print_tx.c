#include <uart/uart.h>
#include <uart/log.h>
#include <can/can.h>

void tx_callback(uint8_t*, uint8_t*);

tx_mob_t tx_mob = {
    .mob_num = 0,
    .id_tag = { 0x0000 },
    .ctrl = { 0, 0, 0, 0, 0 },
    .tx_data_cb = tx_callback
};

void tx_callback(uint8_t* data, uint8_t* len) {
    *len = 8;
    char str[] = "Hello!";

    for(uint8_t i = 0; i < *len; i++) {
        data[i] = str[i];
    }
}

int main(void) {
    init_uart();
    print("UART initialized\n");

    init_can();
    init_tx_mob(&tx_mob);

    resume_tx_mob(&tx_mob);

    while (1) {
        print("Status: %#02x\n", tx_mob_status(&tx_mob));
        print("Tx error count: %d\n", CANTEC);
    }
}
