/* https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8209-8-bit%20AVR%20ATmega16M1-32M1-64M1_Datasheet.pdf */

#include <stdio.h> // Need to import these libraries.
#include <stdlib.h>
#include <can/can.h>
#include <uart/uart.h>

#define ERR_MSG "ERR: %s.\n"

mob_t* mob_array[6] = {0};

uint8_t boffit_count = 0;

// Selects the relevant mob from the CANPAGE register, in order to access
// registers that are duplicated for each mob
static inline void select_mob(uint8_t mob_num) {
    CANPAGE = mob_num << 4;
}

// Sets Identifier Tag registers
static inline void set_id_tag(mob_id_tag_t id_tag) {
    CANIDT2 = id_tag.tab[0] << 5;
    CANIDT1 = (id_tag.tab[1] << 5) | (id_tag.tab[0] >> 3);
}

// Sets Identifier Mask registers
static inline void set_id_mask(mob_id_mask_t id_mask) {
    CANIDM2 = id_mask.tab[0] << 5;
    CANIDM1 = (id_mask.tab[1] << 5) | (id_mask.tab[0] >> 3);
}

static inline void set_ctrl_flags(mob_ctrl_t ctrl) {
    // The IDE bit sets the CAN version
    if (ctrl.ide) {
        CANCDMOB |= _BV(IDE); // this should never happen (version B)
    }

    else {
        CANCDMOB &= ~(_BV(IDE)); // Version 2.0 A
    }

    // RB0 bit of the remote or data frame to send (updated with value of received frame)
    if (ctrl.ide_mask) {
        CANIDM4 |= _BV(IDEMSK); // Bit comparison enabled
    }
    else {
        CANIDM4 &= ~(_BV(IDEMSK)); // Comparison true forced
    }

    // Remote Transmission Request Mask
    if (ctrl.rtr_mask) {
        CANIDM4 |= _BV(RTRMSK); // Bit comparison enabled
    }
    else {
        CANIDM4 &= ~(_BV(RTRMSK)); // Comparison true forced
    }

    // Remote Transmission Request
    if (ctrl.rtr) {
        CANIDT4 |= _BV(RTRTAG); // 1 for remote frames (no data)
    }
    else {
        CANIDT4 &= ~(_BV(RTRTAG)); // 0 for data frames
    }

    // Reserved Bit 0 Tag (updated with value of received frame)
    if (ctrl.rbn_tag) {
        CANIDT4 |= _BV(RB0TAG);
    }
    else {
        CANIDT4 &= ~(_BV(RB0TAG));
    }

    // Used in the automatic reply mode after receiving a remote frame
    // Since we do not use auto-mobs, this should always be 0
    if (ctrl.rplv) {
        CANCDMOB |= _BV(RPLV); // Reply ready and valid
    }
    else {
        CANCDMOB &= ~(_BV(RPLV)); // Reply not ready
    }
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

    return len;
}

// Tests to see if the selected mob is paused
uint8_t is_paused(mob_t* mob) {
    select_mob(mob->mob_num);

    // the MOb is paused iff the two MSB of CANCDMOB are 0
    if (CANCDMOB & 0xc0) {
        return 0;
    } else {
        return 1;
    }
}

// Initializes CAN protocol
void init_can(void) {
    // Reset CAN controller and set communication baud rate to default (100 kbps)
    void set_can_baud_rate(can_baud_rate_t baud_rate); // Need to formally declare this function first.
    CANGCON |= _BV(SWRES);
    set_can_baud_rate(CAN_DEF_BAUD_RATE);

<<<<<<< HEAD

<<<<<<< HEAD
    CANGIE |= _BV(ENIT) | _BV(ENTX) | _BV(ENRX) | _BV(ENERR);
=======
    CANGIE |= _BV(ENIT) | _BV(ENBOFF) |_BV(ENTX) | _BV(ENRX) | _BV(ENERR);
    //TODO: deal with error interrupts? - BOFFIT
>>>>>>> b2507a5... Custom baud rate and BOFFIT Interrupt Handling
=======
    CANGIE |= _BV(ENIT) | _BV(ENBOFF) | _BV(ENRX) |  _BV(ENTX) |  _BV(ENERR);
>>>>>>> 802e49e... Added documentation to the CAN library code and removed references to automobs
    // enable most CAN interrupts, execept the overrun timer, and general errors

    // disable all mobs, clear all interrupt flags
    for (uint8_t i = 0; i < 6; i++) {
        select_mob(i);
        CANCDMOB = 0x00;
        CANSTMOB = 0x00;
    }

    // set global interrupt flag
    sei();

    // set can to enable mode
    CANGCON |= _BV(ENASTB);
    while (!(CANGSTA & _BV(ENFG))) {}
    // enable CAN and wait for CAN to turn on before returning
<<<<<<< HEAD
=======
    // added a timeout for this infinite loop
>>>>>>> 802e49e... Added documentation to the CAN library code and removed references to automobs
}

