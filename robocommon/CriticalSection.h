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

#ifndef __cplusplus
#error sorry, this header is c++ only
#endif

#include "CS1.h"


class DisableInterrupts
{
public:
	DisableInterrupts()
	{
		CS1_EnterCritical();
	}

	~DisableInterrupts()
	{
		CS1_ExitCritical();
	}

private:
	CS1_CriticalVariable();
};

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

#endif /* CRITICALSECTION_H_ */
