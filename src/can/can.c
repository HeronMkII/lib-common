#include <can/can.h>
#include <uart/log.h>

mob_t* mob_array[6] = {0};

static inline void select_mob(uint8_t mob_num) {
    CANPAGE = mob_num << 4;
}

static inline void set_id_tag(mob_id_tag_t id_tag) {
    CANIDT2 = id_tag.tab[0] << 5;
    CANIDT1 = (id_tag.tab[1] << 5) | (id_tag.tab[0] >> 3);
}

static inline void set_id_mask(mob_id_mask_t id_mask) {
    CANIDM2 = id_mask.tab[0] << 5;
    CANIDM1 = (id_mask.tab[1] << 5) | (id_mask.tab[0] >> 3);
}

static inline void set_ctrl_flags(mob_ctrl_t ctrl) {
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

    if (ctrl.rplv) CANCDMOB |= _BV(RPLV);
    else CANCDMOB &= ~(_BV(RPLV));
}

void load_data(mob_t* mob) {
    select_mob(mob->mob_num);

    // load data from callback
    (mob->tx_data_cb)(mob->data, &(mob->dlc));

    uint8_t len = mob->dlc;
    CANCDMOB |= len;

    CANPAGE &= ~(0x07); // reset data buffer index
    for (uint8_t i = 0; i < len; i++) {
        CANMSG = (mob->data)[i]; // data buffer index auto-incremented
    }

    print("data: %s len: %d\n", (char *) mob->data, mob->dlc);
}

void init_can() {
    CANGCON |= _BV(SWRES);

    // TODO: figure out why these settings work, and why they weren't working
    // earlier. These settings are taken directly from the 32m1 datasheet,
    // pg 240
    CANBT1 = 0x08;
    CANBT2 = 0x0C;
    CANBT3 = 0x37;

    CANGIE |= _BV(ENIT) | _BV(ENTX) | _BV(ENRX) | _BV(ENERR);
    // enable most CAN interrupts, execept the overrun timer, and general errors

    // disable all mobs, clear all interrupt flags
    for (uint8_t i = 0; i < 6; i++) {
        select_mob(i);
        CANCDMOB = 0x00;
        CANSTMOB = 0x00;
    }

    sei();

    CANGCON |= _BV(ENASTB);
    while (!(CANGSTA & _BV(ENFG))) {}
    // enable CAN and wait for CAN to turn on before returning

    print("CAN initialized\n");
}

void pause_mob(mob_t* mob) {
    select_mob(mob->mob_num);

    CANCDMOB &= ~(_BV(CONMOB0) | _BV(CONMOB1));
    print("Mob %d paused\n", mob->mob_num);
}

void resume_mob(mob_t* mob) {
    select_mob(mob->mob_num);

    switch (mob->mob_type) {
        case TX_MOB:
            CANCDMOB |= _BV(CONMOB0);
            CANCDMOB &= ~(_BV(CONMOB1));
            break;
        case RX_MOB:
        case AUTO_MOB:
            CANCDMOB |= _BV(CONMOB1);
            CANCDMOB &= ~(_BV(CONMOB0));
            break;
    }

    print("Mob %d resumed\n", mob->mob_num);
}

void init_rx_mob(mob_t* mob) {
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

    resume_mob(mob); // enable mob
    print("RX mob initialized\n");
    print("mob_num: %d dlc: %d\n", mob->mob_num, mob->dlc);
}

void init_tx_mob(mob_t* mob) {
    select_mob(mob->mob_num);

    set_id_tag(mob->id_tag);
    set_ctrl_flags(mob->ctrl);

    load_data(mob);

    CANGIE |= _BV(ENTX);
    CANIE2 |= _BV(mob->mob_num);

    mob_array[mob->mob_num] = mob;
    pause_mob(mob); // tx mobs must be resumed manually

    print("TX mob initialized\n");
    print("mob_num: %d dlc: %d\n", mob->mob_num, mob->dlc);
}

void init_auto_mob(mob_t* mob) {
    select_mob(mob->mob_num);

    set_id_tag(mob->id_tag);
    set_id_mask(mob->id_mask);
    set_ctrl_flags(mob->ctrl);

    // TODO: this data might be stale
    // ideally, you'd load the data just before the auto-reply was sent
    load_data(mob);

    CANGIE |= _BV(ENRX) | _BV(ENTX);
    CANIE2 |= _BV(mob->mob_num);

    mob_array[mob->mob_num] = mob;
    pause_mob(mob);
    print("Auto MOb initialized\n");
}

void handle_rx_interrupt(mob_t* mob) {
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

    resume_mob(mob);
    // required because ENMOB is reset after RXOK goes high
}

void handle_tx_interrupt(mob_t* mob) {
    print("Handling TX interrupt\n");

    select_mob(mob->mob_num);
    load_data(mob);
    // update dlc and load the next 8 bytes
    // load the next 8 bytes to send

    CANSTMOB &= ~(_BV(TXOK));  // clear interrupt flag
    // this also resets the mob, without clearing any of the data fields
    // this is why we must resume the mob if there is still data left to send

    if (mob->dlc != 0) resume_mob(mob);
    else pause_mob(mob);
}

void handle_auto_tx_interrupt(mob_t* mob) {
    print("Handling AUTO TX interrupt\n");

    select_mob(mob->mob_num);

    // TODO: Many of the variables, including IDTAG, dlc, etc
    // are copied from the incoming remote frame; thus, they must all be
    // reset and restored to their original values
    // In particular, the RTR tag and RPLV bit must be reset to their
    // original values
    // as in the TX case, if there is no data left, pause the mob
    // otherwise, load fresh data via tx callback

    set_id_tag(mob->id_tag);
    set_id_mask(mob->id_mask);
    set_ctrl_flags(mob->ctrl);

    load_data(mob);

    // clear all interrupts
    CANSTMOB = 0x00;

    if (mob->dlc != 0) resume_mob(mob);
    else pause_mob(mob);
}

uint8_t mob_status(mob_t* mob) {
    select_mob(mob->mob_num);
    return CANSTMOB;
}

uint8_t handle_err(mob_t* mob) {
    select_mob(mob->mob_num);

    uint8_t err = mob_status(mob) & 0x9f;

    if (err != 0) {
        if (err & _BV(DLCW)) {
            print("ERR: Incoming message did not have expected DLC.\n");
        } else if (err & _BV(BERR)) {
            print("ERR: Bit error.\n");
        } else if (err & _BV(SERR)) {
            print("ERR: Five consecutive bits with same polarity.\n");
        } else if (err & _BV(CERR)) {
            print("ERR: CRC mismatch.\n");
        } else if (err & _BV(FERR)) {
            print("ERR: Form error.\n");
        } else if (err & _BV(AERR)) {
            print("ERR: No acknowledgement.\n");
        }

        // TODO: is this the best way to handle errors?
        CANSTMOB &= ~(0x9f);
        return 1;
    }

    return 0;
}

ISR(CAN_INT_vect){
    print("Interrupt received\n");
    for (uint8_t i = 0; i < 6; i++) {
        mob_t* mob = mob_array[i];

        if (mob == 0) continue;
        else select_mob(i);

        uint8_t status = mob_status(mob);
        print("Status: %#02x\n", status);

        if (handle_err(mob)) continue;

        if (status & _BV(RXOK)) {
            handle_rx_interrupt(mob);
        } else if (status & _BV(TXOK)) {
            switch (mob->mob_type) {
                case TX_MOB:
                    handle_tx_interrupt(mob);
                    break;
                case AUTO_MOB:
                    handle_auto_tx_interrupt(mob);
                    break;
            }
        }
    }
}
