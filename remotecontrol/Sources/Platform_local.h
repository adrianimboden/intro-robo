/*!
 * \file Platform_local.h
 * \brief Local platform implementation module.
 *
 * This implements the local platform interface.
 * Here the local platform gets defined so that the Platform.h can initialize the right board.
 */

#ifndef PLATFORM_LOCAL_H_
#define PLATFORM_LOCAL_H_

#define PL_L_BOARD_IS_FRDM
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

#define PL_L_HAS_KBI_NMI    (1)
  /*!< Set to 1 for special case on NMI/PTA pin on FRDM board, 0 otherwise */

#define PL_L_HAS_RESET_KEY  (0)
  /*!< Set to 1 to use reset switch on FRDM as button, 0 otherwise */

#define PL_L_HAS_JOYSTICK   (1)
/*!< Set to 1 to enable joystick shield support, 0 otherwise */

#define PL_L_HAS_TRIGGER        (1)
  /*!< Set to 1 for trigger enabled, 0 otherwise */

#define PL_L_HAS_SHELL          (1)
  /*!< Set to 1 for shell enabled, 0 otherwise */

#define PL_L_HAS_BUZZER         (0)
  /*!< Set to 1 for buzzer enabled, 0 otherwise */

#define PL_L_HAS_DEBOUNCE       (1)
  /*!< Set to 1 for debouncing enabled, 0 otherwise */

#define PL_L_HAS_RTOS			(1)
  /*!< Set to 1 for rtos enabled, 0 otherwise */

#define PL_L_HAS_USB_CDC		(1)
  /*!< Set to 1 for USB_CDC enabled, 0 otherwise */

#define PL_L_HAS_RADIO			(1)
 /*! Set to 1 for Radio enabled, 0 otherwise */

#define PL_L_HAS_REMOTE         (1)
  /*! Set to 1 for remote controller enabled, 0 otherwise */

#define PL_L_APP_ACCEL_CONTROL_SENDER  (1)
  /*! Set to 1 if we are the remote control sender, 0 otherwise */


#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* PLATFORM_LOCAL_H_ */
