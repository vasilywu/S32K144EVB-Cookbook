/**
* @file				adc.c
* @brief            ADC Driver
*/
 
/*==================================================================================================
*                                        INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
#include "adc.h"

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
* @brief            ADC Initialization.
* @details          Initiate ADC.
* @param        	void.
* @return           void.
*/
void ADC_init(void)
{
	/* ADC0 Clocking */
	PCC->PCCn[PCC_ADC0_INDEX] &=~ PCC_PCCn_CGC_MASK;  /* Disable clock to change PCS */
	PCC->PCCn[PCC_ADC0_INDEX] |= PCC_PCCn_PCS(1U);    /* PCS=1: Select SOSCDIV2 		*/
	PCC->PCCn[PCC_ADC0_INDEX] |= PCC_PCCn_CGC_MASK;   /* Enable bus clock in ADC 	*/

	ADC0->SC1[0] |= ADC_SC1_ADCH_MASK;	/* ADCH=1F: Module is disabled for conversions	*/
										/* AIEN=0: Interrupts are disabled 			*/
	ADC0->CFG1 |= (ADC_CFG1_ADIV_MASK | ADC_CFG1_MODE(1U));
										/* ADICLK=0: Input clk=ALTCLK1=SOSCDIV2 	*/
										/* ADIV=0: Prescaler=1 					*/
										/* MODE=1: 12-bit conversion 				*/

	ADC0->CFG2 = ADC_CFG2_SMPLTS(12U);	/* SMPLTS=12(default): sample time is 13 ADC clks 	*/
	ADC0->SC2 = 0x00000000U;         	/* ADTRG=0: SW trigger 							*/
										/* ACFE,ACFGT,ACREN=0: Compare func disabled		*/
										/* DMAEN=0: DMA disabled 							*/
										/* REFSEL=0: Voltage reference pins= VREFH, VREEFL */
	ADC0->SC3 = 0x00000000U;       		/* CAL=0: Do not start calibration sequence 		*/
										/* ADCO=0: One conversion performed 				*/
										/* AVGE,AVGS=0: HW average function disabled 		*/
}

/**
* @brief            Initiate ADC conversion.
* @details          Initiate ADC conversion.
* @param[in]        u16AdcChan - ADC Channel.
* @return           void.
*/
void convertAdcChan(uint16_t u16AdcChan)
{ 
	/* For SW trigger mode, SC1[0] is used */
	ADC0->SC1[0] &= ~ADC_SC1_ADCH_MASK; 		/* Clear prior ADCH bits */
	ADC0->SC1[0] = ADC_SC1_ADCH(u16AdcChan); 	/* Initiate Conversion*/
}

/**
* @brief            Get Conversion Complete Flag.
* @details          This function returns Conversion Complete Flag in ADC0 Status and Control reg.
* @param	        void.
* @return           Conversion Complete Flag.
*/
uint8_t adc_complete(void)
{
	return (uint8_t)((ADC0->SC1[0] & ADC_SC1_COCO_MASK) >> ADC_SC1_COCO_SHIFT); /* Wait for completion */
}

/**
* @brief            Get ADC0 result in mV.
* @details          This function reads ADC0 result and converts it to mV for 0-5V range.
* @param	        void.
* @return           ADC0 result.
*/
uint32_t read_adc_chx(void)
{
	uint16_t adc_result = 0U;
	adc_result = (uint16_t)(ADC0->R[0]); 		/* For SW trigger mode, R[0] is used */
	return (uint32_t)((5000U*adc_result)/0xFFFU); /* Convert result to mv for 0-5V range */
}


/* END adc */
