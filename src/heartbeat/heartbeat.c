/*
Heartbeat is a status recovery program that (1) retreives the latest mission
status from an SSM after a reset, and (2) keeps track of SSM status throughout
the mission.

The SSMs keep track of each other's status by passing their latest status
messages to one another, according to the following configuration:

    ... -> OBC -> EPS -> PAY -> ...

The ellipses indicate the pattern repeats in the obvious way.

Thus, in this configuration, we call OBC the parent of PAY, and the child of
EPS, i.e. OBC receives status messages from PAY, and send status messages to
EPS.

When a status is updated, the new status is first written to EEPROM, before
being sent to the parent.

CAN Message Format:
Byte 0: 1 (ping) or 2 (response)
Byte 1: Sender
Byte 2: Receiver
*/

// Assume init_uart() and init_can() have been called in SSM main program

#include <avr/eeprom.h>

#include <uart/uart.h>
#include <heartbeat/heartbeat.h>
#include <utilities/utilities.h>


// The current MCU's ID
uint8_t hb_self_id = 0xFF;  // None of the subsystems by default

pin_info_t obc_rst_eps = {
    .pin = HB_OBC_RST_EPS_PIN,
    .port = &HB_OBC_RST_EPS_PORT,
    .ddr = &HB_OBC_RST_EPS_DDR
};
pin_info_t obc_rst_pay = {
    .pin = HB_OBC_RST_PAY_PIN,
    .port = &HB_OBC_RST_PAY_PORT,
    .ddr = &HB_OBC_RST_PAY_DDR
};

pin_info_t eps_rst_obc = {
    .pin = HB_EPS_RST_OBC_PIN,
    .port = &HB_EPS_RST_OBC_PORT,
    .ddr = &HB_EPS_RST_OBC_DDR
};
pin_info_t eps_rst_pay = {
    .pin = HB_EPS_RST_PAY_PIN,
    .port = &HB_EPS_RST_PAY_PORT,
    .ddr = &HB_EPS_RST_PAY_DDR
};

pin_info_t pay_rst_obc = {
    .pin = HB_PAY_RST_OBC_PIN,
    .port = &HB_PAY_RST_OBC_PORT,
    .ddr = &HB_PAY_RST_OBC_DDR
};
pin_info_t pay_rst_eps = {
    .pin = HB_PAY_RST_EPS_PIN,
    .port = &HB_PAY_RST_EPS_PORT,
    .ddr = &HB_PAY_RST_EPS_DDR
};


mob_t obc_hb_mob;
mob_t eps_hb_mob;
mob_t pay_hb_mob;

// Must all be volatile because they are modified inside CAN TX/RX interrupts

// Current SSM -> ping other SSM
// true if we are currently trying to send a new ping to OBC
volatile bool hb_send_obc_ping = false;
volatile bool hb_send_eps_ping = false;
volatile bool hb_send_pay_ping = false;
// true if we successfully received the other SSM's (OBC's) response
volatile bool hb_received_obc_resp = false;
volatile bool hb_received_eps_resp = false;
volatile bool hb_received_pay_resp = false;

// Other SSM -> ping current SSM
// true if we are currently trying to respond to a received ping from OBC
volatile bool hb_send_obc_resp = false;
volatile bool hb_send_eps_resp = false;
volatile bool hb_send_pay_resp = false;

// Last recorded uptime when we sent a ping
volatile uint32_t hb_ping_prev_uptime_s = 0;
// How long to count before sending a heartbeat ping to both other subsystems
// TODO - make this different by subsystem to prevent race conditions/continuous resets
volatile uint32_t hb_ping_period_s = HB_RESET_UPTIME_THRESH;


