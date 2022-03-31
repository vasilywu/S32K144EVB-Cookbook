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

/*==================================================================================================
*                                      LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                      LOCAL VARIABLES
==================================================================================================*/
/* LPIT0 chan 0 timeout counter */
uint32_t u32Lpit0_ch0_flag_counter = 0U;

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
* @brief            Initialize an interrupt.
* @details          To initialize an interrupt.
* @param        	void.
* @return           void.
*/
void NVIC_init_IRQs(void);

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
* @brief            Initialize an interrupt.
* @details          To initialize an interrupt.
* @param        	void.
* @return           void.
*/
void NVIC_init_IRQs(void)
{
	S32_NVIC->ICPR[1] = 1U << (48 % 32);  /* IRQ48-LPIT0 ch0: clr any pending IRQ*/
	S32_NVIC->ISER[1] = 1U << (48 % 32);  /* IRQ48-LPIT0 ch0: enable IRQ */
	S32_NVIC->IP[48] = 0xAU;              	/* IRQ48-LPIT0 ch0: priority 10 of 0-15*/
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
	PTD->PDDR |= 1U << 0U;            					/* Port D0:  Data Direction= output */
	PORTD->PCR[PTD0] |=  PORT_PCR_MUX(1U);  			/* Port D0:  MUX = ALT1, GPIO (to blue LED on EVB) */
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
	
	LPIT0->MIER = LPIT_MIER_TIE0_MASK;  /* TIE0=1: Timer Interrupt Enabled fot Chan 0 */
	
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
	
	NVIC_init_IRQs();       /* Enable desired interrupts and priorities */
	
	LPIT0_init();           /* Initialize PIT0 for 1 second timeout  */

	/*----------------------------------------------------------- */
	/*    Infinite For                                            */
	/*----------------------------------------------------------- */ 
	for(;;)
	{
		u32Idle_counter++;					/* Increment idle counter */
	}
}

/**
* @brief            LPIT0 Initialization.
* @details          Initialize LPIT0 channel.
* @param        	void.
* @return           void.
*/
void LPIT0_Ch0_IRQHandler(void)
{
	/* Perform read-after-write to ensure flag clears before ISR exit */
	LPIT0->MSR |= LPIT_MSR_TIF0_MASK; 	/* Clear LPIT0 timer flag 0 */
		
	u32Lpit0_ch0_flag_counter++; 		/* Increment LPIT0 timeout counter */
	PTD->PTOR |= 1U << 0U; 					/* Toggle output on port D0 (blue LED) */
}

/* END main */
