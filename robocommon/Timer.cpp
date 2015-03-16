/*!
 * \file Timer.cpp
 * \brief Timer driver interface.
 *
 * This module implements the driver for the timers used in the system.
 */
/*!
 ** @addtogroup Timer_module Timer module documentation
 ** @{
 */

#include "Platform.h"
#if PL_HAS_TIMER
#include "Timer.h"
#if PL_HAS_EVENT
	#include "Event.h"
#endif

/*! \brief Function called from timer interrupt every TMR_TICK_MS */
void TMR_OnInterrupt(void){
	static int cntr = 0;

	cntr++;
	if(cntr==1000/TMR_TICK_MS){
		eventQueue.setEvent(Event::LedHeartbeat);
		cntr = 0;
	}
}

/*! \brief Timer driver initialization */
void TMR_Init(void){
	/* nothing to do */
}

/*! \brief Timer driver deinitialization */
void TMR_Deinit(void){
	/* nothing to do */
}


#endif /* PL_HAS_TIMER */

/*!
 ** @}
 */
