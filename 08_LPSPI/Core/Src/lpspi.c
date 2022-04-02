/**
* @file				lpspi.c
* @brief            LPSPI Driver
*/
 
/*==================================================================================================
*                                        INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
#include "lpspi.h"

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
* @brief            LPSPI1 Initialization.
* @details          Function to initialize LPSPI1 as master.
* @param        	void.
* @return           void.
*/
void LPSPI1_init_master(void)
{
	PCC->PCCn[PCC_LPSPI1_INDEX] = 0U;          			/* Disable clocks to modify PCS ( default) 	*/
	PCC->PCCn[PCC_LPSPI1_INDEX] = PCC_PCCn_PR_MASK		/* (default) Peripheral is present.			*/
								| PCC_PCCn_CGC_MASK		/* Enable PCS=SPLL_DIV2 (40 MHz func'l clock) 	*/
								| PCC_PCCn_PCS(6U);

	LPSPI1->CR    = 0x00000000U;   			/* Disable module for configuration 			*/
	LPSPI1->IER   = 0x00000000U;   			/* Interrupts not used 						*/
	LPSPI1->DER   = 0x00000000U;   			/* DMA not used 								*/
	LPSPI1->CFGR0 = 0x00000000U;   			/* Defaults: 									*/
											/* RDM0=0: rec'd data to FIFO as normal 		*/
											/* CIRFIFO=0; Circular FIFO is disabled 		*/
											/* HRSEL, HRPOL, HREN=0: Host request disabled */

	LPSPI1->CFGR1 = LPSPI_CFGR1_MASTER_MASK;/* Configurations: master mode									*/
											/* PCSCFG=0: PCS[3:2] are enabled 								*/
											/* OUTCFG=0: Output data retains last value when CS negated	*/
											/* PINCFG=0: SIN is input, SOUT is output 						*/
											/* MATCFG=0: Match disabled 									*/
											/* PCSPOL=0: PCS is active low 								*/
											/* NOSTALL=0: Stall if Tx FIFO empty or Rx FIFO full 			*/
											/* AUTOPCS=0: does not apply for master mode 					*/
											/* SAMPLE=0: input data sampled on SCK edge 					*/
											/* MASTER=1: Master mode 										*/

	LPSPI1->TCR = LPSPI_TCR_CPHA_MASK
				| LPSPI_TCR_PRESCALE(2U)
				| LPSPI_TCR_PCS(3U)
				| LPSPI_TCR_FRAMESZ(15U);   /* Transmit cmd: PCS3, 16 bits, prescale func'l clk by 4, etc	*/
											/* CPOL=0: SCK inactive state is low 							*/
											/* CPHA=1: Change data on SCK lead'g, capture on trail'g edge	*/
											/* PRESCALE=2: Functional clock divided by 2**2 = 4 			*/
											/* PCS=3: Transfer using PCS3 									*/
											/* LSBF=0: Data is transfered MSB first 						*/
											/* BYSW=0: Byte swap disabled 									*/
											/* CONT, CONTC=0: Continuous transfer disabled 				*/
											/* RXMSK=0: Normal transfer: rx data stored in rx FIFO 		*/
											/* TXMSK=0: Normal transfer: data loaded from tx FIFO 			*/
											/* WIDTH=0: Single bit transfer 								*/
											/* FRAMESZ=15: # bits in frame = 15+1=16 						*/

	LPSPI1->CCR = LPSPI_CCR_SCKPCS(4U)
				| LPSPI_CCR_PCSSCK(4U)
				| LPSPI_CCR_DBT(8U)
				| LPSPI_CCR_SCKDIV(8U);   	/* Clock dividers based on prescaled func'l clk of 100 nsec 	*/
											/* SCKPCS=4: SCK to PCS delay = 4+1 = 5 (500 nsec) 			*/
											/* PCSSCK=4: PCS to SCK delay = 9+1 = 10 (1 usec) 				*/
											/* DBT=8: Delay between Transfers = 8+2 = 10 (1 usec) 			*/
											/* SCKDIV=8: SCK divider =8+2 = 10 (1 usec: 1 MHz baud rate) 	*/

	LPSPI1->FCR = LPSPI_FCR_TXWATER(3U);   	/* RXWATER=0: Rx flags set when Rx FIFO >0 	*/
											/* TXWATER=3: Tx flags set when Tx FIFO <= 3 	*/

	LPSPI1->CR = LPSPI_CR_MEN_MASK
				|LPSPI_CR_DBGEN_MASK;   	/* Enable module for operation 			*/
											/* DBGEN=1: module enabled in debug mode 	*/
											/* DOZEN=0: module enabled in Doze mode 	*/
											/* RST=0: Master logic not reset 			*/
											/* MEN=1: Module is enabled */
}

/**
* @brief            SPI Transmit 16-bit data.
* @details          This function polls Tx FIFO, and transmit given 16-bit data, then clears TDF flag.
* @param[in]        u16Send - The 16-bit data to send.
* @return           void.
*/
void LPSPI1_transmit_16bits(uint16_t u16Send)
{ 
	while((LPSPI1->SR & LPSPI_SR_TDF_MASK)>>LPSPI_SR_TDF_SHIFT==0)	/* Wait for Tx FIFO available 	*/
	{
	}
								   
	LPSPI1->TDR = u16Send;              	/* Transmit data 				*/
	LPSPI1->SR |= LPSPI_SR_TDF_MASK; 		/* Clear TDF flag   */
}

/**
* @brief            SPI Receive 16-bit data.
* @details          This function waits for at least one Rx FIFO entry, reads the received data, clears RDF flag,
*					and returns the 16-bit received data.
* @param        	void.
* @return           The 16-bit data to received.
*/
uint16_t LPSPI1_receive_16bits(void)
{
	uint16_t u16Recieve = 0U;

	while((LPSPI1->SR & LPSPI_SR_RDF_MASK)>>LPSPI_SR_RDF_SHIFT==0) /* Wait at least one RxFIFO entry 	*/
	{
	}
								   
	u16Recieve = (uint16_t)LPSPI1->RDR;            	/* Read received data 				*/
	LPSPI1->SR |= LPSPI_SR_RDF_MASK; 				/* Clear RDF flag 					*/
	return u16Recieve;                  			/* Return received data 			*/
}


/* END lpspi */
