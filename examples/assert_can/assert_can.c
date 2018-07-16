#include <uart/uart.h>
#include <can/can.h>
#include <test/test.h>
#include <util/delay.h>

void tx_callback(uint8_t*, uint8_t*);
void rx_callback(const uint8_t*, uint8_t);
void select_mob(uint8_t);
uint8_t load_data(mob_t*);
void handle_rx_interrupt(mob_t*);
void handle_tx_interrupt(mob_t*);

mob_t tx_mob = {
    .mob_num = 0,
    .mob_type = TX_MOB,
    .id_tag = { 0x0001 },
    .ctrl = default_tx_ctrl,
    .tx_data_cb = tx_callback,
};


mob_t rx_mob = {
    .mob_num = 1,
    .mob_type = RX_MOB,
    .dlc = 1,
    .id_tag = { 0x0000 },
    .id_mask = { 0x0000 },
    .ctrl = default_rx_ctrl,
    .rx_cb = rx_callback
};

uint8_t DATA[] = {0,1,2,3};

uint8_t DATA_R[4];

void rx_callback(const uint8_t* data, uint8_t len) {
    print("TX received!\n");
    _delay_ms(200);
    print("%s\n", (char *) data);
    for (int i = 0; i < len; i++){
      DATA_R[i] = DATA[i];
      print("DATA_R[i]: %d",DATA_R[i]);
    }
    _delay_ms(200);
}

void tx_callback(uint8_t* data, uint8_t* len) {
    *len = 4;
    data = DATA;
    print("TX sent\n");
}

void select_mob(uint8_t mob_num) {
    CANPAGE = mob_num << 4;
}

int main(void){
    init_uart();
    init_can();

    print("Initializing CAN ASSERT_TRUE Testing\n");
    print("Verifying that registers are set appropriately: \n");
    print("init_can() test\n\n");
    ASSERT_TRUE((CANGCON & 0x03) == 0x02);
    ASSERT_TRUE(CANBT1 == 0x08);
    ASSERT_TRUE(CANBT2 == 0x0C);
    ASSERT_TRUE(CANBT3 == 0x37);
    ASSERT_TRUE(CANGIE == 0xB8);

    print("Verify that MObs are reset\n\n");
    for (int i = 0; i < 3; i++){
        select_mob(i);
        ASSERT_TRUE(CANCDMOB == 0x00);
        ASSERT_TRUE(CANSTMOB == 0x00);
    }

    print("Verify that flags are cleared\n\n");
    ASSERT_TRUE((CANGSTA & _BV(ENFG)) == _BV(ENFG));

    print("Initializing tx MOb...\n\n");
    init_tx_mob(&tx_mob);
    ASSERT_TRUE((CANGIE & 0x10) == 0x10);
    ASSERT_TRUE((CANIE2 & _BV(tx_mob.mob_num)) == _BV(tx_mob.mob_num));
    ASSERT_TRUE(CANIDT2 == tx_mob.id_tag.tab[0] << 5);
    ASSERT_TRUE(CANIDT1 == (tx_mob.id_tag.tab[1] << 5) | (tx_mob.id_tag.tab[0] >> 3));

    print("Initializing rx MOb...\n\n");

    init_rx_mob(&rx_mob);
    ASSERT_TRUE((CANGIE & 0x20) == 0x20);//ENRX enabled
    ASSERT_TRUE((CANIE2 & _BV(rx_mob.mob_num)) == _BV(rx_mob.mob_num));//correct interrupts enabled

    print("Testing id_tags and id_masks for rx... \n");
    ASSERT_TRUE(CANIDT2 == rx_mob.id_tag.tab[0] << 5);
    ASSERT_TRUE(CANIDT1 == (rx_mob.id_tag.tab[1] << 5) | (rx_mob.id_tag.tab[0] >> 3));//fail
    ASSERT_TRUE(CANIDM2 == rx_mob.id_mask.tab[0] << 5);
    ASSERT_TRUE(CANIDM1 == (rx_mob.id_mask.tab[1] << 5) | (rx_mob.id_mask.tab[0] >> 3));

    resume_mob(&tx_mob);
    while (!is_paused(&tx_mob)) {};
    _delay_ms(100);

    for (int i = 0; i < 4; i++){
      print("D: %d\n",DATA_R[i]);
      ASSERT_TRUE(DATA_R[i] == DATA[i]);
    }

    print("CANPAGE register testing\n\n");
    for (int i = 0; i < 2; i++){
        select_mob(i);
        ASSERT_TRUE((CANPAGE)== (i << 4));
        print("MOb_num: %d\n CANPAGE: %d\n", i << 4, CANPAGE);
    }

    print("Pause MOb test\n\n");

    pause_mob(&tx_mob);
    ASSERT_TRUE((CANCDMOB & 0xC0) == 0x00);

    print("Resume mob test\n\n");

    resume_mob(&tx_mob);
    print("CANCDMOB register contents:\n\n");
    ASSERT_TRUE((CANCDMOB & 0xC0) == 0x80);
    print("%x\n",CANCDMOB);

    tx_mob.dlc = 10;//set incorrect length and send msg
    resume_mob(&tx_mob);
    while (!is_paused(&tx_mob)) {};
    _delay_ms(100);

    print("CANSTMOB: %x\n", CANSTMOB);//expected to be 1000 0000 (80)

    handle_rx_interrupt(&rx_mob);
    ASSERT_TRUE((CANSTMOB & _BV(RXOK)) == 0x00);//verify that interrupt flags are cleared

    handle_tx_interrupt(&tx_mob);
    ASSERT_TRUE((CANSTMOB & _BV(TXOK)) == 0x00);//verify that interrupt flags are cleared

    print("Tests complete\n");
    return 0;

}
