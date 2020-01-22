/*
Updated CAN Heartbeat Protocol
Byte 0: HB_SENDER
Byte 1: HB_RECEIVER
Byte 2: HB_OPCODE (1 = ping request, 2 = ping response)
Byte 3: Restart Reason (if HB_OPCODE = 2)(See data conversion protocol)
Byte 4-7: Restart Count (if HB_OPCODE = 2)

TODO - test with flight model PAY with proper reset hardware
*/

#include <avr/eeprom.h>

#include <uart/uart.h>
#include <heartbeat/heartbeat.h>
#include <utilities/utilities.h>
#include <uptime/uptime.h>

// Extra debugging logs
// #define HB_DEBUG
// #define HB_VERBOSE


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


// Must all be volatile because they are modified inside CAN TX/RX interrupts
// Don't initialize mobs here
volatile hb_dev_t obc_hb_dev = {
    .name = "OBC",
    .id = HB_OBC,
    .ping_in_progress = false,
    .send_req_flag = false,
    .rcvd_resp_flag = false,
    .send_resp_flag = false,
    .reset = NULL,
    .ping_start_uptime_s = 0,
    .restart_reason = 0,
    .restart_count = 0,
};
volatile hb_dev_t eps_hb_dev = {
    .name = "EPS",
    .id = HB_EPS,
    .ping_in_progress = false,
    .send_req_flag = false,
    .rcvd_resp_flag = false,
    .send_resp_flag = false,
    .reset = NULL,
    .ping_start_uptime_s = 0,
    .restart_reason = 0,
    .restart_count = 0,
};
volatile hb_dev_t pay_hb_dev = {
    .name = "PAY",
    .id = HB_PAY,
    .ping_in_progress = false,
    .send_req_flag = false,
    .rcvd_resp_flag = false,
    .send_resp_flag = false,
    .reset = NULL,
    .ping_start_uptime_s = 0,
    .restart_reason = 0,
    .restart_count = 0,
};

volatile hb_dev_t* all_hb_devs[HB_NUM_DEVS] = {
    &obc_hb_dev,
    &eps_hb_dev,
    &pay_hb_dev,
};

// Assume OBC just in case
volatile hb_dev_t* self_hb_dev = &obc_hb_dev;

// Last recorded uptime when we sent a ping
volatile uint32_t hb_req_prev_uptime_s = 0;
// How long to count before sending a heartbeat ping to both other subsystems
volatile uint32_t hb_req_period_s = HB_REQ_PERIOD_S;
volatile uint32_t hb_resp_wait_time_s = HB_RESP_WAIT_TIME_S;


void init_hb_resets(void);
void init_hb_rx_mob(mob_t* mob, uint8_t mob_num, uint16_t id_tag);
void init_hb_tx_mob(mob_t* mob, uint8_t mob_num, uint16_t id_tag);
void init_hb_mobs(void);
void hb_tx_cb(uint8_t* data, uint8_t* len);
void hb_rx_cb(const uint8_t* data, uint8_t len);


// Assumes init_uptime() and init_can() have already been called,
// but init_rx_mob() and init_tx_mob() have NOT been called on the HB MOBs
void init_hb(uint8_t self_id) {
    for (uint8_t i = 0; i < HB_NUM_DEVS; i++) {
        if (all_hb_devs[i]->id == self_id) {
            self_hb_dev = all_hb_devs[i];
        }
    }

    init_hb_resets();
    init_hb_mobs();
}

void init_hb_resets(void) {
    // Set self heartbeat reset just in case, but it should never be triggered
    switch (self_hb_dev->id) {
        case HB_OBC:
            obc_hb_dev.reset = &obc_rst_pay;
            pay_hb_dev.reset = &obc_rst_pay;
            eps_hb_dev.reset = &obc_rst_eps;
            break;
        case HB_PAY:
            obc_hb_dev.reset = &pay_rst_obc;
            pay_hb_dev.reset = &pay_rst_obc;
            eps_hb_dev.reset = &pay_rst_eps;
            break;
        case HB_EPS:
            obc_hb_dev.reset = &eps_rst_obc;
            pay_hb_dev.reset = &eps_rst_pay;
            eps_hb_dev.reset = &eps_rst_pay;
            break;
        default:
#ifdef HB_DEBUG
            print("Error: %s\n", __FUNCTION__);
#endif
            return;
    }

    for (uint8_t i = 0; i < HB_NUM_DEVS; i++) {
        hb_dev_t* dev = (hb_dev_t*) all_hb_devs[i];

        if (dev != self_hb_dev) {
            // See table on p.96 - by default, need tri-state input with pullup (DDR = 0, PORT = 1)
            init_input_pin(dev->reset->pin, dev->reset->ddr);
            set_pin_pullup(dev->reset->pin, dev->reset->port, 1);
        }
    }
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
    switch (self_hb_dev->id) {
        case HB_OBC:
            init_hb_rx_mob((mob_t*) &obc_hb_dev.mob, OBC_HB_MOB_NUM, OBC_OBC_HB_RX_MOB_ID);
            init_hb_tx_mob((mob_t*) &pay_hb_dev.mob, PAY_HB_MOB_NUM, OBC_PAY_HB_TX_MOB_ID);
            init_hb_tx_mob((mob_t*) &eps_hb_dev.mob, EPS_HB_MOB_NUM, OBC_EPS_HB_TX_MOB_ID);
            break;
        case HB_PAY:
            init_hb_tx_mob((mob_t*) &obc_hb_dev.mob, OBC_HB_MOB_NUM, PAY_OBC_HB_TX_MOB_ID);
            init_hb_rx_mob((mob_t*) &pay_hb_dev.mob, PAY_HB_MOB_NUM, PAY_PAY_HB_RX_MOB_ID);
            init_hb_tx_mob((mob_t*) &eps_hb_dev.mob, EPS_HB_MOB_NUM, PAY_EPS_HB_TX_MOB_ID);
            break;
        case HB_EPS:
            init_hb_tx_mob((mob_t*) &obc_hb_dev.mob, OBC_HB_MOB_NUM, EPS_OBC_HB_TX_MOB_ID);
            init_hb_tx_mob((mob_t*) &pay_hb_dev.mob, PAY_HB_MOB_NUM, EPS_PAY_HB_TX_MOB_ID);
            init_hb_rx_mob((mob_t*) &eps_hb_dev.mob, EPS_HB_MOB_NUM, EPS_EPS_HB_RX_MOB_ID);
            break;
        default:
 #ifdef HB_DEBUG
            print("Error: %s\n", __FUNCTION__);
#endif
            break;
    }
}