void init_hb_resets(void);
void init_hb_rx_mob(mob_t* mob, uint8_t mob_num, uint16_t id_tag);
void init_hb_tx_mob(mob_t* mob, uint8_t mob_num, uint16_t id_tag);
void init_hb_mobs(void);
void init_hb_uptime(void);
void hb_tx_cb(uint8_t* data, uint8_t* len);
void hb_rx_cb(const uint8_t* data, uint8_t len);
void hb_uptime_cb(void);


// Assumes init_uptime() and init_can() have already been called,
// but init_rx_mob() and init_tx_mob() have NOT been called on the HB MOBs
void init_hb(uint8_t self_id) {
    // Store ID in the global variable
    hb_self_id = self_id;

    init_hb_resets();
    init_hb_mobs();
    init_hb_uptime();
}

void init_hb_resets(void) {
    pin_info_t rst_pin_1;
    pin_info_t rst_pin_2;

    switch (hb_self_id) {
        case HB_OBC:
            rst_pin_1 = obc_rst_eps;
            rst_pin_2 = obc_rst_pay;
            break;
        case HB_EPS:
            rst_pin_1 = eps_rst_obc;
            rst_pin_2 = eps_rst_pay;
            break;
        case HB_PAY:
            rst_pin_1 = pay_rst_obc;
            rst_pin_2 = pay_rst_eps;
            break;
        default:
            return;
    }

    // See table on p.96 - by default, need tri-state input with pullup (DDR = 0, PORT = 1)
    init_input_pin(rst_pin_1.pin, rst_pin_1.ddr);
    set_pin_pullup(rst_pin_1.pin, rst_pin_1.port, 1);
    init_input_pin(rst_pin_2.pin, rst_pin_2.ddr);
    set_pin_pullup(rst_pin_2.pin, rst_pin_2.port, 1);
}


// Need to define these separately here because CAN_RX_MASK_ID, default_rx_ctrl,
// and default_tx_ctrl are defined using {} initializers, which only work for
// globally declared structs (not in functions)
mob_id_mask_t hb_id_mask = CAN_RX_MASK_ID;
mob_ctrl_t hb_rx_ctrl = default_rx_ctrl;
mob_ctrl_t hb_tx_ctrl = default_tx_ctrl;

void init_hb_rx_mob(mob_t* mob, uint8_t mob_num, uint16_t id_tag) {
    mob->mob_num = mob_num;
    mob->mob_type = RX_MOB;
    mob->dlc = 8;
    // Can't set id_tag/id_mask directly because of type mob_id_tag_t/mob_id_mask_t
    mob->id_tag.std = id_tag;
    mob->id_mask = hb_id_mask;
    mob->ctrl = hb_rx_ctrl;
    mob->rx_cb = hb_rx_cb;
}

void init_hb_tx_mob(mob_t* mob, uint8_t mob_num, uint16_t id_tag) {
    mob->mob_num = mob_num;
    mob->mob_type = TX_MOB;
    mob->id_tag.std = id_tag;
    mob->ctrl = hb_tx_ctrl;
    mob->tx_data_cb = hb_tx_cb;
}

void init_hb_mobs(void) {
    switch (hb_self_id) {
        case HB_OBC:
            init_hb_rx_mob(&obc_hb_mob, 0, OBC_OBC_HB_RX_MOB_ID);
            init_hb_tx_mob(&eps_hb_mob, 1, OBC_EPS_HB_TX_MOB_ID);
            init_hb_tx_mob(&pay_hb_mob, 2, OBC_PAY_HB_TX_MOB_ID);
            break;
        case HB_EPS:
            init_hb_tx_mob(&obc_hb_mob, 0, EPS_OBC_HB_TX_MOB_ID);
            init_hb_rx_mob(&eps_hb_mob, 1, EPS_EPS_HB_RX_MOB_ID);
            init_hb_tx_mob(&pay_hb_mob, 2, EPS_PAY_HB_TX_MOB_ID);
            break;
        case HB_PAY:
            init_hb_tx_mob(&obc_hb_mob, 0, PAY_OBC_HB_TX_MOB_ID);
            init_hb_tx_mob(&eps_hb_mob, 1, PAY_EPS_HB_TX_MOB_ID);
            init_hb_rx_mob(&pay_hb_mob, 2, PAY_PAY_HB_RX_MOB_ID);
            break;
        default:
            break;
    }
}

