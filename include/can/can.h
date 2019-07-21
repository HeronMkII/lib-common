#ifndef CAN_H
#define CAN_H

#include <avr/io.h>
#include <avr/interrupt.h>

// mob types
typedef enum {
    TX_MOB,
    RX_MOB,
} mob_type_t;

typedef enum {
    CAN_RATE_100,
    CAN_RATE_125,
    CAN_RATE_250,
    CAN_RATE_500,
    CAN_RATE_1000,
} can_baud_rate_t;

// Define the default baud rate as 100
#define CAN_DEF_BAUD_RATE CAN_RATE_100

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

// Default values for a `mob_ctrl_t` struct
// Don't care about ide_msk (identifier extension mask, see CANIDM/IDEMSK, p.269)
#define default_rx_ctrl { 0, 0, 0, 0, 0, 0 }
#define default_tx_ctrl { 0, 0, 0, 0, 0, 0 }

typedef void (*can_rx_callback_t)(const uint8_t*, uint8_t);
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

extern volatile uint8_t boffit_count;


void init_can(void);

void init_rx_mob(mob_t*);
void init_tx_mob(mob_t*);

void pause_mob(mob_t*);
void resume_mob(mob_t*);
uint8_t is_paused(mob_t*);

uint8_t mob_status(mob_t*);
void select_mob(uint8_t);

void set_can_baud_rate(can_baud_rate_t);

#endif
