/**
* @file				flexcan_fd.h
* @brief			Header for flexcan_fd.c file
*/

#ifndef FLEXCAN_FD_H
#define FLEXCAN_FD_H

/*==================================================================================================
*                                        INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
#include "device_registers.h"	/* include peripheral declarations S32K144 */

/*==================================================================================================
*                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
*                                       LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
*                                      LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                      LOCAL VARIABLES
==================================================================================================*/

/*==================================================================================================
*                                      GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                      GLOBAL VARIABLES
==================================================================================================*/
/* Received message buffer code */
extern uint32_t RxCODE; 

/* Received message ID */
extern uint32_t RxID;

/* Received message number of data bytes */
extern uint32_t RxLENGTH;

/* Received message data (2 words) */
extern uint32_t RxDATA[2];

/* Received message time */
extern uint32_t RxTIMESTAMP;

/*==================================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
*                                       LOCAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
*                                       GLOBAL FUNCTIONS
==================================================================================================*/
/**
* @brief            FlexCAN0 Initialization.
* @details          Function to initialize FLEXCAN0 for 500 KHz bit time, clears CAN0 32 msg bufs.
* @param        	void.
* @return           void.
*/
void FLEXCAN0_init(void);

/**
* @brief            Transmit msg.
* @details          Function to transmit defined msg, using ID 0x555.
* @param        	void.
* @return           void.
*/
void FLEXCAN0_transmit_msg(void);

/**
* @brief            Receive msg.
* @details          Function to Receive msg from ID 0x556 using msg buffer 4.
* @param        	void.
* @return           void.
*/
void FLEXCAN0_receive_msg(void);


#endif	/* FLEXCAN_FD_H */
