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
#include "stddef.h"

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

#define PL_HAS_RTOS			(PL_L_HAS_RTOS)
  /*!< Set to 1 for rtos enabled, 0 otherwise */

#define PL_HAS_USB_CDC		(PL_L_HAS_USB_CDC)
 /*!< Set to 1 for USB CDC enabled, 0 otherwise */

#define PL_HAS_LINE_SENSOR	(PL_L_HAS_LINE_SENSOR)
 /*!< Set to 1 for Line Sensors enabled, 0 otherwise */

#define PL_HAS_MOTOR		(PL_L_HAS_MOTOR)
 /*!< Set to 1 for Motor enabled, 0 otherwise */

#define PL_HAS_CONFIG_NVM	(PL_L_HAS_CONFIG_NVM)
/*! Set to 1 for NVM enabled, 0 otherwise */

#define PL_HAS_MCP4728		(PL_L_HAS_MCP4728)

#define PL_HAS_QUAD_CALIBRATION	(PL_L_HAS_QUAD_CALIBRATION)
/*! Set to 1 for QUAD Calibration enabled, 0 otherwise */

#define PL_HAS_MOTOR_QUAD	(PL_L_HAS_MOTOR_QUAD)
/*! Set to 1 for Motor QUAD enabled, 0 otherwise */

#define PL_HAS_MOTOR_TACHO		(PL_L_HAS_MOTOR_TACHO)
/*! Set to 1 for Tacho enabled, 0 otherwise */

#define PL_HAS_DRIVE				(PL_L_HAS_DRIVE)
/*! Set to 1 for Drive enabled, 0 otherwise */

#define PL_HAS_PID				(PL_L_HAS_PID)
/*! Set to 1 for PID enabled, 0 otherwise */

#define PL_HAS_ULTRASONIC		(PL_L_HAS_ULTRASONIC)
/*! Set to 1 for Ultrasonic enabled, 0 otherwise */

#define PL_HAS_ACCEL			(PL_L_HAS_ACCEL)
/*! Set to 1 for Acceleration enabled, 0 otherwise */

#define PL_HAS_RADIO			(PL_L_HAS_RADIO)
 /*! Set to 1 for Radio enabled, 0 otherwise */

#define PL_HAS_REMOTE         (PL_L_HAS_REMOTE)
  /*! Set to 1 for remote controller enabled, 0 otherwise */

#define PL_APP_ACCEL_CONTROL_SENDER  (PL_L_APP_ACCEL_CONTROL_SENDER)
  /*! Set to 1 if we are the remote control sender, 0 otherwise */

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
  #if PL_HAS_RADIO
	#define PL_NOF_LEDS       (2)
      /*!< FRDM board has 2 LEDs (blue is used by nrf24L01+ SPI CLK) */
  #endif
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
