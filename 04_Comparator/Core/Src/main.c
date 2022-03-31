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
/* Port PTD15, bit 15: EVB output to red LED */
#define PTD15		(15U)
/* Port PTD16, bit 16: EVB output to green LED */
#define PTD16		(16U)
/* Port PTA0, bit 0: EVB Comparator Input */
#define PTA0		(0U)
/* Port PTE3, bit 3: EVB Comparator Output */
#define PTE3		(3U)

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
* @brief            Enable Interrupt.
* @details          Enable Interrupt.
* @param[in]        u8VectorNumber - NVIC Vector Number.
* @return           void.
*/
void Enable_Interrupt(uint8_t u8VectorNumber);

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
* @brief            Enable Interrupt.
* @details          Enable Interrupt.
* @param[in]        u8VectorNumber - NVIC Vector Number.
* @return           void.
*/
void Enable_Interrupt(uint8_t u8VectorNumber)
{
	S32_NVIC->ISER[(uint32_t)(u8VectorNumber) >> 5U] = (uint32_t)(1U << ((uint32_t)(u8VectorNumber) & (uint32_t)0x1FU));
	S32_NVIC->ICPR[(uint32_t)(u8VectorNumber) >> 5U] = (uint32_t)(1U << ((uint32_t)(u8VectorNumber) & (uint32_t)0x1FU));
}

/**
* @brief            Configure Port.
* @details          Enable clocks to GPIO modules and configure GPIO ports.
* @param        	void.
* @return           void.
*/
void PORT_init(void)
{
	PCC->PCCn[PCC_PORTA_INDEX] = PCC_PCCn_CGC_MASK; 	/* Enable clock for PORT A  */
	PORTA->PCR[PTA0] = PORT_PCR_MUX(0U);				/* Port A0: default, Comparator Input Signal */

	PCC->PCCn[PCC_PORTD_INDEX] = PCC_PCCn_CGC_MASK; 	/* Enable clock for PORT D  */
	PORTD->PCR[PTD15] = PORT_PCR_MUX(1U);  				/* Port D15: MUX = GPIO */
	PORTD->PCR[PTD16] = PORT_PCR_MUX(1U);  				/* Port D16: MUX = GPIO */

	PTD->PDDR |= ((1U << PTD15) | (1U << PTD16));	    /* Port D15: Data Direction = output */
														/* Port D16: Data Direction = output */

	PTD->PSOR |= ((1U << PTD15) | (1U << PTD16));		/* Turn-Off RGB led */

	PCC->PCCn[PCC_PORTE_INDEX] = PCC_PCCn_CGC_MASK; 	/* Enable clock for PORT D  */
	PORTE->PCR[PTE3] = PORT_PCR_MUX(7U);				/* Port E7: MUX = ALT7, Comparator Output Trigger */
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
	
	ACMP_Init(127U);		/* Initialize CMP0 */
	
	Enable_Interrupt(CMP0_IRQn);	/* Enable CMP0 interrupt */

	/*----------------------------------------------------------- */
	/*    Infinite For                                            */
	/*----------------------------------------------------------- */ 
	for(;;)
	{
		u32Idle_counter++;					/* Increment idle counter */
	}
}

/**
* @brief            Comparator Interrupt Service Routine.
* @details          LED will turn red if the input voltage does not overcome the given threshold 
*					and green if it does.
* @param        	void.
* @return           void.
*/
void CMP0_IRQHandler(void)
{
	if(CMP0->C0 & CMP_C0_CFR_MASK)
	{
		/* A rising edge on COUT has occurred */
		CMP0->C0 |= CMP_C0_CFR_MASK;	/* CFR is cleared by writing 1 to it. */
		PTD->PSOR |= 1U << PTD15;		/* Turn-Off RED led */
		PTD->PCOR |= 1U << PTD16;		/* Turn-On GREEN led */
	}
	else if(CMP0->C0 & CMP_C0_CFF_MASK)
	{
		/* A falling edge on COUT has occurred */
		CMP0->C0 |= CMP_C0_CFF_MASK;	/* CFF is cleared by writing 1 to it. */
		PTD->PSOR |= 1U << PTD16;		/* Turn-Off GREEN led */
		PTD->PCOR |= 1U << PTD15;		/* Turn-On RED led */
	}
	else
	{
	}
}

/* END main */
