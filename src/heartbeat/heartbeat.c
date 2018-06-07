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
*/

// Assume init_uart() and init_can() have been called in SSM main program
#include <avr/eeprom.h>
#include <can/can.h>
#include <uart/log.h>
#include <heartbeat/heartbeat.h>

// EEPROM address assignment to store status of each SSM
// EEPROM address starts from 0x0000. Use const uint16_t to declare the address
// value and type casted when using eeprom functions.

// TODO: #define these instead?
const uint16_t INIT_WORD_EEMEM  = 0x0000; //4 bytes
const uint16_t OBC_STATUS_EEMEM = 0x0004; //1 byte
const uint16_t EPS_STATUS_EEMEM = 0x0005; //1 byte
const uint16_t PAY_STATUS_EEMEM = 0x0006; //1 byte

// Pre-declare and define SELF_STATUS_EEMEM
// Will be redefined based on SSM ids in line 55-82
uint16_t SELF_STATUS_EEMEM = 0x0004;

// Pre-define SSM status gloabl variables and pointers. Will be defined
// throughout program code as the mission progresses
uint8_t obc_status = 0x00;
uint8_t eps_status = 0x00;
uint8_t pay_status = 0x00;

uint8_t* self_status = 0x00;
uint8_t* parent_status = 0x00;
uint8_t* child_status = 0x00;

// ssm_id will be defined in the SSM main program
// obc {0x00} eps {0x02} pay {0x01}
// (i.e. heartbeat_test.c in the example folder)
extern uint8_t ssm_id; // Will be changed by each SSM
uint8_t receiving_id = 0xff;

// Pre-define fresh_start. Will be re-defined as the mission progresses
uint8_t fresh_start = 1; // 1 is true. 0 is false


void rx_callback(const uint8_t*, uint8_t);
void tx_callback(uint8_t*, uint8_t*);

/*Status CAN Message Format. Follow PAY's format*/
/*Assume 8 bytes: (0)sending SSM (1) receiving SSM (2) message type (2 for
heartbeat) (3) obc status (4) eps status (5) pay status (6) time stamp */
mob_t status_rx_mob = {
    .mob_num = 0,
    .mob_type = RX_MOB,
    .dlc = 8,
    .id_tag = {  }, // ID of this nodes parent
    .id_mask = { 0x00f },
    .ctrl = default_rx_ctrl,
    .rx_cb = rx_callback
};

mob_t status_tx_mob = {
    .mob_num = 1,
    .mob_type = TX_MOB,
    .id_tag = { }, // ID of this nodes child
    .ctrl = default_tx_ctrl,
    .tx_data_cb = tx_callback
};

// Implement heartbeat functions
void assign_heartbeat_status() {
    switch(ssm_id) {
        case OBC:
            SELF_STATUS_EEMEM = OBC_STATUS_EEMEM;
            self_status = &obc_status;
            parent_status = &eps_status;
            child_status = &pay_status;
            receiving_id = 0x02;
            break;
        case EPS:
            SELF_STATUS_EEMEM = EPS_STATUS_EEMEM;
            self_status = &eps_status;
            parent_status = &pay_status;
            child_status = &obc_status;
            receiving_id = 0x01;
            break;
        case PAY:
            SELF_STATUS_EEMEM = PAY_STATUS_EEMEM;
            self_status = &pay_status;
            parent_status = &obc_status;
            child_status = &eps_status;
            receiving_id = 0x00;
            break;
        default:
            print("INVALID SSM ID");
            break;
    }
}

