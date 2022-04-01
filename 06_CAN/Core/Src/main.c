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
/* Port PTE4, bit 4: EVB CAN0_RX */
#define PTE4		(4U)
/* Port PTE5, bit 5: EVB CAN0_TX */
#define PTE5		(5U)
/* Port PTD16, bit 16: EVB output to green LED */
#define PTD16		(16U)

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
	PCC->PCCn[PCC_PORTE_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clock for PORTE */
	PORTE->PCR[PTE4] |= PORT_PCR_MUX(5U); 	/* Port E4: MUX = ALT5, CAN0_RX */
	PORTE->PCR[PTE5] |= PORT_PCR_MUX(5U); 	/* Port E5: MUX = ALT5, CAN0_TX */
	
	PCC->PCCn[PCC_PORTD_INDEX ]|=PCC_PCCn_CGC_MASK; /* Enable clock for PORTD */
	PORTD->PCR[PTD16] = PORT_PCR_MUX(1U); 	/* Port D16: MUX = GPIO (to green LED) */
	PTD->PDDR |= 1U << 16U; 				/* Port D16: Data direction = output */
}

/*==================================================================================================
*                                       GLOBAL FUNCTIONS
==================================================================================================*/
/**
* @brief			The main function for the project.       
*/
int main(void)
{
	/* receive msg counter */
	uint32_t rx_msg_count = 0U;

	/*----------------------------------------------------------- */
	/*    Initialization                                          */
	/*----------------------------------------------------------- */	
	WDOG_disable();			/* Disable Watchdog in case it is not done in startup code */
	
	PORT_init();            /* Configure ports */
	
	SOSC_init_8MHz();       /* Initialize system oscilator for 8 MHz xtal */
	
	SPLL_init_160MHz();     /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
	
	NormalRUNmode_80MHz();  /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */
	
	FLEXCAN0_init(); 		/* Init FlexCAN0 */
	
	FLEXCAN0_transmit_msg(); /* Transmit initial message from EVB to CAN tool */
	
	/*----------------------------------------------------------- */
	/*    Infinite For                                            */
	/*----------------------------------------------------------- */ 
	for(;;)
	{
		if (1U == ((CAN0->IFLAG1 >> 4U) & 1U))	/* If CAN 0 MB 4 flag is set (received msg), read MB4 */
		{
			FLEXCAN0_receive_msg(); 	/* Read message */
			rx_msg_count++; 			/* Increment receive msg counter */
			if (rx_msg_count >= 1000U) 	/* If 1000 messages have been received, */
			{ 
				PTD->PTOR |= 1U << 16U; /* toggle output port D16 (Green LED) */
				rx_msg_count = 0U; /* and reset message counter */
			}
			FLEXCAN0_transmit_msg(); /* Transmit message using MB0 */	
		}
	}
}


/* END main */
