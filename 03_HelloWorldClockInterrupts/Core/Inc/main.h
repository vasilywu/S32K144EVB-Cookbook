/**
* @file				main.h
* @brief            Header for main.c file
*/

#ifndef MAIN_H
#define MAIN_H

/*==================================================================================================
*                                        INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
#include "device_registers.h"	/* include peripheral declarations S32K144 */
#include "clocks_and_modes.h"

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
/* LPIT0 chan 0 timeout counter */
extern uint32_t u32Lpit0_ch0_flag_counter;

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
* @brief            LPIT0 Initialization.
* @details          Initialize LPIT0 channel.
* @param        	void.
* @return           void.
*/
extern void LPIT0_Ch0_IRQHandler(void);

#endif	/* MAIN_H */
