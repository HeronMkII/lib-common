#include <uart/uart.h>
#include <uart/log.h>
#include <can/can.h>

#define F_CPU 8000000UL
#include <util/delay.h>

void tx_callback_1(uint8_t*, uint8_t*);
void tx_callback_2(uint8_t*, uint8_t*);

// this is a tx mob used for testing
mob_t tx_mob_1 = {
    .mob_num = 0,
	.mob_type = TX_MOB,
    .id_tag = { 0x0001 },
    .ctrl = default_tx_ctrl,
    .tx_data_cb = tx_callback_1,
};

mob_t tx_mob_2 = {
    .mob_num = 1,
	.mob_type = TX_MOB,
    .id_tag = { 0x0002 },
    .ctrl = default_tx_ctrl,
    .tx_data_cb = tx_callback_2,
};

int main (void) {
	init_uart();
	print("UART Initialized\n");

    init_can();

	init_tx_mob(&tx_mob_1);
    init_tx_mob(&tx_mob_2);

    while (1) {
        resume_mob(&tx_mob_1);
        while (!is_paused(&tx_mob_1)) {};
        _delay_ms(100);

        resume_mob(&tx_mob_2);
        while (!is_paused(&tx_mob_2)) {};
        _delay_ms(100);
    };
}

void tx_callback_1(uint8_t* data, uint8_t* len) {
    static uint32_t val_1 = 0;
    if (val_1 % 2 == 0) {
        data[0] = 0x41; // A
        *len = 1;
    } else {
        *len = 0;
    }

    val_1 += 1;
}

void tx_callback_2(uint8_t* data, uint8_t* len) {
    static uint32_t val_2 = 0;
    if (val_2 % 2 == 0) {
        data[0] = 0x42; // B
        *len = 1;
    } else {
        *len = 0;
    }

    val_2 += 1;
}
