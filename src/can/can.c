#include <can/can.h>
#include <uart/log.h>

void* mob_array[6] = {0};

void select_mob(uint8_t mob_num) {
    print("Selected mob %d\n", mob_num);
    CANPAGE = mob_num << 4;
}

void set_id_tag(mob_id_tag_t id_tag) {
    CANIDT2 = id_tag.tab[0] << 5;
    CANIDT1 = (id_tag.tab[1] << 5) | (id_tag.tab[0] >> 3);
    print("Set mob id_tag\n");
}

void set_id_mask(mob_id_mask_t id_mask) {
    CANIDM2 = id_mask.tab[0] << 5;
    CANIDM1 = (id_mask.tab[1] << 5) | (id_mask.tab[0] >> 3);
    print("Set mob id mask\n");
}

void set_ctrl_flags(mob_ctrl_t ctrl) {
    if (ctrl.ide) CANCDMOB |= _BV(IDE); // this should never happen
    else CANCDMOB &= ~(_BV(IDE)); // set the IDE bit to 0, since we're using rev A

    if (ctrl.ide_mask) CANIDM4 |= _BV(IDEMSK);
    else CANIDM4 &= ~(_BV(IDEMSK));

    if (ctrl.rtr_mask) CANIDM4 |= _BV(RTRMSK);
    else CANIDM4 &= ~(_BV(RTRMSK));

    if (ctrl.rtr) CANIDT4 |= _BV(RTRTAG);
    else CANIDT4 &= ~(_BV(RTRTAG));

    if (ctrl.rbn_tag) CANIDT4 |= _BV(RB0TAG);
    else CANIDT4 &= ~(_BV(RB0TAG));

    print("Set mob ctrl:\n ide: %d ide_mask: %d rtr: %d rtr_mask: %d rbn: %d\n", ctrl.ide, ctrl.ide_mask, ctrl.rtr, ctrl.rtr_mask, ctrl.rbn_tag);
}

void init_can() {
    CANGCON |= _BV(SWRES);

    // TODO: figure out why these settings work, and why they weren't working
    // earlier. These settings are taken directly from the 32m1 datasheet,
    // pg 240
    CANBT1 = 0x08;
    CANBT2 = 0x0C;
    CANBT3 = 0x37;

    CANGIE |= _BV(ENIT) | _BV(ENTX) | _BV(ENRX);
    // enable all CAN interrupts, execept the overrun timer

    // disable all mobs
    for (uint8_t i = 0; i < 6; i++) {
        select_mob(i);
        CANCDMOB = 0x00;
    }

    sei();

    CANGCON |= _BV(ENASTB);
    // enable CAN

    while (!(CANGSTA & _BV(ENFG))) {}
    // wait for CAN to turn on before returning

    print("CAN initialized\n");
}

void pause_rx_mob(rx_mob_t* mob) {
    select_mob(mob->mob_num);

    // TODO: only do this RX is not busy?
    // disable the mob
    CANCDMOB &= ~(_BV(CONMOB0));
    CANCDMOB &= ~(_BV(CONMOB1));
    print("Mob %d paused\n", mob->mob_num);
}

void resume_rx_mob(rx_mob_t* mob) {
    select_mob(mob->mob_num);

    // enable the mob in RX mode
    CANCDMOB |= _BV(CONMOB1);
    CANCDMOB &= ~(_BV(CONMOB0));
    print("Mob %d resumed\n", mob->mob_num);
}

void pause_tx_mob(tx_mob_t* mob) {
    select_mob(mob->mob_num);

    // TODO: only do this when TX is not busy?
    // disable the mob
    CANCDMOB &= ~(_BV(CONMOB0));
    CANCDMOB &= ~(_BV(CONMOB1));
    print("Mob %d paused\n", mob->mob_num);
}

void resume_tx_mob(tx_mob_t* mob) {
    select_mob(mob->mob_num);

    // enable the mob in TX mode
    CANCDMOB |= _BV(CONMOB0);
    CANCDMOB &= ~(_BV(CONMOB1));
    print("Mob %d resumed\n", mob->mob_num);
}

