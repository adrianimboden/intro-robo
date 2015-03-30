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
#include "CharBit.h"

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

#define PL_HAS_KEYS		(PL_L_HAS_KEYS)
/*!< Macro is defined in the local platform file */

#define PL_HAS_KBI            (PL_L_HAS_KBI)
  /*!< Set to 1 to enable key interrupt support, 0 otherwise */

#define PL_HAS_KBI_NMI        ((PL_L_HAS_KBI_NMI) && PL_IS_FRDM && PL_HAS_JOYSTICK)
  /*!< Set to 1 for special case on NMI/PTA pin on FRDM board, 0 otherwise */

#define PL_HAS_MEALY          ((PL_L_HAS_MEALY) && PL_NOF_LEDS>=1 && PL_NOF_KEYS>=1)
  /*!< Set to 1 to enable Mealy sequential state machine, 0 otherwise */

#define PL_HAS_RESET_KEY      ((PL_L_HAS_RESET_KEY) && PL_IS_FRDM && PL_HAS_KEYS)
  /*!< Set to 1 to use reset switch on FRDM as button, 0 otherwise */

#define PL_HAS_JOYSTICK       ((PL_L_HAS_JOYSTICK) && PL_IS_FRDM && PL_HAS_KEYS)
  /*!< Set to 1 to enable joystick shield support, 0 otherwise */

#define PL_HAS_TRIGGER        (PL_L_HAS_TRIGGER)
  /*!< Set to 1 for trigger enabled, 0 otherwise */

#define PL_HAS_SHELL          (PL_L_HAS_SHELL)
  /*!< Set to 1 for shell enabled, 0 otherwise */

#define PL_HAS_BUZZER         (PL_L_HAS_BUZZER && PL_IS_ROBO)
  /*!< Set to 1 for buzzer enabled, 0 otherwise */

#define PL_HAS_DEBOUNCE       (PL_L_HAS_DEBOUNCE)
  /*!< Set to 1 for debouncing enabled, 0 otherwise */

#define PL_HAS_RTOS			(1)
  /*!< Set to 1 for rtos enabled, 0 otherwise */

/* if keys are using interrupts or are polled */
#if PL_IS_FRDM
  #define PL_KEY_POLLED_KEY1    (0)
  #define PL_KEY_POLLED_KEY2    (0)
  #define PL_KEY_POLLED_KEY3    (0)
  #define PL_KEY_POLLED_KEY4    (0)
  #define PL_KEY_POLLED_KEY5    (1)
  #define PL_KEY_POLLED_KEY6    (1)
  #define PL_KEY_POLLED_KEY7    (0)
#elif PL_IS_ROBO
  #define PL_KEY_POLLED_KEY1    (0)
#endif

#if PL_IS_FRDM
  #if PL_HAS_JOYSTICK
    #define PL_NOF_LEDS       (2)
      /*!< FRDM board has 2 LEDs (blue is used by joystick shield/nrf24L01+ SPI CLK) */
    #define PL_NOF_KEYS       (7)
       /*!< FRDM board has no keys without joystick shield */
  #else
    #define PL_NOF_LEDS       (3)
       /*!< FRDM board has up to 3 LEDs (RGB) */
#if PL_HAS_RESET_KEY
    #define PL_NOF_KEYS       (1)
       /*!< FRDM board with using the reset button */
#else
    #define PL_NOF_KEYS       (0)
       /*!< FRDM board has no keys without joystick shield */
#endif
  #endif
#elif PL_IS_ROBO
  #define PL_NOF_LEDS       (2)
     /*!< We have up to 2 LED's on the robo board */
  #define PL_NOF_KEYS       (1)
     /*!< We have up to 1 push button */
#else
  #error "unknown configuration?"
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
