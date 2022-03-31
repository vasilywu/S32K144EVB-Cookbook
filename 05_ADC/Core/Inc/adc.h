/**
* @file				adc.h
* @brief            Header for adc.c file
*/

#ifndef ADC_H
#define ADC_H

/*==================================================================================================
*                                        INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
#include "device_registers.h"	/* include peripheral declarations S32K144 */

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
void ADC_init(void);

/**
* @brief            Initiate ADC conversion.
* @details          Initiate ADC conversion.
* @param[in]        u16AdcChan - ADC Channel.
* @return           void.
*/
void convertAdcChan(uint16_t u16AdcChan);

/**
* @brief            Get Conversion Complete Flag.
* @details          This function returns Conversion Complete Flag in ADC0 Status and Control reg.
* @param	        void.
* @return           Conversion Complete Flag.
*/
uint8_t adc_complete(void);
	
/**
* @brief            Get ADC0 result in mV.
* @details          This function reads ADC0 result and converts it to mV for 0-5V range.
* @param	        void.
* @return           ADC0 result.
*/
uint32_t read_adc_chx(void);
	
/**
* @brief            Get ADC0 result in mV.
* @details          This function reads ADC0 result and converts it to mV for 0-5V range.
* @param	        void.
* @return           ADC0 result.
*/
uint32_t read_adc_chx(void);


#endif	/* ADC_H */
