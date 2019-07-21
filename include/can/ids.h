#ifndef CAN_IDS_H
#define CAN_IDS_H

/*
DESCRIPTION: Defines global definitions for use with CAN
AUTHORS: Dylan Vogel, Ali Haydaroglu
*/

// GLOBAL RX MASK
#define CAN_RX_MASK_ID          { 0x07F8 }
// 0b 111 1111 1000

// 'X' digits for sender IDs are don't cares (not in RX mask), but just use 0

// TODO - fix use of {}

/*
################################################################################
                         OBC
################################################################################
*/



// OBC MOB IDs
#define OBC_OBC_HB_RX_MOB_ID      0x0009
// 0b 000 0000 1XX1

#define OBC_PAY_HB_TX_MOB_ID      0x0210
// 0b 010 0001 0000

#define OBC_EPS_HB_TX_MOB_ID      0x0420
// 0b 100 0010 0000

#define OBC_PAY_CMD_TX_MOB_ID   { 0x0240 }
// 0b 010 0100 0000

#define OBC_EPS_CMD_TX_MOB_ID   { 0x0480 }
// 0b 100 1000 0000

#define OBC_DATA_RX_MOB_ID      { 0x0101 }
// 0b 001 0000 0XX1


/*
################################################################################
                         EPS
################################################################################
*/
// EPS MOB IDS
#define EPS_OBC_HB_TX_MOB_ID      0x000C
// 0b 000 0000 1100

#define EPS_PAY_HB_TX_MOB_ID      0x0214
// 0b 010 0001 0100

#define EPS_EPS_HB_RX_MOB_ID      0x0421
// 0b 100 0010 0XX1

#define EPS_CMD_RX_MOB_ID       { 0x0481 }
// 0b 100 1000 0001

#define EPS_DATA_TX_MOB_ID      { 0x0104 }
// 0b 001 0000 0100


/*
################################################################################
                         PAY
################################################################################
*/

// PAY MOB IDS
// RX MObs should have reciever ID of board
#define PAY_OBC_HB_TX_MOB_ID      0x000A
// 0b 000 0000 1010

#define PAY_PAY_HB_RX_MOB_ID      0x0211
// 0b 010 0001 0XX1

#define PAY_EPS_HB_TX_MOB_ID      0x0422
// 0b 100 0010 0010

#define PAY_CMD_RX_MOB_ID       { 0x0241 }
// 0b 010 0100 0001

#define PAY_DATA_TX_MOB_ID      { 0x0102 }
// 0b 001 0000 0010

#endif
