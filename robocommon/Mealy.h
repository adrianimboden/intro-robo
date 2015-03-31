/**
 * \file
 * \brief This is the interface to the Mealy Sequential State Machine
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * We are using a Mealy Sequential Machine to test the LED driver.
 */

#ifndef MEALY_H_
#define MEALY_H_


#ifdef __cplusplus
extern "C" {
#endif

/*! \brief This implements our Mealy Sequential Machine. On each call the FSM will perform a check/test advances as necessary. */
void MEALY_Step(void);

/*! \brief Initialization routine of the state machine */
void MEALY_Init(void);

/*! \brief De-iinitialization routine of the state machine */
void MEALY_Deinit(void);

#ifdef __cplusplus
}  /* extern "C" */
#endif



#endif /* MEALY_H_ */
