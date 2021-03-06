#pragma once

#ifndef __cplusplus
#error sorry, this header is c++ only
#endif

#include "Platform.h"
#include "EventQueue.h"
#include "CriticalSection.h"

enum class Event : uint8_t
{
	SystemStartup,		/*!< System startup Event */
	LedHeartbeat,		/*!< LED heartbeat */
#if PL_NOF_KEYS >= 1
	Sw1Pressed,
	Sw1LongPressed,
	Sw1Released,
	Sw1ReleasedLong,
#endif
#if PL_NOF_KEYS >= 2
	Sw2Pressed,
	Sw2LongPressed,
	Sw2Released,
	Sw2ReleasedLong,
#endif
#if PL_NOF_KEYS >= 3
	Sw3Pressed,
	Sw3LongPressed,
	Sw3Released,
	Sw3ReleasedLong,
#endif
#if PL_NOF_KEYS >= 4
	Sw4Pressed,
	Sw4LongPressed,
	Sw4Released,
	Sw4ReleasedLong,
#endif
#if PL_NOF_KEYS >= 5
	Sw5Pressed,
	Sw5LongPressed,
	Sw5Released,
	Sw5ReleasedLong,
#endif
#if PL_NOF_KEYS >= 6
	Sw6Pressed,
	Sw6LongPressed,
	Sw6Released,
	Sw6ReleasedLong,
#endif
#if PL_NOF_KEYS >= 7
	Sw7Pressed,
	Sw7LongPressed,
	Sw7Released,
	Sw7ReleasedLong,
#endif
#if PL_HAS_LINE_SENSOR
	RefStartStopCalibration,
#endif
	/*!< add your events here */
	AmountOfEvents		/*!< must be the last */
};

using MainEventQueue = EventQueue<Event, DisableInterrupts>;
extern MainEventQueue eventQueue;
