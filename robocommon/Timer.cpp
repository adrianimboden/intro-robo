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
#if PL_HAS_TRIGGER
  #include "Trigger.h"
#endif
#if PL_HAS_MOTOR_TACHO
	#include "Tacho.h"
#endif

/*! \brief Function called from timer interrupt every TMR_TICK_MS */
void TMR_OnInterrupt(void){
	static int cntr = 0;

#if PL_HAS_TRIGGER
	TRG_IncTick();
#endif

#if PL_HAS_EVENT
	cntr++;
	if(cntr==1000/TMR_TICK_MS){
		eventQueue.setEvent(Event::LedHeartbeat);
		cntr = 0;
	}
#endif

#if PL_HAS_MOTOR_TACHO
	TACHO_Sample();
#endif
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
