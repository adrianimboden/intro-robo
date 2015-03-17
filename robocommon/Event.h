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
	Sw1Pressed,
	Sw2Pressed,
	Sw3Pressed,
	Sw4Pressed,
	Sw5Pressed,
	Sw6Pressed,
	Sw7Pressed,
	/*!< \todo Your events here */
	AmountOfEvents		/*!< must be the last */
};

using MainEventQueue = EventQueue<Event, DisableInterrupts>;
extern MainEventQueue eventQueue;
