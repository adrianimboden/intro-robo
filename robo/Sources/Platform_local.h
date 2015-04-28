/*!
 * \file Platform_local.h
 * \brief Local platform implementation module.
 *
 * This implements the local platform interface.
 * Here the local platform gets defined so that the Platform.h can initialize the right board.
 */

#ifndef PLATFORM_LOCAL_H_
#define PLATFORM_LOCAL_H_

#define PL_L_BOARD_IS_ROBO
/*!< Macro that define the board type */

#define PL_L_HAS_LED		(1)
/*!< Set to 1 to enable LED support, 0 otherwise */

#define PL_L_HAS_EVENT		(1)
/*!< Set to 1 to enable EVENT support, 0 otherwise */

#define PL_L_HAS_TIMER		(1)
/*! Set to 1 to enable TIMER support, 0 otherwise */

#define PL_L_HAS_KEYS		(1)
/*! Set to 1 to enable KEYS support,  otherwise */

#define PL_L_HAS_KBI		(1)
/*!< Set to 1 to enable key interrupt support, 0 otherwise */

#define PL_L_HAS_KBI_NMI    (0)
  /*!< Set to 1 for special case on NMI/PTA pin on FRDM board, 0 otherwise */

#define PL_L_HAS_RESET_KEY  (0)
  /*!< Set to 1 to use reset switch on FRDM as button, 0 otherwise */

#define PL_L_HAS_JOYSTICK   (0)
/*!< Set to 1 to enable joystick shield support, 0 otherwise */

#define PL_L_HAS_TRIGGER        (1)
  /*!< Set to 1 for trigger enabled, 0 otherwise */

#define PL_L_HAS_SHELL          (1)
  /*!< Set to 1 for shell enabled, 0 otherwise */

#define PL_L_HAS_BUZZER         (1)
  /*!< Set to 1 for buzzer enabled, 0 otherwise */

#define PL_L_HAS_DEBOUNCE       (1)
  /*!< Set to 1 for debouncing enabled, 0 otherwise */

#define PL_L_HAS_RTOS			(1)
  /*!< Set to 1 for rtos enabled, 0 otherwise */

#define PL_L_HAS_LINE_SENSOR	(1)
 /*!< Set to 1 for Line Sensors enabled, 0 otherwise */

#define PL_L_HAS_MOTOR			(1)
 /*!< Set to 1 for Motor enabled, 0 otherwise */

#define PL_L_HAS_CONFIG_NVM		(1)
/*! Set to 1 for NVM enabled, 0 otherwise */

#define PL_L_HAS_MCP4728		(1)

#define PL_L_HAS_QUAD_CALIBRATION	(1)
/*! Set to 1 for QUAD Calibration enabled, 0 otherwise */

#define PL_L_HAS_MOTOR_QUAD	(1)
/*! Set to 1 for Motor QUAD enabled, 0 otherwise */

#define PL_L_HAS_MOTOR_TACHO		(1)
/*! Set to 1 for Tacho enabled, 0 otherwise */

#define PL_IS_INTRO_ZUMO_K22_V2		(1)

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* PLATFORM_LOCAL_H_ */
