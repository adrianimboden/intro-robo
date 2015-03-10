/**
 * \brief Platform implementation module.
 *
 * This implements the platform module.
 * Here the platform gets initialized.
 */

#include "Platform.h"
#include "LED.h"

void PL_Init(void){

#if PL_HAS_LED
	LED_Init();
#endif

}


void PL_Deinit(void){

#if PL_HAS_LED
	LED_Deinit();
#endif

}