void init_rx_mob(rx_mob_t* mob) {
    select_mob(mob->mob_num);

    set_id_tag(mob->id_tag);
    set_id_mask(mob->id_mask);
    set_ctrl_flags(mob->ctrl);

    CANCDMOB |= mob->dlc;

    // enable the required interrupts
    CANGIE |= _BV(ENRX);
    CANIE2 |= _BV(mob->mob_num);

    // add mob to mob_list
    mob_array[mob->mob_num] = mob;

    resume_rx_mob(mob); // enable mob
    print("RX mob initialized\n");
    print("mob_num: %d dlc: %d\n", mob->mob_num, mob->dlc);
}

void init_tx_mob(tx_mob_t* mob) {
    select_mob(mob->mob_num);

    set_id_tag(mob->id_tag);
    set_ctrl_flags(mob->ctrl);

    // load data from callback
    (mob->tx_data_cb)(mob->data, &(mob->dlc));

    uint8_t len = mob->dlc;
    CANCDMOB |= len;

    CANPAGE &= ~(0x07); // reset data buffer index
    for (uint8_t i = 0; i < len; i++) {
        CANMSG = (mob->data)[i]; // data buffer index auto-incremented
    }

    CANGIE |= _BV(ENTX);
    CANIE2 |= _BV(mob->mob_num);

    mob_array[mob->mob_num] = mob;
    pause_tx_mob(mob); // tx mobs must be resumed manually

    print("TX mob initialized\n");
    print("mob_num: %d dlc: %d\n", mob->mob_num, mob->dlc);
    print("data: %s\n", (char *) mob->data);
}

void handle_rx_interrupt(rx_mob_t* mob) {
    print("Handling RX interrupt\n");

    select_mob(mob->mob_num);

    uint8_t len = CANCDMOB & 0x0F;
    mob->dlc = (len >= 8) ? 8 : len;
    // update dlc only; no need to update id/mask

    CANPAGE &= ~(0x07); // reset data buffer index

    uint8_t data[8] = {0};
    for (uint8_t j = 0; j < 8; j++) {
        data[j] = CANMSG; // reading auto-increments the data buffer index
    }

    (mob->rx_cb)(data, len); // execute callback

    CANSTMOB &= ~(_BV(RXOK));  // clear interrupt flag

    resume_rx_mob(mob);
    // required because ENMOB is reset after RXOK goes high
}

void handle_tx_interrupt(tx_mob_t* mob) {
    print("Handling TX interrupt\n");

    select_mob(mob->mob_num);
    // update dlc and load the next 8 bytes
    // load the next 8 bytes to send
    uint8_t len = 0;
    uint8_t data[8] = {0};

    (mob->tx_data_cb)(data, &len);
    print("data: %s len: %d\n", (char *) data, len);

    CANSTMOB &= ~(_BV(TXOK));  // clear interrupt flag
    // this also resets the mob, without clearing any of the data fields
    // this is why we must resume the mob if there is still data left to send

    if (len != 0) {
        CANPAGE &= ~(0x07); // reset data buffer index
        for (uint8_t i = 0; i < len; i++) {
            CANMSG = data[i]; // data buffer index auto-incremented
        }

        resume_tx_mob(mob);
    } else {
        pause_tx_mob(mob);
    }
}

uint8_t rx_mob_status(rx_mob_t* mob) {
    select_mob(mob->mob_num);
    return CANSTMOB;
}

uint8_t tx_mob_status(tx_mob_t* mob) {
    select_mob(mob->mob_num);
    return CANSTMOB;
}

ISR(CAN_INT_vect){
    print("Interrupt received\n");
    for (uint8_t i = 0; i < 6; i++) {
        // ignore un-initialized mobs
        if (mob_array[i] == 0)
            continue;

        select_mob(i);
        // should disable interrupts for the time being
        print("Status: %#02x\n", CANSTMOB);
        uint8_t err = CANSTMOB & 0x9f;

        if (err != 0) {
            print("CAN error\n");
            CANSTMOB &= ~(0x9f);
            // TODO: shouldn't just clear the error and move on..
            continue;
        }

        if (CANSTMOB & _BV(RXOK)) {
            print("RX OK received\n");
            rx_mob_t* mob = (rx_mob_t *) mob_array[i];
            handle_rx_interrupt(mob);
        } else if (CANSTMOB & _BV(TXOK)) {
            print("TX OK received\n");
            tx_mob_t* mob = (tx_mob_t *) mob_array[i];
            handle_tx_interrupt(mob);
        }
    }
}