void hb_tx_cb(uint8_t* data, uint8_t* len) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        // Set up CAN message data to be sent
        *len = 8;
        for (uint8_t i = 0; i < *len; i++){
            data[i] = 0x00;
        }

        // Must return after setting message contents to make sure the data
        // bytes are not overwritten by another message that needs to be sent

        // Send ping response
        for (uint8_t i = 0; i < HB_NUM_DEVS; i++) {
            hb_dev_t* dev = (hb_dev_t*) all_hb_devs[i];

            if (dev != self_hb_dev && dev->send_resp_flag) {
                dev->send_resp_flag = false;

                data[HB_SENDER] = self_hb_dev->id;
                data[HB_RECEIVER] = dev->id;
                data[HB_OPCODE] = HB_PING_RESPONSE;
                data[HB_RESTART_REASON] = restart_reason;
                data[HB_RESTART_COUNT+0] = (restart_count >> 24) & 0xFF;
                data[HB_RESTART_COUNT+1] = (restart_count >> 16) & 0xFF;
                data[HB_RESTART_COUNT+2] = (restart_count >> 8) & 0xFF;
                data[HB_RESTART_COUNT+3] = (restart_count & 0xFF);

                print("HB TX: ");
                print_bytes(data, *len);
                return;
            }
        }

        // Send ping request
        for (uint8_t i = 0; i < HB_NUM_DEVS; i++) {
            hb_dev_t* dev = (hb_dev_t*) all_hb_devs[i];

            if (dev != self_hb_dev && dev->send_req_flag) {
                dev->send_req_flag = false;

                data[HB_SENDER] = self_hb_dev->id;
                data[HB_RECEIVER] = dev->id;
                data[HB_OPCODE] = HB_PING_REQUEST;

                print("HB TX: ");
                print_bytes(data, *len);
                return;
            }
        }
    }

    print("HB TX: ");
    print_bytes(data, *len);

#ifdef HB_DEBUG
    print("Error: %s\n", __FUNCTION__);
#endif
}

// This function will be called within an ISR when we receive a message
void hb_rx_cb(const uint8_t* data, uint8_t len) {
    print("HB RX: ");
    print_bytes((uint8_t*) data, len);

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if (len != 8) {
            return;
        }
        if (data[HB_RECEIVER] != self_hb_dev->id) {
            return;
        }

        // Ping Request received
        if (data[HB_OPCODE] == HB_PING_REQUEST) {
            for (uint8_t i = 0; i < HB_NUM_DEVS; i++) {
                hb_dev_t* dev = (hb_dev_t*) all_hb_devs[i];
                if (data[HB_SENDER] == dev->id) {
                    dev->send_resp_flag = true;
                    return;
                }
            }
        }

        // Ping Response received
        if (data[HB_OPCODE] == HB_PING_RESPONSE) {
            uint8_t reason = data[HB_RESTART_REASON];
            uint32_t count =
                ((uint32_t) data[HB_RESTART_COUNT+0] << 24) |
                ((uint32_t) data[HB_RESTART_COUNT+1] << 16) |
                ((uint32_t) data[HB_RESTART_COUNT+2] << 8) |
                ((uint32_t) data[HB_RESTART_COUNT+3]);

            for (uint8_t i = 0; i < HB_NUM_DEVS; i++) {
                hb_dev_t* dev = (hb_dev_t*) all_hb_devs[i];
                if (data[HB_SENDER] == dev->id) {
                    dev->rcvd_resp_flag = true;
                    dev->restart_reason = reason;
                    dev->restart_count = count;
                    return;
                }
            }
        }
    }