void assign_status_message_objects() {
    switch(ssm_id) {
        case OBC:
            status_rx_mob.mob_num = 1;
            status_rx_mob.id_tag.std = OBC_STATUS_RX_MOB_ID;
            status_tx_mob.mob_num = 0;
            status_tx_mob.id_tag.std = OBC_STATUS_TX_MOB_ID;
            break;
        case EPS:
            status_rx_mob.mob_num = 0;
            status_rx_mob.id_tag.std = EPS_STATUS_RX_MOB_ID;
            status_tx_mob.mob_num = 1;
            status_tx_mob.id_tag.std = EPS_STATUS_TX_MOB_ID;
            break;
        case PAY:
            status_rx_mob.mob_num = 0;
            status_rx_mob.id_tag.std = PAY_STATUS_RX_MOB_ID;
            status_tx_mob.mob_num = 1;
            status_tx_mob.id_tag.std = PAY_STATUS_TX_MOB_ID;
            break;
        default:
            print("INVALID SSM ID");
            break;
    }
}

void init_heartbeat() {
    assign_heartbeat_status();
    assign_status_message_objects();

    // Initialize CAN MObs
    init_rx_mob(&status_rx_mob);
    init_tx_mob(&status_tx_mob);

    // Identity whether SSM is fresh start or restart
    if (eeprom_read_dword((uint32_t*) INIT_WORD_EEMEM) != DEADBEEF) {
        print("SSM FRESH START\n");
        eeprom_update_dword((uint32_t*) INIT_WORD_EEMEM, DEADBEEF);
        fresh_start = 0;
    } else {
        // Retrieve most recent status prior to restart from EEPROM
        print("SSM RESTART -> RETRIEVE STATUS\n");
        switch (ssm_id) {
            case OBC:
                *self_status = eeprom_read_byte((uint8_t*) OBC_STATUS_EEMEM);
                *parent_status = eeprom_read_byte((uint8_t*) EPS_STATUS_EEMEM);
                *child_status = eeprom_read_byte((uint8_t*) PAY_STATUS_EEMEM);
                break;
            case EPS:
                *self_status = eeprom_read_byte((uint8_t*) EPS_STATUS_EEMEM);
                *parent_status = eeprom_read_byte((uint8_t*) PAY_STATUS_EEMEM);
                *child_status = eeprom_read_byte((uint8_t*) OBC_STATUS_EEMEM);
                break;
            case PAY:
                *self_status = eeprom_read_byte((uint8_t*) PAY_STATUS_EEMEM);
                *parent_status = eeprom_read_byte((uint8_t*) OBC_STATUS_EEMEM);
                *child_status = eeprom_read_byte((uint8_t*) EPS_STATUS_EEMEM);
                break;
            default:
                print("INVALID SSM ID. STATUS NOT RETRIEVED\n");
                break;
        }

        fresh_start = 0;
        // Redundant, but reinforce the restart condition!
        // In case of bit flipping
    }
}

void tx_callback(uint8_t* data, uint8_t* len) {
    // Update its own EEPROM status first before sending a CAN message to parent
    eeprom_update_byte((uint8_t*) SELF_STATUS_EEMEM, *self_status);
    // Set up CAN message data to be sent to parent
    *len = 8;
    data[0] = ssm_id;
    data[1] = receiving_id;
    data[2] = 2;
    data[3] = obc_status;
    data[4] = eps_status;
    data[5] = pay_status;
    print("Status updated and sent to parent\n");
}

void rx_callback(const uint8_t* data, uint8_t len) {
    print("Received status from child\n");
    if (len != 0) {
        // Update the status global variables
        obc_status = data[3];
        eps_status = data[4];
        pay_status = data[5];
        // Save updated data into EEPROM
        eeprom_update_byte((uint8_t*) OBC_STATUS_EEMEM, data[3]);
        eeprom_update_byte((uint8_t*) EPS_STATUS_EEMEM, data[4]);
        eeprom_update_byte((uint8_t*) PAY_STATUS_EEMEM, data[5]);
        print("Updated status in EEPROM\n");
        print("obc %d, pay %d, eps %d\n\n", obc_status, pay_status, eps_status);
    } else {
        print("Status receiving error! No data!\n");
    }
}

void heartbeat() {
    resume_mob(&status_tx_mob);
}
