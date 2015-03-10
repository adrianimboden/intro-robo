/*!
 * \file CriticalSection.h
 * \brief Critical section interface
 *
 * This module implements the interface that deals with Critical Sections.
 * It use the Processor Expert CritialSection Makros.
 *
 */

#ifndef CRITICALSECTION_H_
#define CRITICALSECTION_H_

#include "CS1.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief This function runs the passed function in a critical section.
 * @param fn The Function to be executed in a critical section.
 */
template <typename Fn>
void runWithDisabledInterrupts(Fn fn)
{

	CS1_CriticalVariable();
	CS1_EnterCritical();
	fn();
	CS1_ExitCritical();
}

#ifdef __cplusplus
}  /* extern "C" */
#endif




#endif /* CRITICALSECTION_H_ */
