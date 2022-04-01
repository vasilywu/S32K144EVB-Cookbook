/**
* @file				flexcan.c
* @brief            FlexCAN Driver
*/
 
/*==================================================================================================
*                                        INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
#include "flexcan.h"

/*==================================================================================================
*                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
*                                       LOCAL MACROS
==================================================================================================*/
/* Msg Buffer Size. (CAN 2.0AB: 2 hdr +  2 data= 4 words) */
#define MSG_BUF_SIZE		(4U)

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
uint32_t RxCODE = 0U; 

/* Received message ID */
uint32_t RxID = 0U;

/* Received message number of data bytes */
uint32_t RxLENGTH = 0U;

/* Received message data (2 words) */
uint32_t RxDATA[2] = {0U};

/* Received message time */
uint32_t RxTIMESTAMP = 0U;

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
void FLEXCAN0_init(void)
{

	uint32_t u32Index = 0U;

	PCC->PCCn[PCC_FlexCAN0_INDEX] |= PCC_PCCn_CGC_MASK; 	/* CGC=1: enable clock to FlexCAN0 */

	CAN0->MCR |= CAN_MCR_MDIS_MASK;         				/* MDIS=1: Disable module before selecting clock  */
	CAN0->CTRL1 &= ~CAN_CTRL1_CLKSRC_MASK;  				/* CLKSRC=0: Clock Source = oscillator (8 MHz)    */
	CAN0->MCR &= ~CAN_MCR_MDIS_MASK;        				/* MDIS=0; Enable module config. (Sets FRZ, HALT) */

	while (!((CAN0->MCR & CAN_MCR_FRZACK_MASK) >> CAN_MCR_FRZACK_SHIFT))  /* wait for FRZACK=1 on freeze mode entry/exit */
	{
	}

	CAN0->CTRL1 = CAN_CTRL1_PSEG2(3U)  		/* Configure for 500 KHz bit time                               */
				|CAN_CTRL1_PSEG1(3U)       	/* Time quanta freq = 16 time quanta x 500 KHz bit time= 8MHz   */
				|CAN_CTRL1_PROPSEG(6U)     	/* PRESDIV+1 = Fclksrc/Ftq = 8 MHz/8 MHz = 1                    */
				|CAN_CTRL1_RJW(3U)         	/*    so PRESDIV = 0                                            */
				|CAN_CTRL1_SMP(1U);        	/* PSEG2 = Phase_Seg2 - 1 = 4 - 1 = 3                           */
											/* PSEG1 = PSEG2 = 3                                            */
											/* PROPSEG= Prop_Seg - 1 = 7 - 1 = 6                            */
											/* RJW: since Phase_Seg2 >=4, RJW+1=4 so RJW=3.                 */
											/* SMP = 1: use 3 bits per CAN sample                           */
											/* CLKSRC=0 (unchanged): Fcanclk= Fosc= 8 MHz                   */
 
	for(u32Index = 0U; u32Index < 128U; u32Index++)		/* CAN0: clear 32 msg bufs x 4 words/msg buf = 128 words */
	{                    
		CAN0->RAMn[u32Index] = 0U;  					/* Clear msg buf word */
	}
	
	for(u32Index = 0U; u32Index < 16U; u32Index++)		/* In FRZ mode, init CAN0 16 msg buf filters */
	{                                 						
		CAN0->RXIMR[u32Index] = 0xFFFFFFFFU;    		/* Check all ID bits for incoming messages */
	}
	
	CAN0->RXMGMASK = 0x1FFFFFFFU;                  		/* Global acceptance mask: check all ID bits    */
	
	CAN0->RAMn[4U*MSG_BUF_SIZE + 0U] = 0x04000000U; 	/* Msg Buf 4, word 0: Enable for reception  */
														/* EDL,BRS,ESI=0: CANFD not used                */
														/* CODE=4: MB set to RX inactive                */
														/* IDE=0: Standard ID                           */
														/* SRR, RTR, TIME STAMP = 0: not applicable     */
	
	CAN0->RAMn[4U*MSG_BUF_SIZE + 1U] = 0x14440000U; 	/* Msg Buf 4, word 1: Standard ID = 0x511       */

	CAN0->MCR = 0x0000001FU;       						/* Negate FlexCAN 1 halt state for 32 MBs */

	while ((CAN0->MCR & CAN_MCR_FRZACK_MASK) >> CAN_MCR_FRZACK_SHIFT)  /* wait for FRZACK to clear (not in freeze mode) */
	{
	}

	while ((CAN0->MCR & CAN_MCR_NOTRDY_MASK) >> CAN_MCR_NOTRDY_SHIFT)  /* wait for NOTRDY to clear (module ready) */
	{
	}
}

/**
* @brief            Transmit msg.
* @details          Function to transmit defined msg, using ID 0x555.
* @param        	void.
* @return           void.
*/
void FLEXCAN0_transmit_msg(void)
{ 
	CAN0->IFLAG1 = 0x00000001U;  				  /* Clear CAN 0 MB 0 flag without clearing others*/

	CAN0->RAMn[0U*MSG_BUF_SIZE + 2U] = 0xA5112233U; /* MB0 word 2: data word 0 */
	CAN0->RAMn[0U*MSG_BUF_SIZE + 3U] = 0x44556677U; /* MB0 word 3: data word 1 */
	CAN0->RAMn[0U*MSG_BUF_SIZE + 1U] = 0x15540000U; /* MB0 word 1: Tx msg with STD ID 0x555 */
	CAN0->RAMn[0U*MSG_BUF_SIZE + 0U] = 0x0C400000U | 8U << CAN_WMBn_CS_DLC_SHIFT;
													/* MB0 word 0:                              	*/
													/* EDL,BRS,ESI=0: CANFD not used                */
													/* CODE=0xC: Activate msg buf to transmit       */
													/* IDE=0: Standard ID                           */
													/* SRR=1 Tx frame (not req'd for std ID)        */
													/* RTR = 0: data, not remote tx request frame   */
													/* DLC = 8 bytes                                */
}

/**
* @brief            Receive msg.
* @details          Function to Receive msg from ID 0x556 using msg buffer 4.
* @param        	void.
* @return           void.
*/
void FLEXCAN0_receive_msg(void)
{
	uint8_t u8Index = 0U;
	uint32_t dummy = 0U;

	RxCODE   = (CAN0->RAMn[4U*MSG_BUF_SIZE + 0U] & 0x07000000U) >> 24U;  							/* Read CODE field */
	RxID     = (CAN0->RAMn[4U*MSG_BUF_SIZE + 1U] & CAN_WMBn_ID_ID_MASK)  >> CAN_WMBn_ID_ID_SHIFT;  	/* Read ID          */
	RxLENGTH = (CAN0->RAMn[4U*MSG_BUF_SIZE + 0U] & CAN_WMBn_CS_DLC_MASK) >> CAN_WMBn_CS_DLC_SHIFT; 	/* Read Message Length */

	for (u8Index = 0U; u8Index < 2U; u8Index++)
	{  
		/* Read two words of data (8 bytes) */
		RxDATA[u8Index] = CAN0->RAMn[4U*MSG_BUF_SIZE + 2U + u8Index];
	}
	
	RxTIMESTAMP = (CAN0->RAMn[0U*MSG_BUF_SIZE + 0U] & 0x000FFFFU);
	
	dummy = CAN0->TIMER;				/* Read TIMER to unlock message buffers */
	
	CAN0->IFLAG1 = 0x00000010U;       	/* Clear CAN 0 MB 4 flag without clearing others*/
}


/* END flexcan */
