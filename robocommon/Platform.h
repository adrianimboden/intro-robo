/*!
 * \file Platform.h
 * \brief Platform implementation module.
 *
 * This implements the platform interface.
 * Here the platform gets initialized, and all platform dependent macros get defined.
 */

#ifndef PLATFORM_H_
#ifdef PLATFORM_LOCAL_H_
#define PLATFORM_H_

#define CHAR_BIT 8

#include "PE_Types.h" /* for common Processor Expert types used throughout the project, e.g. 'bool' */
#include "PE_Error.h" /* global error constants */

#ifdef __cplusplus
extern "C" {
#endif

#define PL_IS_FRDM		(defined(PL_L_BOARD_IS_FRDM))
/*!< Macro is defined in the local platform file for the FRDM board*/

#define PL_IS_ROBO		(defined(PL_L_BOARD_IS_ROBO))
/*!< Macro is defined in the local platform file for the Robo board*/

#define PL_HAS_LED		(PL_L_HAS_LED)
/*! < Macro is defined in the local platform file. */

#if PL_IS_FRDM

#define PL_NOF_LEDS		(3)
/*!< FRDM board has 3 LEDs (RGB) */

#elif PL_IS_ROBO

#define PL_NOF_LEDS		(2)
/*!< Robo board has 2 LEDs */

#else
  #error "unknown board?"
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

#endif /* PLATFORM_LOCAL_H_ */
#endif /* PLATFORM_H_ */
