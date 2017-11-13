#include <avr/io.h>
#include <avr/interrupt.h>

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
} mob_ctrl_t;

typedef void (*can_rx_callback_t)(uint8_t*, uint8_t);
typedef void (*can_tx_callback_t)(uint8_t*, uint8_t*);

typedef struct {
    uint8_t mob_num;
    uint8_t dlc;

    mob_id_tag_t id_tag;
    mob_id_mask_t id_mask;
    mob_ctrl_t ctrl;

    can_rx_callback_t rx_cb;
} rx_mob_t;

typedef struct {
    uint8_t mob_num;
    uint8_t dlc;

    mob_id_tag_t id_tag;
    mob_ctrl_t ctrl;

    uint8_t data[8];

    can_tx_callback_t tx_data_cb;
} tx_mob_t;

void init_can(void);

void init_rx_mob(rx_mob_t*);
void pause_rx_mob(rx_mob_t*);
void resume_rx_mob(rx_mob_t*);

void init_tx_mob(tx_mob_t*);
void pause_tx_mob(tx_mob_t*);
void resume_tx_mob(tx_mob_t*);

uint8_t rx_mob_status(rx_mob_t*);
uint8_t tx_mob_status(tx_mob_t*);

