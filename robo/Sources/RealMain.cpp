/*!
 * \file RealMain.cpp
 * \brief RealMain
 */

#include "RealMain.h"

#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

#include "Platform.h"
#include "EventQueue.h"
#include "CriticalSection.h"
#include "WAIT1.h"
#include "EventHandler.h"
#include "Event.h"
#include "LED.h"

void doLedHeartbeat(void);

/**
 * C++ world main function
 */
void realMain()
{
	new(&eventQueue)MainEventQueue();
	for(;;){
		handleOneEvent(eventQueue, []{}, doLedHeartbeat);
	}

	eventQueue.~EventQueue();
}

/**
 * Forward to C++ world
 */
void _main()
{
	realMain();
}

void doLedHeartbeat(void){
	LED1_On();
	WAIT1_Waitms(50);
	LED1_Off();
}