void init_hb_uptime(void) {
    add_uptime_callback(hb_uptime_cb);
}


void hb_tx_cb(uint8_t* data, uint8_t* len) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        // Set up CAN message data to be sent
        *len = 8;

        if (hb_send_obc_ping) {
            data[0] = 1;
            data[1] = hb_self_id;
            data[2] = HB_OBC;
        } else if (hb_send_eps_ping) {
            data[0] = 1;
            data[1] = hb_self_id;
            data[2] = HB_EPS;
        } else if (hb_send_pay_ping) {
            data[0] = 1;
            data[1] = hb_self_id;
            data[2] = HB_PAY;
        } else if (hb_send_obc_resp) {
            data[0] = 2;
            data[1] = hb_self_id;
            data[2] = HB_OBC;
        } else if (hb_send_eps_resp) {
            data[0] = 2;
            data[1] = hb_self_id;
            data[2] = HB_EPS;
        } else if (hb_send_pay_resp) {
            data[0] = 2;
            data[1] = hb_self_id;
            data[2] = HB_PAY;
        } else {
            // Should not get here
            data[0] = 0;
            data[1] = 0;
            data[2] = 0;
        }
        
        data[3] = 0x00;
        data[4] = 0x00;
        data[5] = 0x00;
        data[6] = 0x00;
        data[7] = 0x00;

        print("Sending HB: ");
        print_bytes(data, *len);
    }
}

// This function will be called within an ISR when we receive a message
void hb_rx_cb(const uint8_t* data, uint8_t len) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        print("Received HB: ");
        print_bytes((uint8_t*) data, len);

        if (len != 8) {
            return;
        }
        if (data[2] != hb_self_id) {
            return;
        }
        
        // Received ping
        if (data[0] == 1) {
            switch (data[1]) {
                case HB_OBC:
                    hb_send_obc_resp = true;
                    break;
                case HB_EPS:
                    hb_send_eps_resp = true;
                    break;
                case HB_PAY:
                    hb_send_pay_resp = true;
                    break;
                default:
                    break;
            }
        } else if (data[0] == 2) {
            switch (data[1]) {
                case HB_OBC:
                    hb_received_obc_resp = true;
                    break;
                case HB_EPS:
                    hb_received_eps_resp = true;
                    break;
                case HB_PAY:
                    hb_received_pay_resp = true;
                    break;
                default:
                    break;
            }
        }
    }
}

// This will be atomic (in timer ISR)
void hb_uptime_cb(void) {
    print("%s\n", __FUNCTION__);

    // If it is time to send another ping
    // Need to set the flags - can't send CAN directly because we are inside an ISR
    if (uptime_s > hb_ping_prev_uptime_s && uptime_s >= hb_ping_prev_uptime_s + hb_ping_period_s) {
        if (hb_self_id != HB_OBC) {
            hb_send_obc_ping = true;
        }
        if (hb_self_id != HB_EPS) {
            hb_send_eps_ping = true;
        }
        if (hb_self_id != HB_PAY) {
            hb_send_pay_ping = true;
        }
        hb_ping_prev_uptime_s = uptime_s;
    }
}


bool wait_for_hb_mob_not_paused(mob_t* mob) {
    // Wait up to 100 ms
    // TODO - how long is appropriate?
    for (uint8_t i = 0; i < 100; i++) {
        if (is_paused(mob)) {
            return true;
        }
        _delay_ms(1);
    }
    return false;
}

bool wait_for_hb_resp(volatile bool* received_resp) {
    // Wait up to 1 s
    // TODO - how long is appropriate?
    for (uint16_t i = 0; i < 1000; i++) {
        if (*received_resp) {
            return true;
        }
        _delay_ms(1);
    }
    return false;
}


