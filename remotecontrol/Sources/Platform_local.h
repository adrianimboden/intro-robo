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

#include "Platform.h"


#if PL_L_HAS_LED

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

#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* PLATFORM_LOCAL_H_ */
