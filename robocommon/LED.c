/**
 * \brief LED driver interface.
 *
 * This module implements a generic LED driver for several LEDs.
 * It is using macros for maximum flexibility with minimal code overhead.
 */
/*!
 ** @addtogroup LED_module LED module documentation
 ** @{
 */

#include "LED.h"

void LED_Init(void) {
	/* all LED's off by default */
	LED1_Off();
	LED2_Off();
	LED3_Off();
}

void LED_Deinit(void) {
	/* nothing to do */
}

void LED_Open(void) {
	/* nothing to do */
}

void LED_Close(void) {
	/* nothing to do */
}

/*!
 ** @}
 */