void send_hb_resp(mob_t* mob, volatile bool* send_resp) {
    resume_mob(mob);
    wait_for_hb_mob_not_paused(mob);
    *send_resp = false;
}

void send_hb_ping(mob_t* mob, uint8_t other_id, volatile bool* send_ping, volatile bool* received_resp) {
    // Make sure this is true so the CAN TX callback knows which subsystem to ping
    *send_ping = true;
    // Make sure this is false
    *received_resp = false;

    resume_mob(mob);
    wait_for_hb_mob_not_paused(mob);
    *send_ping = false;
    
    if (!wait_for_hb_resp(received_resp)) {
        send_hb_reset(other_id);
    }

    // Set flags to false just in case
    *send_ping = false;
    *received_resp = false;
}

bool send_hb_reset(uint8_t other_id) {
    pin_info_t rst_pin;

    if        (hb_self_id == HB_OBC && other_id == HB_EPS) {
        rst_pin = obc_rst_eps;
    } else if (hb_self_id == HB_OBC && other_id == HB_PAY) {
        rst_pin = obc_rst_pay;
    } else if (hb_self_id == HB_EPS && other_id == HB_OBC) {
        rst_pin = eps_rst_obc;
    } else if (hb_self_id == HB_EPS && other_id == HB_PAY) {
        rst_pin = eps_rst_pay;
    } else if (hb_self_id == HB_PAY && other_id == HB_OBC) {
        rst_pin = pay_rst_obc;
    } else if (hb_self_id == HB_PAY && other_id == HB_EPS) {
        rst_pin = pay_rst_eps;
    } else {
        return false;
    }

    // Assert the reset
    // TODO - how long?
    // See table on p.96 - for reset, need to output low (DDR = 1, PORT = 0)
    // Then go back to tri-state input with pullup (DDR = 0, PORT = 1)
    init_output_pin(rst_pin.pin, rst_pin.ddr, 0);
    _delay_ms(100);
    init_input_pin(rst_pin.pin, rst_pin.ddr);
    set_pin_pullup(rst_pin.pin, rst_pin.port, 1);

    return true;
}

// This should be run in the main loop
// because we can't interrupt inside of an interrupt, etc.
void run_hb(void) {
    // Only run up to one thing at a time, should go back through main loop quickly and prevent WDT timeouts

    // Check if we need to respond to a ping first so we don't get reset
    if (hb_send_obc_resp) {
        send_hb_resp(&obc_hb_mob, &hb_send_obc_resp);
    }
    else if (hb_send_eps_resp) {
        send_hb_resp(&eps_hb_mob, &hb_send_eps_resp);
    }
    else if (hb_send_pay_resp) {
        send_hb_resp(&pay_hb_mob, &hb_send_pay_resp);
    }

    else if (hb_send_obc_ping) {
        send_hb_ping(&obc_hb_mob, HB_OBC, &hb_send_obc_ping, &hb_received_obc_resp);
    }
    else if (hb_send_eps_ping) {
        send_hb_ping(&eps_hb_mob, HB_EPS, &hb_send_eps_ping, &hb_received_eps_resp);
    }
    else if (hb_send_pay_ping) {
        send_hb_ping(&pay_hb_mob, HB_PAY, &hb_send_pay_ping, &hb_received_pay_resp);
    }
}









/*Status CAN Message Format. Follow PAY's format*/
/*Assume 8 bytes: (0)sending SSM (1) receiving SSM (2) message type (2 for
heartbeat) (3) obc status (4) eps status (5) pay status (6) time stamp */

// EEPROM address assignment to store status of each SSM
// EEPROM address starts from 0x0000. Use const uint16_t to declare the address
// value and type casted when using eeprom functions.

