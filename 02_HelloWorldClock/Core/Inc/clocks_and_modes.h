/**
* @file				clocks_and_modes.h
* @brief            Header for clocks_and_modes.c file
*/

#ifndef CLOCKS_AND_MODES_H
#define CLOCKS_AND_MODES_H

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
* @brief            SOSC Initialization.
* @details          Initialize system oscillator (SOSC) for 8 MHz crystal.
* @param        	void.
* @return           void.
*/
void SOSC_init_8MHz(void);

/**
* @brief            SPLL Initialization.
* @details          Initialize System PLL (SPLL) to 160 MHz using 8 MHz SOSC.
* @param        	void.
* @return           void.
*/
void SPLL_init_160MHz(void);

/**
* @brief            Change Normal RUN mode clock to SPLL.
* @details          Change to normal RUN mode with 8MHz SOSC, 80 MHz PLL.
* @param        	void.
* @return           void.
*/
void NormalRUNmode_80MHz(void);

#endif	/* CLOCKS_AND_MODES_H */