// Pauses the selected mob by setting 2 MSB of CANCDMOB to 0
void pause_mob(mob_t* mob) {
    select_mob(mob->mob_num);
    switch (mob->mob_type) {
        case TX_MOB:
            CANCDMOB &= ~(_BV(CONMOB0) | _BV(CONMOB1));
            break;
        case RX_MOB: // RX mob should not be paused
            break;
        }
}

// Resumes the selected mob
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
        // It is not necessary to resume the RX mob, as it should not be paused.
        // However, this is called in the init_rx function
        case RX_MOB:
            CANCDMOB &= ~(_BV(CONMOB0));
            CANCDMOB |= _BV(CONMOB1);
    }
}

// Initializes RX mob
void init_rx_mob(mob_t* mob) {
    select_mob(mob->mob_num);

    // Sets ID tags, masks, and ctrl flags
    set_id_tag(mob->id_tag);
    set_id_mask(mob->id_mask);
    set_ctrl_flags(mob->ctrl);

    // Sets data length, ranging from 0 to 8, as defined in rx_mob struct
    // If the number is greater than 8, it may interfere with other CANCDMOB bits
    if (mob->dlc > 8){
        mob->dlc = 8;
    }
    CANCDMOB |= mob->dlc;

    // enable global RX interrupts and interrupts for selected mob
    // NOTE: This is redundant with init_can
    CANGIE |= _BV(ENRX);
    CANIE2 |= _BV(mob->mob_num);

    // add mob to mob_list
    mob_array[mob->mob_num] = mob;

    // does this do anything, since the appropriate case is empty?
    // Case is now filled in, test if below line is necessary
    resume_mob(mob); // enable mob
}

// Initializes TX mob
void init_tx_mob(mob_t* mob) {
    select_mob(mob->mob_num);

    // Sets ID tags, masks, and ctrl flags
    set_id_tag(mob->id_tag);
    set_ctrl_flags(mob->ctrl);
    // Remote frame, so dlc is 0
    mob->dlc = 0;

    // enable global TX interrupts and interrupts on selected mob
    // NOTE: This is redundant with init_can
    CANGIE |= _BV(ENTX);
    CANIE2 |= _BV(mob->mob_num);

    // add mob to mob_list
    mob_array[mob->mob_num] = mob;
    // tx mobs must be resumed manually
    pause_mob(mob);
}


// Handles interrupts for RX mobs
void handle_rx_interrupt(mob_t* mob) {
    select_mob(mob->mob_num);

    // we must reset the ID and various flags because they
    // have been copied over from the sender
    set_id_tag(mob->id_tag);
    set_id_mask(mob->id_mask);
    set_ctrl_flags(mob->ctrl);

    uint8_t len = CANCDMOB & 0x0F;
    if (len > 8){
        len = 8;
    }
    mob->dlc = len;

    CANPAGE &= ~(0x07); // reset data buffer index

    // Reads data from CANMSG
    // reading auto-increments the data buffer index
    for (uint8_t j = 0; j < len; j++) {
        (mob->data)[j] = CANMSG;
    }

    // executes rx callback
    (mob->rx_cb)(mob->data, len);

    // clear interrupt flag
    CANSTMOB &= ~(_BV(RXOK));

    if (mob->mob_type == TX_MOB) pause_mob(mob);
    else resume_mob(mob);
    // resume_mob(mob);
    // required because ENMOB is reset after RXOK goes high
}

// Handles interrupts for TX mobs
void handle_tx_interrupt(mob_t* mob) {

    select_mob(mob->mob_num);
    CANSTMOB &= ~(_BV(TXOK));  // clear interrupt flag
    // this also resets the mob, without clearing any of the data fields
    // this is why we must resume the mob if there is still data left to send

    pause_mob(mob);
}

