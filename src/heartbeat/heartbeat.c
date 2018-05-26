#include <heartbeat/heartbeat.h>

extern uint8_t obc_status; //global variables to store SSM status
extern uint8_t eps_status;
extern uint8_t pay_status;

uint8_t* self_status = 0x00;
uint8_t* parent_status = 0x00;
uint8_t* child_status = 0x00;

extern uint8_t ssm_id; //will be changed by each SSM
//obc {0b00} eps {0b10} pay {0b01}

bool fresh_start = true;

void assign_heartbeat_status() {
    switch(ssm_id){
        case 0x00:
            self_status = &obc_status;
            parent_status = &eps_status;
            child_status = &pay_status;
            break;
        case 0x02:
            self_status = &eps_status;
            parent_status = &pay_status;
            child_status = &obc_status;
            break;
        case 0x01:
            self_status = &pay_status;
            parent_status = &obc_status;
            child_status = &eps_status;
            break;
        default:
            print("INVALID SSM ID\n");
            break;
    }
}

void init_heartbeat() {
    //assign_heartbeat_status();
    assign_heartbeat_status();
    if (eeprom_read_dword((uint32_t*) INIT_WORD_EEMEM) != DEADBEEF) {
        print("SSM FRESH START\n");
        eeprom_update_dword((uint32_t*) INIT_WORD_EEMEM, DEADBEEF);
    }
}
