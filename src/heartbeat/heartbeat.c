/*
Updated CAN Heartbeat Protocol
Byte 0: HB_SENDER
Byte 1: HB_RECEIVER
Byte 2: HB_OPCODE (1 = ping request, 2 = ping response)
Byte 3: Restart Reason (if HB_OPCODE = 2)(See data conversion protocol)
Byte 4-7: Restart Count (if HB_OPCODE = 2)

TODO - fix race conditions when 2 heartbeat pings are sent around the same time
TODO - figure out better testing modes (e.g. not responding to all HB pings, randomly responding to some pings but not others)
TODO - test with flight model PAY with proper reset hardware
TODO - Add proper constants for PAY, EPS, OBC
*/

#include <avr/eeprom.h>

#include <uart/uart.h>
#include <heartbeat/heartbeat.h>
#include <utilities/utilities.h>
#include <uptime/uptime.h>

// Extra debugging logs
// #define HB_DEBUG

// temporary constants
#define HB_OBC_PLACEHOLDER 0x00
#define HB_PAY_PLACEHOLDER 0x01
#define HB_EPS_PLACEHOLDER 0x02

// The current MCU's ID
uint8_t hb_self_id = 0xFF;  // None of the subsystems by default

// Reset reason
uint8_t hb_latest_restart_reason = 0x00;
// Reset count
uint32_t hb_latest_restart_count = 0x00;

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
volatile bool hb_send_obc_req = false;
volatile bool hb_send_eps_req = false;
volatile bool hb_send_pay_req = false;
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
        case HB_OBC_PLACEHOLDER:
            rst_pin_1 = obc_rst_eps;
            rst_pin_2 = obc_rst_pay;
            break;
        case HB_PAY_PLACEHOLDER:
            rst_pin_1 = pay_rst_obc;
            rst_pin_2 = pay_rst_eps;
            break;
        case HB_EPS_PLACEHOLDER:
            rst_pin_1 = eps_rst_obc;
            rst_pin_2 = eps_rst_pay;
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
mob_id_mask_t hb_id_mask = { CAN_RX_MASK_ID };
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
        case HB_OBC_PLACEHOLDER:
            init_hb_rx_mob(&obc_hb_mob, HB_OBC_PLACEHOLDER, OBC_OBC_HB_RX_MOB_ID);
            init_hb_tx_mob(&pay_hb_mob, HB_PAY_PLACEHOLDER, OBC_PAY_HB_TX_MOB_ID);
            init_hb_tx_mob(&eps_hb_mob, HB_EPS_PLACEHOLDER, OBC_EPS_HB_TX_MOB_ID);
            break;
        case HB_PAY_PLACEHOLDER:
            init_hb_tx_mob(&obc_hb_mob, HB_OBC_PLACEHOLDER, PAY_OBC_HB_TX_MOB_ID);
            init_hb_rx_mob(&pay_hb_mob, HB_PAY_PLACEHOLDER, PAY_PAY_HB_RX_MOB_ID);
            init_hb_tx_mob(&eps_hb_mob, HB_EPS_PLACEHOLDER, PAY_EPS_HB_TX_MOB_ID);
            break;
        case HB_EPS_PLACEHOLDER:
            init_hb_tx_mob(&obc_hb_mob, HB_OBC_PLACEHOLDER, EPS_OBC_HB_TX_MOB_ID);
            init_hb_tx_mob(&pay_hb_mob, HB_PAY_PLACEHOLDER, EPS_PAY_HB_TX_MOB_ID);
            init_hb_rx_mob(&eps_hb_mob, HB_EPS_PLACEHOLDER, EPS_EPS_HB_RX_MOB_ID);
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
        for (uint8_t i = 0; i < *len; i++){
            data[i] = 0x00;
        }

        if (hb_send_obc_req) {
            data[HB_SENDER] = hb_self_id;
            data[HB_RECEIVER] = HB_OBC_PLACEHOLDER;
            data[HB_OPCODE] = HB_PING_REQUEST;
        } else if (hb_send_pay_req) {
            data[HB_SENDER] = hb_self_id;
            data[HB_RECEIVER] = HB_PAY_PLACEHOLDER;
            data[HB_OPCODE] = HB_PING_REQUEST;
        } else if (hb_send_eps_req) {
            data[HB_SENDER] = hb_self_id;
            data[HB_RECEIVER] = HB_EPS_PLACEHOLDER;
            data[HB_OPCODE] = HB_PING_REQUEST;

        } else if (hb_send_obc_resp) {
            data[HB_SENDER] = hb_self_id;
            data[HB_RECEIVER] = HB_OBC_PLACEHOLDER;
            data[HB_OPCODE] = HB_PING_RESPONSE;
            data[HB_RESTART_REASON] = restart_reason;
            data[HB_RESTART_COUNT] = (restart_count >> 24) & 0xFF;
            data[HB_RESTART_COUNT+1] = (restart_count >> 16) & 0xFF;
            data[HB_RESTART_COUNT+2] = (restart_count >> 8) & 0xFF;
            data[HB_RESTART_COUNT+3] = (restart_count & 0xFF);
        } else if (hb_send_pay_resp) {
            data[HB_SENDER] = hb_self_id;
            data[HB_RECEIVER] = HB_PAY_PLACEHOLDER;
            data[HB_OPCODE] = HB_PING_RESPONSE;
            data[HB_RESTART_REASON] = restart_reason;
            data[HB_RESTART_COUNT] = (restart_count >> 24) & 0xFF;
            data[HB_RESTART_COUNT+1] = (restart_count >> 16) & 0xFF;
            data[HB_RESTART_COUNT+2] = (restart_count >> 8) & 0xFF;
            data[HB_RESTART_COUNT+3] = (restart_count & 0xFF);
        } else if (hb_send_eps_resp) {
            data[HB_SENDER] = hb_self_id;
            data[HB_RECEIVER] = HB_EPS_PLACEHOLDER;
            data[HB_OPCODE] = HB_PING_RESPONSE;
            data[HB_RESTART_REASON] = restart_reason;
            data[HB_RESTART_COUNT] = (restart_count >> 24) & 0xFF;
            data[HB_RESTART_COUNT+1] = (restart_count >> 16) & 0xFF;
            data[HB_RESTART_COUNT+2] = (restart_count >> 8) & 0xFF;
            data[HB_RESTART_COUNT+3] = (restart_count & 0xFF);
        } else {
            // Should not get here
            print("Error: Failed to execute hb tx callback\n");
        }
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
        if (data[HB_RECEIVER] != hb_self_id) {
            return;
        }

        // Ping Request received
        if (data[HB_OPCODE] == HB_PING_REQUEST) {
            switch (data[HB_SENDER]) {
                case HB_OBC_PLACEHOLDER:
                    hb_send_obc_resp = true;
                    break;
                case HB_PAY_PLACEHOLDER:
                    hb_send_pay_resp = true;
                    break;
                case HB_EPS_PLACEHOLDER:
                    hb_send_eps_resp = true;
                    break;
                default:
                    break;
            }
        // Ping Response received
        } else if (data[HB_OPCODE] == HB_PING_RESPONSE) {
            hb_latest_restart_reason = data[HB_RESTART_REASON];
            hb_latest_restart_count = ((uint32_t)data[HB_RESTART_COUNT] << 24)| ((uint32_t)data[HB_RESTART_COUNT+1] << 16)
                | ((uint32_t)data[HB_RESTART_COUNT+2] << 8) | ((uint32_t)data[HB_RESTART_COUNT+3]);

            switch (data[HB_SENDER]) {
                case HB_OBC_PLACEHOLDER:
                    hb_received_obc_resp = true;
                    break;
                case HB_PAY_PLACEHOLDER:
                    hb_received_pay_resp = true;
                    break;
                case HB_EPS_PLACEHOLDER:
                    hb_received_eps_resp = true;
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
    if (uptime_s >= hb_ping_prev_uptime_s + hb_ping_period_s) {
        if (hb_self_id != HB_OBC_PLACEHOLDER) {
            hb_send_obc_req = true;
        }
        if (hb_self_id != HB_PAY_PLACEHOLDER) {
            hb_send_pay_req = true;
        }
        if (hb_self_id != HB_EPS_PLACEHOLDER) {
            hb_send_eps_req = true;
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

    if (*send_ping != false){
        print("Error: send_ping is not false\n");
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

    if        (hb_self_id == HB_OBC_PLACEHOLDER && other_id == HB_EPS_PLACEHOLDER) {
        rst_pin = obc_rst_eps;
    } else if (hb_self_id == HB_OBC_PLACEHOLDER && other_id == HB_PAY_PLACEHOLDER) {
        rst_pin = obc_rst_pay;
    } else if (hb_self_id == HB_PAY_PLACEHOLDER && other_id == HB_OBC_PLACEHOLDER) {
        rst_pin = pay_rst_obc;
    } else if (hb_self_id == HB_PAY_PLACEHOLDER && other_id == HB_EPS_PLACEHOLDER) {
        rst_pin = pay_rst_eps;
    } else if (hb_self_id == HB_EPS_PLACEHOLDER && other_id == HB_OBC_PLACEHOLDER) {
        rst_pin = eps_rst_obc;
    } else if (hb_self_id == HB_EPS_PLACEHOLDER && other_id == HB_PAY_PLACEHOLDER) {
        rst_pin = eps_rst_pay;
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
    // Check is there is a ping that needs to be sent
    else if (hb_send_obc_req) {
        send_hb_ping(&obc_hb_mob, HB_OBC_PLACEHOLDER, &hb_send_obc_req, &hb_received_obc_resp);
    }
    else if (hb_send_pay_req) {
        send_hb_ping(&pay_hb_mob, HB_PAY_PLACEHOLDER, &hb_send_pay_req, &hb_received_pay_resp);
    }
    else if (hb_send_eps_req) {
        send_hb_ping(&eps_hb_mob, HB_EPS_PLACEHOLDER, &hb_send_eps_req, &hb_received_eps_resp);
    }

}
