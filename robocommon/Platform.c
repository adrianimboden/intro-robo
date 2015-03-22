/*!
 * \file Platform.c
 * \brief Platform implementation module.
 *
 * This implements the platform module.
 * Here the platform gets initialized.
 */

#include "Platform.h"
#include "LED.h"
#include "Timer.h"
#include "Mealy.h"
#include "Keys.h"

#ifdef __cplusplus
extern "C" {
#endif

void PL_Init(void){

#if PL_HAS_LED
	LED_Init();
#endif
#if PL_HAS_EVENT
	/* do nothing */
#endif
#if PL_HAS_TIMER
	TMR_Init();
#endif
MEALY_Init();
#if PL_HAS_KEYS
	KEY_Init();
#endif
}


void PL_Deinit(void){

#if PL_HAS_LED
	LED_Deinit();
#endif
#if PL_HAS_EVENT
	/* do nothing */
#endif
#if PL_HAS_TIMER
	TMR_Deinit();
#endif
MEALY_Deinit();
#if PL_HAS_KEYS
	KEY_Deinit();
#endif

}

#ifdef __cplusplus
}  /* extern "C" */
#endif