// TODO: #define these instead?
// const uint16_t INIT_WORD_EEMEM  = 0x0000; //4 bytes
// const uint16_t OBC_STATUS_EEMEM = 0x0004; //1 byte
// const uint16_t EPS_STATUS_EEMEM = 0x0005; //1 byte
// const uint16_t PAY_STATUS_EEMEM = 0x0006; //1 byte

// Pre-declare and define SELF_STATUS_EEMEM
// Will be redefined based on SSM ids in line 55-82
// uint16_t SELF_STATUS_EEMEM = 0x0004;

// Pre-define SSM status gloabl variables and pointers. Will be defined
// throughout program code as the mission progresses
// uint8_t obc_status = 0x00;
// uint8_t eps_status = 0x00;
// uint8_t pay_status = 0x00;

// uint8_t* self_status = 0x00;
// uint8_t* parent_status = 0x00;
// uint8_t* child_status = 0x00;

// hb_self_id will be defined in the SSM main program
// obc {0x00} eps {0x02} pay {0x01}
// (i.e. heartbeat_test.c in the example folder)
// uint8_t hb_self_id; // Will be changed by each SSM

// uint8_t receiving_id = 0xff;

// Pre-define fresh_start. Will be re-defined as the mission progresses
// uint8_t fresh_start = 1; // 1 is true. 0 is false


// void rx_callback(const uint8_t*, uint8_t);
// void tx_callback(uint8_t*, uint8_t*);

// /*Status CAN Message Format. Follow PAY's format*/
// /*Assume 8 bytes: (0)sending SSM (1) receiving SSM (2) message type (2 for
// heartbeat) (3) obc status (4) eps status (5) pay status (6) time stamp */
// mob_t status_rx_mob = {
//     .mob_num = 0,
//     .mob_type = RX_MOB,
//     .dlc = 8,
//     .id_tag = {  }, // ID of this nodes parent
//     .id_mask = { 0x00f },
//     .ctrl = default_rx_ctrl,
//     .rx_cb = rx_callback
// };

// mob_t status_tx_mob = {
//     .mob_num = 1,
//     .mob_type = TX_MOB,
//     .id_tag = { }, // ID of this nodes child
//     .ctrl = default_tx_ctrl,
//     .tx_data_cb = tx_callback
// };

// void init_heartbeat() {
//     assign_heartbeat_status();
//     assign_status_message_objects();

//     // Initialize CAN MObs
//     init_rx_mob(&status_rx_mob);
//     init_tx_mob(&status_tx_mob);

//     // Identity whether SSM is fresh start or restart
//     if (eeprom_read_dword((uint32_t*) INIT_WORD_EEMEM) != DEADBEEF) {
//         print("SSM FRESH START\n");
//         eeprom_update_dword((uint32_t*) INIT_WORD_EEMEM, DEADBEEF);
//         fresh_start = 0;
//     } else {
//         // Retrieve most recent status prior to restart from EEPROM
//         print("SSM RESTART -> RETRIEVE STATUS\n");
//         switch (hb_self_id) {
//             case HB_OBC:
//                 *self_status = eeprom_read_byte((uint8_t*) OBC_STATUS_EEMEM);
//                 *parent_status = eeprom_read_byte((uint8_t*) EPS_STATUS_EEMEM);
//                 *child_status = eeprom_read_byte((uint8_t*) PAY_STATUS_EEMEM);
//                 break;
//             case HB_EPS:
//                 *self_status = eeprom_read_byte((uint8_t*) EPS_STATUS_EEMEM);
//                 *parent_status = eeprom_read_byte((uint8_t*) PAY_STATUS_EEMEM);
//                 *child_status = eeprom_read_byte((uint8_t*) OBC_STATUS_EEMEM);
//                 break;
//             case HB_PAY:
//                 *self_status = eeprom_read_byte((uint8_t*) PAY_STATUS_EEMEM);
//                 *parent_status = eeprom_read_byte((uint8_t*) OBC_STATUS_EEMEM);
//                 *child_status = eeprom_read_byte((uint8_t*) EPS_STATUS_EEMEM);
//                 break;
//             default:
//                 print("INVALID SSM ID. STATUS NOT RETRIEVED\n");
//                 break;
//         }

