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

#define PL_L_HAS_MEALY		(1)
/*! Set to 1 to enable MEALY support, 0 otherwise */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* PLATFORM_LOCAL_H_ */