// Returns contents of CANSTMOB register
uint8_t mob_status(mob_t* mob) {
    select_mob(mob->mob_num);
    return CANSTMOB;
}

// Prints error statements
// Most errors are handled by automatically by CAN
uint8_t handle_err(mob_t* mob) {
    select_mob(mob->mob_num);

    uint8_t err = mob_status(mob) & 0x9f;

    if (err != 0) {
        if (err & _BV(DLCW)) {
            print(ERR_MSG, "Bad DLC");
        } else if (err & _BV(BERR)) {
            print(ERR_MSG, "Bit error");
        } else if (err & _BV(SERR)) {
            print(ERR_MSG, "Bit stuffing error");
        } else if (err & _BV(CERR)) {
            print(ERR_MSG, "CRC mismatch");
        } else if (err & _BV(FERR)) {
            print(ERR_MSG, "Form error");
        } else if (err & _BV(AERR)) {
            print(ERR_MSG, "No ack");
        }

        // Clears CANSTMOB error bits
        CANSTMOB &= ~(0x9f);
        return 1;
    }

    return 0;
}

// Sets baud rate for CAN (pg. 240 of data sheet)
void set_can_rate_reg(uint8_t canbt1, uint8_t canbt2, uint8_t canbt3){
    CANBT1 = canbt1;
    CANBT2 = canbt2;
    CANBT3 = canbt3;
}

// Pre-defined settings for baud rate. 100 is the default.
void set_can_baud_rate(can_baud_rate_t baud_rate){
switch (baud_rate) {
    case CAN_RATE_100:
        set_can_rate_reg(8, 12, 55);
        break;
    case CAN_RATE_125:
        set_can_rate_reg(6, 12, 55);
        break;
    case CAN_RATE_250:
        set_can_rate_reg(2, 12, 55);
        break;
    case CAN_RATE_500:
        set_can_rate_reg(0, 12, 54);
        break;
    case CAN_RATE_1000:
        set_can_rate_reg(0, 4, 18);
        break;
    default:
        break;
}
}
// Handles the circumstance where the CAN channel is not allowed to have
// any influence on bus (i.e. entering bus off mode)
// Reference pg. 237 for error management
void handle_bus_off_interrupt(mob_t* mob) {
    select_mob(mob->mob_num); //not sure if this is necessary

    CANGIT |= _BV(BOFFIT); //setting this bit clears it

    // do something
    // i.e. store appropriate information to be retrieved later and modify global variable
    //   that keeps track of number of BOFFIT interrupts
    // handle interrupt by performing a software reset

    if(boffit_count == 0){
        print("Resetting CAN controller...");
        CANGCON |= _BV(SWRES); // Only resets CAN controller
        CANGCON |= _BV(ENASTB); // Enable mode (0x02)
        while (!(CANGSTA & _BV(ENFG))){ // Wait until enabled
            print("Waiting...");
        }
        print("Node enabled...");
    }
    else if (boffit_count < 3){
        print("Aborting message...");
        CANGCON |= _BV(ABRQ); // Disable pending and terminate current communications
    }

    else {
        print("Standby mode...");
        CANGCON |= _BV(SWRES); // Only resets CAN controller
        CANGCON |= ~(_BV(ENASTB)); // Disable node (0x02)
    }

    // NOTE:
    // Force enable mode: CANGCON |= _BV(ENASTB);
    // Current status of bus: CANGSTA & _BV(BOFF);
    // Error passive mode: CANGSTA & _BV(ERRP);
    // Software reset: CANGCON |= _BV(SWRES); (this resets CAN controller)

}

// ISR routine for CAN to handle various interrupts
ISR(CAN_INT_vect) {
    for (uint8_t i = 0; i < 6; i++) {
        mob_t* mob = mob_array[i];
        if (mob == 0) {continue;}
        else {select_mob(i);}

        uint8_t status = mob_status(mob);

        if (handle_err(mob)) {continue;}

        // Bus off interrupt
        if (CANGIT & _BV(BOFFIT)){
            handle_bus_off_interrupt(mob);
        }

        // RX interrupts
        if (status & _BV(RXOK)) {
            handle_rx_interrupt(mob);
        }

        // TX interrupts
        else if (status & _BV(TXOK)) {
            switch (mob->mob_type) {
                case TX_MOB:
                    handle_tx_interrupt(mob);
                    break;
                default:
                    // should never get here
                    handle_err(mob);
                    break;
            }
        }
    }
}
