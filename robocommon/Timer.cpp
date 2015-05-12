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

#if PL_HAS_DRIVE
#include <atomic>

std::atomic_uint_fast32_t counter{0};
#endif


/*! \brief Function called from timer interrupt every TMR_TICK_MS */
void TMR_OnInterrupt(void){
	static int cntr = 0;
#if PL_HAS_DRIVE
	counter.fetch_add(1);
#endif

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

#if PL_HAS_DRIVE
static_assert(TMR_TICK_MS == 1, "TMR_ValueMs is no longer correct");

uint32_t TMR_ValueMs()
{
	return counter.load();
}
#endif

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