#ifdef HB_DEBUG
    print("Error: %s\n", __FUNCTION__);
#endif
}

bool wait_for_hb_mob_not_paused(mob_t* mob) {
    // Wait up to 5 ms
    for (uint16_t i = 0; i < 5; i++) {
        if (is_paused(mob)) {
            return true;
        }
        _delay_ms(1);
    }

#ifdef HB_DEBUG
    print("Error: %s\n", __FUNCTION__);
#endif
    return false;
}

bool send_hb_reset(hb_dev_t* device) {
    print("HB reset to %u (%s)\n", device->id, device->name);

    // Assert the reset
    // See table on p.96 - for reset, need to output low (DDR = 1, PORT = 0)
    // Then go back to tri-state input with pullup (DDR = 0, PORT = 1)
    init_output_pin(device->reset->pin, device->reset->ddr, 0);
    _delay_ms(10);
    init_input_pin(device->reset->pin, device->reset->ddr);
    set_pin_pullup(device->reset->pin, device->reset->port, 1);

    return true;
}

// This should be run in the main loop
// because we can't interrupt inside of an interrupt, etc.
void run_hb(void) {
#ifdef HB_VERBOSE
    print("%s\n", __FUNCTION__);
#endif

    // If we are currently in the process of sending a CAN message, wait until
    // all TX MOBs are paused before possibly trying to send another message
    // Don't check self (RX MOB) because it is always in receive mode and never
    // paused
    // This must be outside the atomic block because the MOB is only unpaused
    // (set CONMOB[1:0] = 0b00) by our CAN library when a TX interrupt occurs
    // (after it actually sends a message)
    for (uint8_t i = 0; i < HB_NUM_DEVS; i++) {
        hb_dev_t* dev = (hb_dev_t*) all_hb_devs[i];        
        if (dev != self_hb_dev) {
            wait_for_hb_mob_not_paused(&dev->mob);
        }
    }

    // Do all this logic in an atomic block because the structs and flags could
    // be modified by CAN RX interrupts
    // Note if we send a CAN message we return from the ISR because we can only
    // load one message at a time
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        // Send response - check if we need to respond to a request first so we
        // don't get reset
        for (uint8_t i = 0; i < HB_NUM_DEVS; i++) {
            hb_dev_t* dev = (hb_dev_t*) all_hb_devs[i];

            if ((dev != self_hb_dev) && dev->send_resp_flag) {
#ifdef HB_DEBUG
                print("HB resp to %u (%s)\n", dev->id, dev->name);
#endif

                resume_mob(&dev->mob);

                // Make sure to do this after resume_mob, which calls the TX
                // callback which checks this flag to be true
                dev->send_resp_flag = false;

                return;
            }
        }

        // Check for response - check that we received responses to any requests
        // we already sent out
        for (uint8_t i = 0; i < HB_NUM_DEVS; i++) {
            hb_dev_t* dev = (hb_dev_t*) all_hb_devs[i];

            if ((dev != self_hb_dev) && dev->ping_in_progress) {
                // If we received a response
                if (dev->rcvd_resp_flag) {
                    dev->ping_in_progress = false;
                    dev->send_req_flag = false;
                    dev->rcvd_resp_flag = false;

#ifdef HB_DEBUG
                    print("HB ping to %u (%s) - success\n", dev->id, dev->name);
#endif
                }
                
                // If the wait period has elapsed without receiving a response
                else if (uptime_s >= hb_req_prev_uptime_s + hb_resp_wait_time_s) {
                    dev->ping_in_progress = false;
                    dev->send_req_flag = false;
                    dev->rcvd_resp_flag = false;

#ifdef HB_DEBUG
                    print("HB ping to %u (%s) - fail\n", dev->id, dev->name);
#endif

                    send_hb_reset(dev);
                }
            }
        }

        // Set request flag - check if it is time to send another ping to both
        // other devices - just set the flags if so, but don't send yet
        if (uptime_s >= hb_req_prev_uptime_s + hb_req_period_s) {
            hb_req_prev_uptime_s = uptime_s;
            
            for (uint8_t i = 0; i < HB_NUM_DEVS; i++) {
                hb_dev_t* dev = (hb_dev_t*) all_hb_devs[i];

                if (dev != self_hb_dev) {
                    dev->ping_in_progress = false;
                    dev->send_req_flag = true;
                    dev->rcvd_resp_flag = false;
                }
            }
        }

        // Send request - if the flag is set, send a request to another device
        for (uint8_t i = 0; i < HB_NUM_DEVS; i++) {
            hb_dev_t* dev = (hb_dev_t*) all_hb_devs[i];

            if ((dev != self_hb_dev) && dev->send_req_flag) {
#ifdef HB_DEBUG
                print("HB req to %u (%s)\n", dev->id, dev->name);
#endif

                resume_mob(&dev->mob);

                dev->ping_in_progress = true;
                dev->send_req_flag = false;
                dev->rcvd_resp_flag = false;
                
                return;
            }
        }
    }
}
