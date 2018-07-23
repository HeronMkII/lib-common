#include <test/test.h>
#include <uart/uart.h>
#include <can/can.h>

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <util/delay.h>

void rx_callback_2(const uint8_t*, uint8_t);

mob_t rx_mob_2 = {
    .mob_num = 2,
    .mob_type = RX_MOB,
    .dlc = 1,
    .id_tag = { 0x0000 },
    .id_mask = { 0x0000 },
    .ctrl = default_rx_ctrl,
    .rx_cb = rx_callback_2
};

uint8_t data_s[] = { 0, 1, 2, 3 };
uint8_t data_r[] = { 0, 0, 0, 0 };

void rx_callback_2(const uint8_t* data, uint8_t len) {
    // Note: This assertion runs twice per test suite, whenever board 2 (slave)
    // receives a msg (i.e. when TX with appropriate id_tag is called)
    print("TX received!\n");
    _delay_ms(200);
    print("%s\n", (char *) data);
    for (int i = 0; i < 4; i++) {
        data_r[i] = data[i]; // data[i] transmitted from board 1 (TX)
        ASSERT_EQ(data_r[i], data_s[i]); // data_s (sent) = data_r (received)
    }
    _delay_ms(200);
}

int main(void) {
    init_can();
    init_rx_mob(&rx_mob_2);
    run_slave();
    return 0;
}
