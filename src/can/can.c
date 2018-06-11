#include <can/can.h>
#include <uart/uart.h>

#define ERR_MSG "ERR: %s.\n"

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

uint8_t load_data(mob_t* mob) {
    // load data from callback
    (mob->tx_data_cb)(mob->data, &(mob->dlc));

    select_mob(mob->mob_num);
    uint8_t len = mob->dlc;

    CANCDMOB &= ~(0x0f);
    CANCDMOB |= len;

    CANPAGE &= ~(0x07); // reset data buffer index
    for (uint8_t i = 0; i < len; i++) {
        CANMSG = (mob->data)[i]; // data buffer index auto-incremented
    }

    if (len) {
        print("Loaded data: %s len: %d\n", (char*) mob->data, mob->dlc);
    }

    return len;
}

uint8_t is_paused(mob_t* mob) {
    select_mob(mob->mob_num);

    // the MOb is paused iff the upper two bits of CANCDMOB are 0
    if (CANCDMOB & 0xc0) {
        return 0;
    } else {
        return 1;
    }
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
            if (load_data(mob) == 0) return;
            // load data before resuming the MOb; if there is no new data
            // do not resume the MOb
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
}

void init_tx_mob(mob_t* mob) {
    select_mob(mob->mob_num);

    set_id_tag(mob->id_tag);
    set_ctrl_flags(mob->ctrl);
    mob->dlc = 0;

    CANGIE |= _BV(ENTX);
    CANIE2 |= _BV(mob->mob_num);

    mob_array[mob->mob_num] = mob;
    pause_mob(mob); // tx mobs must be resumed manually
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
}

void handle_rx_interrupt(mob_t* mob) {
    print("Handling %s interrupt\n", "RX");

    select_mob(mob->mob_num);

    // we must reset the ID and various flags because they
    // have been copied over from the sender
    set_id_tag(mob->id_tag);
    set_id_mask(mob->id_mask);
    set_ctrl_flags(mob->ctrl);

    uint8_t len = CANCDMOB & 0x0F;
    mob->dlc = (len >= 8) ? 8 : len;

    CANPAGE &= ~(0x07); // reset data buffer index

    for (uint8_t j = 0; j < len; j++) {
        (mob->data)[j] = CANMSG; // reading auto-increments the data buffer index
    }

    (mob->rx_cb)(mob->data, len); // execute callback

    CANSTMOB &= ~(_BV(RXOK));  // clear interrupt flag

    if (mob->mob_type == TX_MOB) pause_mob(mob);
    else resume_mob(mob);
    // resume_mob(mob);
    // required because ENMOB is reset after RXOK goes high
}

void handle_tx_interrupt(mob_t* mob) {
    print("Handling %s interrupt\n", "TX");

    /* TODO: Add some kind of burst mode, which looks like:
    // Try to load more data automatically
    load_data(mob);

    select_mob(mob->mob_num);
    CANSTMOB &= ~(_BV(TXOK));  // clear interrupt flag

    // if we successfully loaded more data, resume (but without calling
    // load_data)

    if (mob->dlc != 0) resume_mob(mob);
    else pause_mob(mob);
    */

    select_mob(mob->mob_num);
    CANSTMOB &= ~(_BV(TXOK));  // clear interrupt flag
    // this also resets the mob, without clearing any of the data fields
    // this is why we must resume the mob if there is still data left to send

    pause_mob(mob);
}

void handle_auto_tx_interrupt(mob_t* mob) {
    print("Handling %s interrupt\n", "AUTO TX");

    select_mob(mob->mob_num);

    // TODO: Many of the variables, including IDTAG, dlc, etc
    // are copied from the incoming remote frame; thus, they must all be
    // reset and restored to their original values
    // In particular, the RTR tag and RPLV bit must be reset to their
    // original values
    // as in the TX case, if there is no data left, pause the mob
    // otherwise, load fresh data via tx callback

    // TODO: maybe make this work like TX MObs?

    load_data(mob);

    // clear interrupt flag
    CANSTMOB &= ~(_BV(TXOK));

    // this should happen all at once
    if (mob->dlc != 0) resume_mob(mob);
    else pause_mob(mob);

    // TODO: for some reason, we need to set these AFTER
    set_id_tag(mob->id_tag);
    set_id_mask(mob->id_mask);
    set_ctrl_flags(mob->ctrl);
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
            print(ERR_MSG, "Bad DLC");
            //print("ERR: Incoming message did not have expected DLC.\n");
        } else if (err & _BV(BERR)) {
            print(ERR_MSG, "Bit error");
            //print("ERR: Bit error.\n");
        } else if (err & _BV(SERR)) {
            print(ERR_MSG, "Bit stuffing error");
            //print("ERR: Five consecutive bits with same polarity.\n");
        } else if (err & _BV(CERR)) {
            print(ERR_MSG, "CRC mismatch");
            //print("ERR: CRC mismatch.\n");
        } else if (err & _BV(FERR)) {
            print(ERR_MSG, "Form error");
            //print("ERR: Form error.\n");
        } else if (err & _BV(AERR)) {
            print(ERR_MSG, "No ack");
            //print("ERR: No acknowledgement.\n");
        }

        // TODO: is this the best way to handle errors?
        CANSTMOB &= ~(0x9f);
        return 1;
    }

    return 0;
}

ISR(CAN_INT_vect) {
    print("Interrupt received\n");
    for (uint8_t i = 0; i < 6; i++) {
        mob_t* mob = mob_array[i];

        if (mob == 0) continue;
        else select_mob(i);

        uint8_t status = mob_status(mob);
        print("Status: %#.2x\n", status);

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
                default:
                    // should never get here
                    handle_err(mob);
                    break;
            }
        }
    }
}
