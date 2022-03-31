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
/* Port PTD0, bit 0: EVB output to blue LED */
#define PTD0		(0U)

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
/* ADC0 Channel 12 Result in miliVolts */
uint32_t u32AdcResultInMv_pot = 0U;

/* ADC0 Channel 29 Result in miliVolts */
uint32_t u32AdcResultInMv_Vrefsh = 0U;

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
* @brief            Configure Port.
* @details          Enable clocks to GPIO modules and configure GPIO ports.
* @param        	void.
* @return           void.
*/
void PORT_init(void)
{
	PCC->PCCn[PCC_PORTD_INDEX ]|=PCC_PCCn_CGC_MASK; 		/* Enable clock for PORTD */
	PORTD->PCR[PTD0] = PORT_PCR_MUX(1U); 					/* Port D0: MUX = GPIO */
	PORTD->PCR[PTD15] = PORT_PCR_MUX(1U); 					/* Port D15: MUX = GPIO */
	PORTD->PCR[PTD16] = PORT_PCR_MUX(1U);					/* Port D16: MUX = GPIO */
	PTD->PDDR |= (1U << PTD0 | 1U << PTD15 | 1U << PTD16);	/* Port D0: Data Direction= output */
															/* Port D15: Data Direction= output */
															/* Port D16: Data Direction= output */
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
	
	PORT_init(); 			/* Init port clocks and gpio outputs */
	
	ADC_init(); 			/* Init ADC resolution 12 bit*/

	/*----------------------------------------------------------- */
	/*    Infinite For                                            */
	/*----------------------------------------------------------- */ 
	for(;;)
	{
		u32Idle_counter++;						/* Increment idle counter */
		
		convertAdcChan(12U); 					/* Convert Channel AD12 to pot on EVB */
		while(adc_complete() == 0U)				/* Wait for conversion complete flag */
		{
		}
		u32AdcResultInMv_pot = read_adc_chx(); 	/* Get channel's conversion results in mv */
		
		if (u32AdcResultInMv_pot > 3750U) 		/* If result > 3.75V */
		{ 
			PTD->PSOR |= (1U << PTD0 | 1U << PTD16); 	/* turn off blue, green LEDs */
			PTD->PCOR |= 1U << PTD15; 					/* turn on red LED */
		}
		else if (u32AdcResultInMv_pot > 2500U) 	/* If result > 3.75V */
		{ 
			PTD->PSOR |= (1U << PTD0 | 1U << PTD15); 	/* turn off blue, red LEDs */
			PTD->PCOR |= 1U << PTD16; 					/* turn on green LED */
		}
		else if (u32AdcResultInMv_pot >1250U)	/* If result > 3.75V */ 
		{
			PTD->PSOR |= (1U << PTD15 | 1U << PTD16); 	/* turn off red, green LEDs */
			PTD->PCOR |= 1U << PTD0; 					/* turn on blue LED */
		}
		else 
		{
			PTD->PSOR |= (1U << PTD0 | 1U << PTD15 | 1U << PTD16); /* Turn off all LEDs */
		}
		
		convertAdcChan(29U); 					/* Convert chan 29, Vrefsh */
		while(adc_complete() == 0U) 				/* Wait for conversion complete flag */
		{
		}
		u32AdcResultInMv_Vrefsh = read_adc_chx(); /* Get channel's conversion results in mv */		
	}
}


/* END main */
