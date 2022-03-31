/**
* @file				acmp.c
* @brief            Comparator Driver
*/
 
/*==================================================================================================
*                                        INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
#include "acmp.h"

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
* @brief            Comparator Initialization.
* @details          Initiate ACMP with the desired reference (Vref = 3.3*(vosel/255)).
* @param[in]        u8Vosel - DAC Output Voltage Select.
* @return           void.
*/
void ACMP_Init(uint8_t u8Vosel)
{
    PCC->PCCn[PCC_CMP0_INDEX] |= PCC_PCCn_CGC_MASK;     /* Enable bus clock in CMP */

    CMP0->C1 = CMP_C1_INPSEL(0U)    	/* Positive terminal selection from the 8-bit DAC output */
				| CMP_C1_INNSEL(1U)  	/* Negative selection from external analog mux */
				| CMP_C1_VRSEL(0U)      /* Vin1 (VDDA) as supply reference Vin */
				| CMP_C1_MSEL(0U)       /* PSEL Reference Input (0) */
				| CMP_C1_VOSEL(u8Vosel) /* DAC output voltage (Ref.) */
				| CMP_C1_DACEN_MASK;    /* DAC enable */

    CMP0->C0 |= CMP_C0_IEF_MASK			/* Comparator Interrupt Enable Falling */
				|  CMP_C0_IER_MASK;		/* Comparator Interrupt Enable Rising */

    CMP0->C0 |= CMP_C0_COS_MASK			/* COUTA: unfiltered comparator output */
				|  CMP_C0_INVT_MASK		/* Inverts the comparator output */
				|  CMP_C0_OPE_MASK		/* Output Pin Enable */
				|  CMP_C0_EN_MASK;		/* Enable CMP module */
}


/* END acmp */
