/*
CAN Message Format:
Byte 0: 1 (ping) or 2 (response)
Byte 1: Sender
Byte 2: Receiver

TODO - test what happens when sending/receiving 2 messages at the same time from different subsystems
TODO - figure out better testing modes (e.g. not responding to all HB pings, randomly responding to some pings but not others)
*/

#include <avr/eeprom.h>

#include <uart/uart.h>
#include <heartbeat/heartbeat.h>
#include <utilities/utilities.h>

// Extra debugging logs
// #define HB_DEBUG


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

    init_rx_mob(mob);
}

void init_hb_tx_mob(mob_t* mob, uint8_t mob_num, uint16_t id_tag) {
    mob->mob_num = mob_num;
    mob->mob_type = TX_MOB;
    mob->id_tag.std = id_tag;
    mob->ctrl = hb_tx_ctrl;
    mob->tx_data_cb = hb_tx_cb;

    init_tx_mob(mob);
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
    }
    
    print("HB TX: ");
    print_bytes(data, *len);
}

// This function will be called within an ISR when we receive a message
void hb_rx_cb(const uint8_t* data, uint8_t len) {
    print("HB RX: ");
    print_bytes((uint8_t*) data, len);

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
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
#ifdef HB_DEBUG
    print("%s\n", __FUNCTION__);
#endif

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
    // Wait up to 10 ms
    for (uint16_t i = 0; i < 10; i++) {
        if (is_paused(mob)) {
            return true;
        }
        _delay_ms(1);
    }
    return false;
}

bool wait_for_hb_resp(volatile bool* received_resp) {
    // Wait up to 1 s
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
    
    bool success = wait_for_hb_resp(received_resp);
    print("Heartbeat to #%u - ", other_id);
    if (success) {
        print("success\n");
    } else {
        print("failed\n");
        send_hb_reset(other_id);
    }

    // Set flags to false just in case
    *send_ping = false;
    *received_resp = false;
}

bool send_hb_reset(uint8_t other_id) {
#ifdef HB_DEBUG
    print("%s: other_id = %u\n", __FUNCTION__, other_id);
#endif
    print("Sending HB reset to #%u\n", other_id);

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
    // See table on p.96 - for reset, need to output low (DDR = 1, PORT = 0)
    // Then go back to tri-state input with pullup (DDR = 0, PORT = 1)
    init_output_pin(rst_pin.pin, rst_pin.ddr, 0);
    _delay_ms(10);
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