//         fresh_start = 0;
//         // Redundant, but reinforce the restart condition!
//         // In case of bit flipping
//     }
// }

// // Implement heartbeat functions
// void assign_heartbeat_status(void) {
//     switch (hb_self_id) {
//         case HB_OBC:
//             SELF_STATUS_EEMEM = OBC_STATUS_EEMEM;
//             self_status = &obc_status;
//             parent_status = &eps_status;
//             child_status = &pay_status;
//             receiving_id = 0x02;
//             break;
//         case HB_EPS:
//             SELF_STATUS_EEMEM = EPS_STATUS_EEMEM;
//             self_status = &eps_status;
//             parent_status = &pay_status;
//             child_status = &obc_status;
//             receiving_id = 0x01;
//             break;
//         case HB_PAY:
//             SELF_STATUS_EEMEM = PAY_STATUS_EEMEM;
//             self_status = &pay_status;
//             parent_status = &obc_status;
//             child_status = &eps_status;
//             receiving_id = 0x00;
//             break;
//         default:
//             print("INVALID SSM ID");
//             break;
//     }
// }

// void assign_status_message_objects(void) {
//     switch (hb_self_id) {
//         case HB_OBC:
//             status_rx_mob.mob_num = 1;
//             status_rx_mob.id_tag.std = OBC_STATUS_RX_MOB_ID;
//             status_tx_mob.mob_num = 0;
//             status_tx_mob.id_tag.std = OBC_STATUS_TX_MOB_ID;
//             break;
//         case HB_EPS:
//             status_rx_mob.mob_num = 0;
//             status_rx_mob.id_tag.std = EPS_STATUS_RX_MOB_ID;
//             status_tx_mob.mob_num = 1;
//             status_tx_mob.id_tag.std = EPS_STATUS_TX_MOB_ID;
//             break;
//         case HB_PAY:
//             status_rx_mob.mob_num = 0;
//             status_rx_mob.id_tag.std = PAY_STATUS_RX_MOB_ID;
//             status_tx_mob.mob_num = 1;
//             status_tx_mob.id_tag.std = PAY_STATUS_TX_MOB_ID;
//             break;
//         default:
//             print("INVALID SSM ID");
//             break;
//     }
// }


// void tx_callback(uint8_t* data, uint8_t* len) {
//     // Update its own EEPROM status first before sending a CAN message to parent
//     eeprom_update_byte((uint8_t*) SELF_STATUS_EEMEM, *self_status);
//     // Set up CAN message data to be sent to parent
//     *len = 8;
//     data[0] = hb_self_id;
//     data[1] = receiving_id;
//     data[2] = 2;
//     data[3] = obc_status;
//     data[4] = eps_status;
//     data[5] = pay_status;
//     print("Status updated and sent to parent\n");
// }

// void rx_callback(const uint8_t* data, uint8_t len) {
//     print("Received status from child\n");
//     if (len != 0) {
//         // Update the status global variables
//         obc_status = data[3];
//         eps_status = data[4];
//         pay_status = data[5];
//         // Save updated data into EEPROM
//         eeprom_update_byte((uint8_t*) OBC_STATUS_EEMEM, data[3]);
//         eeprom_update_byte((uint8_t*) EPS_STATUS_EEMEM, data[4]);
//         eeprom_update_byte((uint8_t*) PAY_STATUS_EEMEM, data[5]);
//         print("Updated status in EEPROM\n");
//         print("obc %d, pay %d, eps %d\n\n", obc_status, pay_status, eps_status);
//     } else {
//         print("Status receiving error! No data!\n");
//     }
// }

// void heartbeat() {
//     resume_mob(&status_tx_mob);
// }
