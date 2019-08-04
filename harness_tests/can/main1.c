#include <uart/uart.h>
#include <can/can.h>
#include <test/test.h>

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <util/delay.h>

// Function headers necessary for compilation
void tx_callback(uint8_t*, uint8_t*);
void rx_callback(const uint8_t*, uint8_t);
void handle_rx_interrupt(mob_t*);
void handle_tx_interrupt(mob_t*);

mob_t tx_mob = {
    .mob_num = 0,
    .mob_type = TX_MOB,
    .id_tag = { 0x0000 },
    .ctrl = default_tx_ctrl,
    .tx_data_cb = tx_callback,
};


mob_t rx_mob = {
    .mob_num = 1,
    .mob_type = RX_MOB,
    .dlc = 4,
    .id_tag = { 0x0000 },
    .id_mask = { 0x0000 },
    .ctrl = default_rx_ctrl,
    .rx_cb = rx_callback
};

uint8_t data_s[] = { 0, 1, 2, 3 };
uint8_t data_r[] = { 0, 0, 0, 0 };

void rx_callback(const uint8_t* data, uint8_t len) {
    print("TX received!\n");
    _delay_ms(200);
}

void tx_callback(uint8_t* data, uint8_t* len) {
    *len = 4;
    print("data_s: ");
    for (int i = 0; i < 4; i++) {
        data[i] = data_s[i];
        print("%3d",data_s[i]);
    }
    print("\nTX sent\n");
}

void init_can_test(void) {
    init_can();

    // Verify that registers are initialized correctly
    ASSERT_EQ(CANGCON & 0x03, 0x02);
    ASSERT_EQ(CANBT1, 0x08);
    ASSERT_EQ(CANBT2, 0x0C);
    ASSERT_EQ(CANBT3, 0x37);
    ASSERT_EQ(CANGIE, 0xB8);

    // Verify that MObs are reset during init
    for (int i = 0; i < 3; i++){
        select_mob(i);
        ASSERT_EQ(CANCDMOB, 0x00);
        ASSERT_EQ(CANSTMOB, 0x00);
    }

    // Verify that the CAN controller is enabled
    ASSERT_EQ(CANGSTA & _BV(ENFG), _BV(ENFG));
}

void init_tx_test(void) {
    // Initialiazes transmit (TX) MOb, verifies that register values and tags
    // are set as expected
    init_tx_mob(&tx_mob);
    ASSERT_EQ(CANGIE & 0x10, 0x10);
    ASSERT_EQ(CANIE2 & _BV(tx_mob.mob_num), _BV(tx_mob.mob_num));
    // Check that the correct interrupts are enabled
    ASSERT_EQ(CANIDT2, tx_mob.id_tag.tab[0] << 5);
    ASSERT_EQ(CANIDT1, (tx_mob.id_tag.tab[1] << 5) | (tx_mob.id_tag.tab[0] >> 3));
    // Verify that the id_tag is set correctly
}

void init_rx_test(void) {
    // Initializes receiving (RX) MOb, verifies that tags, masks, and register
    // values are set as expected
    init_rx_mob(&rx_mob);
    ASSERT_EQ(CANGIE & 0x20, 0x20); // ENRX enabled
    ASSERT_EQ(CANIE2 & _BV(rx_mob.mob_num), _BV(rx_mob.mob_num));
    // Check that the correct interrupts are enabled
    ASSERT_EQ(CANIDT2, rx_mob.id_tag.tab[0] << 5);
    ASSERT_EQ(CANIDT1,
            (rx_mob.id_tag.tab[1] << 5) | (rx_mob.id_tag.tab[0] >> 3));
    ASSERT_EQ(CANIDM2, rx_mob.id_mask.tab[0] << 5);
    ASSERT_EQ(CANIDM1,
            (rx_mob.id_mask.tab[1] << 5) | (rx_mob.id_mask.tab[0] >> 3));
    // Verify that the id_tag and id_mask are set correctly
}

void pause_resume_test(void) {
    // Verifies that the correct registers are modified when MObs are paused
    // and resumed

    // At this point in the testing, both the TX and RX MObs have been
    // initialized; the RX MOb is resumed by default, the TX MOb must be
    // resumed manually
    resume_mob(&tx_mob);

    // CANPAGE register testing
    for (int i = 0; i < 2; i++){
        select_mob(i);
        ASSERT_EQ(CANPAGE, i << 4);
    }

    // Verify pausing
    pause_mob(&tx_mob);
    ASSERT_EQ(CANCDMOB & 0xC0, 0x00);

    // Verifying resumption and CANCDMOB register contents
    resume_mob(&tx_mob);
    ASSERT_EQ(CANCDMOB & 0xC0, 0x40);
}

void error_handle_test(void) {
    // Simple error handling test to verify that flags are cleared and msgs are
    // transmitted correctly when an error is thrown

    tx_mob.dlc = 10; // Set incorrect length and send msg
    resume_mob(&tx_mob);

    handle_rx_interrupt(&rx_mob);
    ASSERT_EQ(CANSTMOB & _BV(RXOK), 0x00);
    // Verify that RX interrupt flags are cleared

    handle_tx_interrupt(&tx_mob);
    ASSERT_EQ(CANSTMOB & _BV(TXOK), 0x00);
    // Verify that TX interrupt flags are cleared

    tx_mob.dlc = 4;
};

test_t t1 = {.name = "init_can", .fn = init_can_test };
test_t t2 = {.name = "init_tx", .fn = init_tx_test };
test_t t3 = {.name = "init_rx", .fn = init_rx_test };
test_t t4 = {.name = "pause/resume", .fn = pause_resume_test };
test_t t5 = {.name = "error handling", .fn = error_handle_test };

test_t* suite[5] = { &t1, &t2, &t3, &t4, &t5 };

int main(void) {
    run_tests(suite, 5);
    return 0;
}
