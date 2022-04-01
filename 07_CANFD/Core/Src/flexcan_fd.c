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
#include "flexcan_fd.h"

/*==================================================================================================
*                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
*                                       LOCAL MACROS
==================================================================================================*/
/* Msg Buffer Size. (2 words hdr + 16 words data = 18 words) */
#define MSG_BUF_SIZE		(18U)

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
	CAN0->MCR |= CAN_MCR_MDIS_MASK; 						/* MDIS=1: Disable module before selecting clock */
	CAN0->CTRL1 |= CAN_CTRL1_CLKSRC_MASK; 					/* CLKSRC=1: Clock Source = BUSCLK (40 MHz) */
	CAN0->MCR &= ~CAN_MCR_MDIS_MASK; 						/* MDIS=0; Enable module config. (Sets FRZ, HALT)*/
	while (!((CAN0->MCR & CAN_MCR_FRZACK_MASK) >> CAN_MCR_FRZACK_SHIFT)) /* wait for FRZACK=1 on freeze mode entry/exit */
	{
	}
	
	/* Configure nominal phase: 500 KHz bit time, 40 MHz Sclock */
	CAN0->CBT = CAN_CBT_BTF_MASK
				| CAN_CBT_EPRESDIV(1U)
				| CAN_CBT_EPSEG2(15U)    /* Prescaler = CANCLK / Sclock = 40 MHz / 40 MHz = 1 */
				| CAN_CBT_EPSEG1(15U)    /* EPRESDIV = Prescaler - 1 = 0 */
				| CAN_CBT_EPROPSEG(47U)  /* EPSEG2 = 15 */
				| CAN_CBT_ERJW(15U);     /* EPSEG1 = 15 */
                                         /* EPROPSEG = 46 */
                                         /* ERJW = 15 */
                                         /* BITRATEn =Fcanclk /( [(1 + (EPSEG1+1) + (EPSEG2+1) + (EPROPSEG + 1)] x (EPRESDIV+1)) */
                                         /*          = 40 MHz /( [(1 + ( 15   +1) + ( 15   +1) + (   46    + 1)] x (    0   +1)) */
                                         /*          = 40 MHz /( [1+16+16+47] x 1) = 40 MHz /(80x1) = 500 Kz */

	/* Configure data phase: 2 MHz bit time, 40 MHz Sclock */
	CAN0->FDCBT = CAN_CBT_EPSEG2(3U)	 /* Prescaler = CANCLK / Sclock = 40 MHz / 40 MHz = 1 */
				| CAN_CBT_EPSEG1(7U)	 /* FPRESDIV = Prescaler - 1 = 0 */
				| CAN_CBT_EPROPSEG(7U)	 /* FPSEG2 = 3 */
				| CAN_CBT_ERJW(3U)		 /* FPSEG1 = 7 */
				| CAN_FDCBT_FPRESDIV(1U);/* FPROPSEG = 7 */
										 /* FRJW = 3 */
										 /* BITRATEf =Fcanclk /( [(1 + (FPSEG1+1) + (FPSEG2+1) + (FPROPSEG)] x (FPRESDIV+!)) */
										 /*          = 40 MHz /( [(1 + (  7   +1) + (  3   +1) + (   7    )] x (    0   +1)) */
										 /*          = 40 MHz /( [1+8+4+7] x 1) = 40 MHz /(20x1) = 40 MHz / 20 = 2 MHz  */
	
	CAN0->FDCTRL = CAN_FDCTRL_FDRATE_MASK  	/* Configure bit rate switch, data size, transcv'r delay  */
				| CAN_FDCTRL_MBDSR0(3U)   	/* BRS=1: enable Bit Rate Swtich in frame's header */
				| CAN_FDCTRL_TDCEN_MASK  	/* MBDSR1: Not applicable */
				| CAN_FDCTRL_TDCOFF(5U);   	/* MBDSR0=3: Region 0 has 64 bytes data in frame's payload */
											/* TDCEN=1: enable Transceiver Delay Compensation */
											/* TDCOFF=5: 5 CAN clocks (300us) offset used */
	 
	for(u32Index = 0U; u32Index < 128U; u32Index++)	/* CAN0: clear 128 words RAM in FlexCAN 0 */ 
	{    
		CAN0->RAMn[u32Index] = 0U;       			/* Clear msg buf words. All buffers CODE=0 (inactive) */
	}
	
	for(u32Index = 0U; u32Index < 16U; u32Index++)	/* In FRZ mode, init CAN0 16 msg buf filters */ 
	{
		CAN0->RXIMR[u32Index] = 0xFFFFFFFFU;  		/* Check all ID bits for incoming messages */
	}
	
	CAN0->RXMGMASK = 0x1FFFFFFFU;  					/* Global acceptance mask: check all ID bits */

	/* Message Buffer 4 - receive setup: */
	CAN0->RAMn[4U*MSG_BUF_SIZE + 0U] = 0xC4000000U; /* Msg Buf 4, word 0: Enable for reception */
													/* EDL=1: Extended Data Length for CAN FD */
													/* BRS = 1: Bit Rate Switch enabled */
													/* ESI = 0: Error state */
													/* CODE=4: MB set to RX inactive */
													/* IDE=0: Standard ID */
													/* SRR, RTR, TIME STAMP = 0: not applicable */

	CAN0->RAMn[4U*MSG_BUF_SIZE + 1U] = 0x14440000U; /* Msg Buf 4, word 1: Standard ID = 0x511 */

	/* PRIO = 0: CANFD not used */
	CAN0->CTRL2 |= CAN_CTRL2_ISOCANFDEN_MASK;       /* Enable CRC fix for ISO CAN FD */
	CAN0->MCR = 0x0000081FU;       					/* Negate FlexCAN 1 halt state & enable CAN FD for 32 MBs */
	
	while ((CAN0->MCR & CAN_MCR_FRZACK_MASK) >> CAN_MCR_FRZACK_SHIFT)  /* wait for FRZACK to clear (not in freeze mode) */
	{
	}
	
	while ((CAN0->MCR & CAN_MCR_NOTRDY_MASK) >> CAN_MCR_NOTRDY_SHIFT)  /* wait for NOTRDY to clear (module ready)  */
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
	CAN0->RAMn[0U*MSG_BUF_SIZE + 0U] = 0xCC4F0000U | 8U << CAN_WMBn_CS_DLC_SHIFT;
													/* EDL=1 CAN FD format frame*/
													/* BRS=1: Bit rate is switched inside msg */
													/* ESI=0: ??? */
													/* CODE=0xC: Activate msg buf to transmit */
													/* IDE=0: Standard ID */
													/* SRR=1 Tx frame (not req'd for std ID) */
													/* RTR = 0: data, not remote tx request frame*/
													/* DLC=15; 64 bytes */
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
