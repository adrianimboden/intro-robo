/*!
 * \file LED.h
 * \brief LED driver interface.
 *
 * This module implements a generic LED driver for several LEDs.
 * It is using macros for maximum flexibility with minimal code overhead.
 */

#ifndef LED_H_
#define LED_H_

#include "Platform.h"

#if PL_NOF_LEDS >= 1

	#include "LedBit1.h"
	#define LED1_On()		(LedBit1_ClrVal())
	/*!< Turn LED 1 on */
	#define LED1_Off()		(LedBit1_SetVal())
	/*!< Turn LED 1 off */
	#define LED1_Neg()		(LedBit1_NegVal())
	/*!< Toggle LED 1 */
	#define LED1_Get()		(!LedBit1_GetVal())
	/*!< Return true if LED is on, false otherwise */
	#define LED1_Put(val) 	(LedBit1_PutVal(!val))
	/*!< Turn LED 1 on (true) or off (false) */
	#define LED1_Init()		/* do nothing */
	/*!< Initialize LED 1 */
	#define LED1_Deinit()	/* do nothing */
	/*!< Deinitialize LED 1 */

#else

	#define LED1_On()		/* do nothing */
	/*!< Turn LED 1 on */
	#define LED1_Off()		/* do nothing */
	/*!< Turn LED 1 off */
	#define LED1_Neg()		/* do nothing */
	/*!< Toggle LED 1 */
	#define LED1_Get()	1	/* do nothing */
	/*!< Return true if LED is on, false otherwise */
	#define LED1_Put(val) 	/* do nothing */
	/*!< Turn LED 1 on (true) or off (false) */
	#define LED1_Init()		/* do nothing */
	/*!< Initialize LED 1 */
	#define LED1_Deinit()	/* do nothing */
	/*!< Deinitialize LED 1 */

#endif

#if PL_NOF_LEDS >= 2

	#include "LedBit2.h"
	#define LED2_On()		(LedBit2_ClrVal())
	/*!< Turn LED 2 on */
	#define LED2_Off()		(LedBit2_SetVal())
	/*!< Turn LED 2 off */
	#define LED2_Neg()		(LedBit2_NegVal())
	/*!< Toggle LED 2 */
	#define LED2_Get()		(!LedBit2_GetVal())
	/*!< Return true if LED is on, false otherwise */
	#define LED2_Put(val) 	(LedBit2_PutVal(!val))
	/*!< Turn LED 2 on (true) or off (false) */
	#define LED2_Init()		/* do nothing */
	/*!< Initialize LED 2 */
	#define LED2_Deinit()	/* do nothing */
	/*!< Deinitialize LED 2 */

#else

	#define LED2_On()		/* do nothing */
	/*!< Turn LED 2 on */
	#define LED2_Off()		/* do nothing */
	/*!< Turn LED 2 off */
	#define LED2_Neg()		/* do nothing */
	/*!< Toggle LED 2 */
	#define LED2_Get()	1	/* do nothing */
	/*!< Return true if LED is on, false otherwise */
	#define LED2_Put(val) 	/* do nothing */
	/*!< Turn LED 2 on (true) or off (false) */
	#define LED2_Init()		/* do nothing */
	/*!< Initialize LED 2 */
	#define LED2_Deinit()	/* do nothing */
	/*!< Deinitialize LED 2 */

#endif

#if PL_NOF_LEDS >= 3

	#include "LedBit3.h"
	#define LED3_On()		(LedBit3_ClrVal())
	/*!< Turn LED 3 on */
	#define LED3_Off()		(LedBit3_SetVal())
	/*!< Turn LED 3 off */
	#define LED3_Neg()		(LedBit3_NegVal()
	/*!< Toggle LED 3 */
	#define LED3_Get()		(!LedBit3_GetVal())
	/*!< Return true if LED is on, false otherwise */
	#define LED3_Put(val) 	(LedBit3_PutVal(!val))
	/*!< Turn LED 3 on (true) or off (false) */
	#define LED3_Init()		/* do nothing */
	/*!< Initialize LED 3 */
	#define LED3_Deinit()	/* do nothing */
	/*!< Deinitialize LED 3 */

#else

	#define LED3_On()		/* do nothing */
	/*!< Turn LED 3 on */
	#define LED3_Off()		/* do nothing */
	/*!< Turn LED 3 off */
	#define LED3_Neg()		/* do nothing */
	/*!< Toggle LED 3 */
	#define LED3_Get()	1	/* do nothing */
	/*!< Return true if LED is on, false otherwise */
	#define LED3_Put(val) 	/* do nothing */
	/*!< Turn LED 3 on (true) or off (false) */
	#define LED3_Init()		/* do nothing */
	/*!< Initialize LED 3 */
	#define LED3_Deinit()	/* do nothing */
	/*!< Deinitialize LED 3 */

#endif

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief LED test routine.
 * \todo Define and implement the test.
 */
void LED_Test(void);

/*!
 * \brief LED Driver initialization.
 */
void LED_Init(void);

/*!
 * \brief LED Driver deinitialization.
 */
void LED_Deinit(void);

/*!
 * \brief Open the LED Driver
 */
void LED_Open(void);

/*!
 * \brief Close the LED Driver
 */
void LED_Close(void);

#ifdef __cplusplus
}  /* extern "C" */
#endif


#endif /* LED_H_ */
