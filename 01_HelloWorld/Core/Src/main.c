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
/* Port PTD0, bit 0: EVB output to blue LED */
#define PTD0		(0U)

/* Port PTC12, bit 12: EVB input from BTN0 [SW2] */
#define PTC12		(12U)

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
	WDOG_disable();		/* Disable Watchdog in case it is not done in startup code */
	
	/* Enable clocks to peripherals (PORT modules) */
	PCC->PCCn[PCC_PORTC_INDEX] = PCC_PCCn_CGC_MASK;	/* Enable clock to PORT C*/
	PCC->PCCn[PCC_PORTD_INDEX] = PCC_PCCn_CGC_MASK;	/* Enable clock to PORT D*/

	/* Configure port C12 as GPIO input (BTN 0 [SW2] on EVB) */
	PTC->PDDR &= ~(1U << PTC12);   							/* Port C12: Data Direction= input (default) */
	PORTC->PCR[PTC12] = PORT_PCR_MUX(1U) | PORT_PCR_PFE_MASK; 	/* Port C12: MUX = GPIO, input filter enabled */

	/* Configure port D0 as GPIO output (LED on EVB) */
	PTD->PDDR |= 1U << PTD0;       							/* Port D0: Data Direction= output */
	PORTD->PCR[PTD0] = PORT_PCR_MUX(1U); 						/* Port D0: MUX = GPIO */

	/*----------------------------------------------------------- */
	/*    Infinite For                                            */
	/*----------------------------------------------------------- */ 
	for(;;)
	{
		if (PTC->PDIR & (1U << PTC12))	/* If Pad Data Input = 1 (BTN0 [SW2] pushed */
		{
			PTD-> PCOR |= 1U << PTD0;	/* Clear Output on port D0 (LED on) */
		}
		else							/* If BTN0 was not pushed*/
		{	
			PTD-> PSOR |= 1U << PTD0;	/* Set Output on port D0 (LED off) */
		}
		
		u32Idle_counter++;					/* Increment idle counter */
	}
}

/* END main */
