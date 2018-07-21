#ifndef CAN_IDS_H
#define CAN_IDS_H

/*
DESCRIPTION: Defines global definitions for use with CAN
AUTHORS: Dylan Vogel, Ali Haydaroglu
*/

// GLOBAL RX MASK
#define CAN_RX_MASK_ID          { 0x07F8 }
// 0b0 111 1111 1000

/*
################################################################################
                         OBC
################################################################################
*/

// TODO fix 'X' hex digits
// Currently assumed X as 0

// OBC MOB IDs
#define OBC_STATUS_TX_MOB_ID    { 0x0008 }
// 0b0 XX0 0000 1000

#define OBC_STATUS_RX_MOB_ID    { 0x0011 }
// 0b0 000 0001 0XX1

#define OBC_CMD_RX_MOB_ID       { 0x0021 }
// 0b0 000 0010 0XX1

#define OBC_PAY_CMD_TX_MOB_ID   { 0x0240 }
// 0b0 010 0100 0000

#define OBC_EPS_CMD_TX_MOB_ID   { 0x0480 }
// 0b0 100 1000 0000

#define OBC_DATA_RX_MOB_ID      { 0x0101 }
// 0b0 001 0000 0XX1


/*
################################################################################
                         EPS
################################################################################
*/
// EPS MOB IDS
#define EPS_STATUS_RX_MOB_ID    { 0x0409 }
// 0b0 100 0000 1001
#define EPS_STATUS_TX_MOB_ID    { 0x0014 }
// 0b0 000 0001 0100
#define EPS_CMD_TX_MOB_ID       { 0x0024 }
// 0b0 000 0010 0100
#define EPS_CMD_RX_MOB_ID       { 0x0481 }
// 0b0 100 1000 0001
#define EPS_DATA_TX_MOB_ID      { 0x0104 }
// 0b0 001 0000 0100


/*
################################################################################
                         PAY
################################################################################
*/

// PAY MOB IDS
#define PAY_STATUS_RX_MOB_ID    { 0x0209 }    // RX MObs should have reciever ID of board
//0b 0 010 0000 1001
#define PAY_STATUS_TX_MOB_ID    { 0x0012 }
//0b 0 000 0001 0010
#define PAY_CMD_TX_MOB_ID       { 0x0022 }
// 0b 0 000 0010 0010
#define PAY_CMD_RX_MOB_ID       { 0x0241 }
// 0b 0 010 0100 0001
#define PAY_DATA_TX_MOB_ID      { 0x0102 }
// 0b 0 001 0000 0010

#endif
