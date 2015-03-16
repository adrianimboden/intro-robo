#pragma once

#ifndef __cplusplus
#error sorry, this header is c++ only
#endif

#include "EventQueue.h"
#include "CriticalSection.h"

enum class Event : uint8_t
{
	SystemStartup,		/*!< System startup Event */
	LedHeartbeat,		/*!< LED heartbeat */
	/*!< \todo Your events here */
	AmountOfEvents		/*!< must be the last */
};

using MainEventQueue = EventQueue<Event, DisableInterrupts>;
extern MainEventQueue eventQueue;
