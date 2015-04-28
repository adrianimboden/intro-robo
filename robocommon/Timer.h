/*!
 * \file Timer.h
 * \brief Timer driver interface.
 *
 * This module implements the driver for the timers used in the system.
 */

#ifndef TIMER1_H_
#define TIMER1_H_

#ifdef __cplusplus
extern "C" {
#endif

#define TMR_TICK_MS		1
/*! we get called every TMR_TICK_MS ms */

/*! \brief Function called from timer interrupt every TMR_TICK_MS */
void TMR_OnInterrupt(void);

uint32_t TMR_ValueMs();

/*! \brief Timer driver initialization */
void TMR_Init(void);

/*! \brief Timer driver deinitialization */
void TMR_Deinit(void);


#ifdef __cplusplus
}  /* extern "C" */
#endif



#endif /* TIMER1_H_ */
