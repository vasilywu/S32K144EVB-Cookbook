/**
* @file				main.c
* @brief            Main program body
*/
 
/*==================================================================================================
*                                        INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
#include "main.h"

/*==================================================================================================
*                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
*                                       LOCAL MACROS
==================================================================================================*/
/* Port PTB14, bit 14: EVB LPSPI1_SCK */
#define PTB14		(14U)
/* Port PTB15, bit 15: EVB LPSPI1_SIN */
#define PTB15		(15U)
/* Port PTB16, bit 16: EVB LPSPI1_SOUT */
#define PTB16		(16U)
/* Port PTB17, bit 17: EVB LPSPI1_PCS3 */
#define PTB17		(17U)

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
/* SBC UJA1169: read Dev ID Reg @ 0x7E (expect non-zero)*/
uint16_t u16SBC_Tx = 0xFD00;

/* Returned data in to SPI */
uint16_t u16SBC_Rx = 0;

/*==================================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
/**
* @brief            Disable Watchdog.
* @details          Disable Watchdog in case it is not done in startup code.
* @param        	void.
* @return           void.
*/
void WDOG_disable(void);

/**
* @brief            Configure Port.
* @details          Enable clocks to GPIO modules and configure GPIO ports.
* @param        	void.
* @return           void.
*/
void PORT_init(void);

/*==================================================================================================
*                                       LOCAL FUNCTIONS
==================================================================================================*/
/**
* @brief            Disable Watchdog.
* @details          Disable Watchdog in case it is not done in startup code.
* @param        	void.
* @return           void.
*/
void WDOG_disable(void)
{
	WDOG->CNT = 0xD928C520U;		/* Unlock watchdog */
	WDOG->TOVAL = 0x0000FFFFU;		/* Maximum timeout value */
	WDOG->CS = 0x00002100U;			/* Disable watchdog */
}

/**
* @brief            Configure Port.
* @details          Enable clocks to GPIO modules and configure GPIO ports.
* @param        	void.
* @return           void.
*/
void PORT_init(void)
{
	PCC->PCCn[PCC_PORTB_INDEX ] |= PCC_PCCn_CGC_MASK; 	/* Enable clock for PORTB */
	PORTB->PCR[PTB14] |= PORT_PCR_MUX(3U); 				/* Port B14: MUX = ALT3, LPSPI1_SCK */
	PORTB->PCR[PTB15] |= PORT_PCR_MUX(3U); 				/* Port B15: MUX = ALT3, LPSPI1_SIN */
	PORTB->PCR[PTB16] |= PORT_PCR_MUX(3U); 				/* Port B16: MUX = ALT3, LPSPI1_SOUT */
	PORTB->PCR[PTB17] |= PORT_PCR_MUX(3U); 				/* Port B17: MUX = ALT3, LPSPI1_PCS3 */
}

/*==================================================================================================
*                                       GLOBAL FUNCTIONS
==================================================================================================*/
/**
* @brief			The main function for the project.       
*/
int main(void)
{
	/* Main loop idle counter */
	uint32_t u32Idle_counter = 0U;

	/*----------------------------------------------------------- */
	/*    Initialization                                          */
	/*----------------------------------------------------------- */	
	WDOG_disable();			/* Disable Watchdog in case it is not done in startup code */
	
	PORT_init();            /* Configure ports */
	
	SOSC_init_8MHz();       /* Initialize system oscilator for 8 MHz xtal */
	
	SPLL_init_160MHz();     /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
	
	NormalRUNmode_80MHz();  /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */
	
	LPSPI1_init_master(); 	/* Initialize LPSPI 1 as master */
	
	/*----------------------------------------------------------- */
	/*    Infinite For                                            */
	/*----------------------------------------------------------- */ 
	for(;;)
	{
		LPSPI1_transmit_16bits(u16SBC_Tx); 		/* Transmit half word (16 bits) on LPSPI1 */
		u16SBC_Rx = LPSPI1_receive_16bits(); 	/* Receive half word on LSPI1 */
		u32Idle_counter++;
	}
}


/* END main */
