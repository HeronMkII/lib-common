#include <avr/io.h>
#include <avr/interrupt.h>

// mob types
typedef enum {
    TX_MOB,
    RX_MOB,
    AUTO_MOB
} mob_type_t;

// allows access to the id via table
typedef union {
    uint16_t std;
    uint8_t  tab[2];
} mob_id_tag_t, mob_id_mask_t;

// struct to hold RTR, IDE, IDE Mask, RTR Mask and RBnTag bits;
// all boolean
typedef struct {
    uint8_t rtr; // 1 for remote frames, 0 for data frames
    uint8_t ide; // specifies CAN rev; should always be 0, for rev A
    uint8_t ide_mask; // masking bits for RX
    uint8_t rtr_mask; // masking bits for RX
    uint8_t rbn_tag; // masking bit for RX
    uint8_t rplv; // RPLV bit
} mob_ctrl_t;

// TODO: change these; ide_mask SHOULD matter
#define default_rx_ctrl { 0, 0, 0, 0, 0, 0 }
#define default_tx_ctrl { 0, 0, 0, 0, 0, 0 }

typedef void (*can_rx_callback_t)(uint8_t*, uint8_t);
typedef void (*can_tx_callback_t)(uint8_t*, uint8_t*);

typedef struct {
    // common
    uint8_t mob_num;
    uint8_t dlc;
    mob_id_tag_t id_tag;
    mob_ctrl_t ctrl;
    mob_type_t mob_type;

    // rx specific
    mob_id_mask_t id_mask;
    can_rx_callback_t rx_cb;

    // tx specific
    can_tx_callback_t tx_data_cb;
    uint8_t data[8];
} mob_t;

void init_can(void);

void init_rx_mob(mob_t*);
void init_tx_mob(mob_t*);
void init_auto_mob(mob_t*);

void pause_mob(mob_t*);
uint8_t is_paused(mob_t*);

void resume_mob(mob_t*);

uint8_t mob_status(mob_t*);
