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

/**
* @brief            LPIT0 Initialization.
* @details          Initialize LPIT0 channel.
* @param        	void.
* @return           void.
*/
void LPIT0_init(void);

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
	PCC->PCCn[PCC_PORTD_INDEX] = PCC_PCCn_CGC_MASK; 	/* Enable clock for PORT D */
	PTD->PDDR |= 1U << 0U;            				/* Port D0:  Data Direction= output */
	PORTD->PCR[0] |=  PORT_PCR_MUX(1U);  			/* Port D0:  MUX = ALT1, GPIO (to blue LED on EVB) */
}

/**
* @brief            LPIT0 Initialization.
* @details          Initialize LPIT0 channel.
* @param        	void.
* @return           void.
*/
void LPIT0_init(void)
{
	/* LPIT Clocking */
	PCC->PCCn[PCC_LPIT_INDEX] = PCC_PCCn_PCS(6U);    	/* Clock Src = 6 (SPLL2_DIV2_CLK)*/
	PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK; 	/* Enable clk to LPIT0 regs 		*/
	
	/* LPIT Initialization */
	LPIT0->MCR |= LPIT_MCR_M_CEN_MASK;  /* DBG_EN-0: Timer chans stop in Debug mode */
										/* DOZE_EN=0: Timer chans are stopped in DOZE mode */
										/* SW_RST=0: SW reset does not reset timer chans, regs */
										/* M_CEN=1: enable module clk (allows writing other LPIT0 regs) */
	
	LPIT0->TMR[0].TVAL = 40000000U;     /* Chan 0 Timeout period: 40M clocks */

	LPIT0->TMR[0].TCTRL |= LPIT_TMR_TCTRL_T_EN_MASK;
										/* T_EN=1: Timer channel is enabled */
										/* CHAIN=0: channel chaining is disabled */
										/* MODE=0: 32 periodic counter mode */
										/* TSOT=0: Timer decrements immediately based on restart */
										/* TSOI=0: Timer does not stop after timeout */
										/* TROT=0 Timer will not reload on trigger */
										/* TRG_SRC=0: External trigger soruce */
										/* TRG_SEL=0: Timer chan 0 trigger source is selected*/
}

/*==================================================================================================
*                                       GLOBAL FUNCTIONS
==================================================================================================*/
/**
* @brief			The main function for the project.       
*/
int main(void)
{
	uint32_t u32Lpit0_ch0_flag_counter = 0;

	/*----------------------------------------------------------- */
	/*    Initialization                                          */
	/*----------------------------------------------------------- */	
	WDOG_disable();			/* Disable Watchdog in case it is not done in startup code */
	
	PORT_init();            /* Configure ports */
	
	SOSC_init_8MHz();       /* Initialize system oscilator for 8 MHz xtal */
	
	SPLL_init_160MHz();     /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
	
	NormalRUNmode_80MHz();  /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */
	
	LPIT0_init();           /* Initialize PIT0 for 1 second timeout  */

	/*----------------------------------------------------------- */
	/*    Infinite For                                            */
	/*----------------------------------------------------------- */ 
	for(;;)
	{
		/* Toggle output to LED every LPIT0 timeout */
		while (0U == (LPIT0->MSR & LPIT_MSR_TIF0_MASK))	/* Wait for LPIT0 CH0 Flag */
		{
		}
		
		u32Lpit0_ch0_flag_counter++;					/* Increment LPIT0 timeout counter */
		
		PTD->PTOR |= 1U << 0U;                			/* Toggle output on port D0 (blue LED) */
		
		LPIT0->MSR |= LPIT_MSR_TIF0_MASK; 				/* Clear LPIT0 timer flag 0 */
	}
}

/* END main */
