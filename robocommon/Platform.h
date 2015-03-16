/*!
 * \file Platform.h
 * \brief Platform implementation module.
 *
 * This implements the platform interface.
 * Here the platform gets initialized, and all platform dependent macros get defined.
 */

#ifndef PLATFORM_H_
#define PLATFORM_H_

#include "Platform_local.h"

#define CHAR_BIT 8

#include "PE_Types.h" /* for common Processor Expert types used throughout the project, e.g. 'bool' */
#include "PE_Error.h" /* global error constants */

#define PL_IS_FRDM		(defined(PL_L_BOARD_IS_FRDM))
/*!< Macro is defined in the local platform file for the FRDM board*/

#define PL_IS_ROBO		(defined(PL_L_BOARD_IS_ROBO))
/*!< Macro is defined in the local platform file for the Robo board*/

#define PL_HAS_LED		(PL_L_HAS_LED)
/*! < Macro is defined in the local platform file. */

#define PL_HAS_EVENT	(PL_L_HAS_EVENT)
/*!< Macro is defined in the local platform file */

#define PL_HAS_TIMER	(PL_L_HAS_TIMER)
/*!< Macro is defined in the local platform file */

#define PL_HAS_MEALY	(PL_L_HAS_MEALY)

#if PL_IS_FRDM

#define PL_NOF_LEDS		(3)
/*!< FRDM board has 3 LEDs (RGB) */

#include "LED.h"

#define Led_Red_On()		LED1_On()
#define Led_Red_Off()		LED1_Off()
#define Led_Red_Neg()		LED1_Neg()
#define Led_Red_Get()		LED1_Get()
#define Led_Red_Put()		LED1_Put()
#define Led_Red_Init()		LED1_Init()
#define Led_Red_Deinit()	LED1_Deinit()

#define Led_Green_On()		LED2_On()
#define Led_Green_Off()		LED2_Off()
#define Led_Green_Neg()		LED2_Neg()
#define Led_Green_Get()		LED2_Get()
#define Led_Green_Put()		LED2_Put()
#define Led_Green_Init()	LED2_Init()
#define Led_Green_Deinit()	LED2_Deinit()

#define Led_Blue_On()		LED3_On()
#define Led_Blue_Off()		LED3_Off()
#define Led_Blue_Neg()		LED3_Neg()
#define Led_Blue_Get()		LED3_Get()
#define Led_Blue_Put()		LED3_Put()
#define Led_Blue_Init()		LED3_Init()
#define Led_Blue_Deinit()	LED3_Deinit()

#elif PL_IS_ROBO

#define PL_NOF_LEDS		(2)
/*!< Robo board has 2 LEDs */

#else
  #error "unknown board?"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Platform initialization
 */
void PL_Init(void);

/*!
 * \brief Platform deinitialization
 */
void PL_Deinit(void);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* PLATFORM_H_ */
