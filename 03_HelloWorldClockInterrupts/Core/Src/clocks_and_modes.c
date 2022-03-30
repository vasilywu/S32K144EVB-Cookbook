/**
* @file				clocks_and_modes.c
* @brief            Main program body
*/
 
/*==================================================================================================
*                                        INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
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
void SOSC_init_8MHz(void)
{
	SCG->SOSCDIV = SCG_SOSCDIV_SOSCDIV1(1U)| SCG_SOSCDIV_SOSCDIV2(1U);  	/* SOSCDIV1 & SOSCDIV2 =1: divide by 1 */
	SCG->SOSCCFG  =	SCG_SOSCCFG_RANGE(2U)| SCG_SOSCCFG_EREFS_MASK;			/* Range=2: Medium freq (SOSC betw 1MHz-8MHz) */
																			/* HGO=0:   Config xtal osc for low power */
																			/* EREFS=1: Input is external XTAL */
	while(SCG->SOSCCSR & SCG_SOSCCSR_LK_MASK) 	/* Ensure SOSCCSR unlocked */
	{
	}
	
	SCG->SOSCCSR = SCG_SOSCCSR_SOSCEN_MASK; 	/* LK=0:          SOSCCSR can be written 				*/
												/* SOSCCMRE=0:    OSC CLK monitor IRQ if enabled 		*/
												/* SOSCCM=0:      OSC CLK monitor disabled 			*/
												/* SOSCERCLKEN=0: Sys OSC 3V ERCLK output clk disabled */
												/* SOSCLPEN=0:    Sys OSC disabled in VLP modes 		*/
												/* SOSCSTEN=0:    Sys OSC disabled in Stop modes 		*/
												/* SOSCEN=1:      Enable oscillator 					*/

	while(!(SCG->SOSCCSR & SCG_SOSCCSR_SOSCVLD_MASK)) /* Wait for sys OSC clk valid */
	{
	}
}

/**
* @brief            SPLL Initialization.
* @details          Initialize System PLL (SPLL) to 160 MHz using 8 MHz SOSC.
* @param        	void.
* @return           void.
*/
void SPLL_init_160MHz(void)
{
	while(SCG->SPLLCSR & SCG_SPLLCSR_LK_MASK) 	/* Ensure SPLLCSR unlocked 				*/
	{
	}
	SCG->SPLLCSR &= ~SCG_SPLLCSR_SPLLEN_MASK;  	/* SPLLEN=0: SPLL is disabled (default) 	*/

	SCG->SPLLDIV |= SCG_SPLLDIV_SPLLDIV1(2U)| SCG_SPLLDIV_SPLLDIV2(3U);	/* SPLLDIV1 divide by 2 */
																		/* SPLLDIV2 divide by 4 */

	SCG->SPLLCFG = SCG_SPLLCFG_MULT(24U);			/* PREDIV=0: Divide SOSC_CLK by 1 		*/
													/* MULT=24:  Multiply sys pll by 40 	*/
													/* SPLL_CLK = 8MHz / 1 * 40 / 2 = 160 MHz 	*/

	while(SCG->SPLLCSR & SCG_SPLLCSR_LK_MASK) 	/* Ensure SPLLCSR unlocked 						*/
	{
	}
	SCG->SPLLCSR |= SCG_SPLLCSR_SPLLEN_MASK; 	/* LK=0:        SPLLCSR can be written 			*/
												/* SPLLCMRE=0:  SPLL CLK monitor IRQ if enabled 	*/
												/* SPLLCM=0:    SPLL CLK monitor disabled 			*/
												/* SPLLSTEN=0:  SPLL disabled in Stop modes 		*/
												/* SPLLEN=1:    Enable SPLL 						*/

	while(!(SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK)) /* Wait for SPLL valid */
	{
	}
}

/**
* @brief            Change Normal RUN mode clock to SPLL.
* @details          Change to normal RUN mode with 8MHz SOSC, 80 MHz PLL.
* @param        	void.
* @return           void.
*/
void NormalRUNmode_80MHz(void)
{
	SCG->SIRCDIV = SCG_SIRCDIV_SIRCDIV1(1U) | SCG_SIRCDIV_SIRCDIV2(1U); /* Enable SIRCDIV2_CLK and SIRCDIV1_CLK, divide by 1 = 8MHz */

	/* Change to normal RUN mode with 8MHz SOSC, 80 MHz PLL */
	SCG->RCCR = SCG_RCCR_SCS(6U)      		/* Select PLL as clock source 								*/
				|SCG_RCCR_DIVCORE(1U)      	/* DIVCORE=1, div. by 2: Core clock = 160/2 MHz = 80 MHz 		*/
				|SCG_RCCR_DIVBUS(1U)       	/* DIVBUS=1, div. by 2: bus clock = 40 MHz 					*/
				|SCG_RCCR_DIVSLOW(2U);     	/* DIVSLOW=2, div. by 2: SCG slow, flash clock= 26 2/3 MHz	*/

	while (((SCG->CSR & SCG_CSR_SCS_MASK) >> SCG_CSR_SCS_SHIFT ) != 6U) /* Wait for sys clk src = SPLL */
	{
	}
}

/* END clocks_and_modes */
