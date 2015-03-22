/*!
 * \file RealMain.cpp
 * \brief RealMain
 */

#include "RealMain.h"

#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

#include <Console.h>
#include <LineInputStrategy.h>
#include "Platform.h"
#include "EventQueue.h"
#include "Console.h"
#include "LineInputStrategy.h"
#include "CriticalSection.h"
#include "WAIT1.h"
#include "EventHandler.h"
#include "Event.h"
#include "LED.h"
#include "Keys.h"
#include "Mealy.h"

#include "AS1.h"

void doLedHeartbeat(void);

class EchoConsole;

using MyLineInputStrategy = LineInputStrategy<80, DiscardingLineSink, EchoConsole>;
using RemoteControlConsole = Console<decltype(AS1_SendChar)*, MyLineInputStrategy>;

class EchoConsole
{
public:
	explicit EchoConsole(RemoteControlConsole* pConsole)
		: pConsole(pConsole)
	{
	}

	template <typename... Args>
	void write(Args... args)
	{
		pConsole->write(args...);
	}

private:
	RemoteControlConsole* pConsole;
};

/**
 * C++ world main function
 */
void realMain()
{
	RemoteControlConsole console{AS1_SendChar, MyLineInputStrategy{DiscardingLineSink{}, EchoConsole{&console}} };

	auto handleConsoleInput = [&]
    {
		byte inputChar;
		if (AS1_RecvChar(&inputChar) == ERR_OK)
		{
			console.rxChar(inputChar);
		}
    };

	console.write("Up and running\r\n");

	new(&eventQueue)MainEventQueue();
	for(;;){
		handleOneEvent(eventQueue,
			[]{},
			doLedHeartbeat,
			[&]{ console.write("Key_A_Pressed!\r\n"); },
			[&]{ console.write("Key_B_Pressed!\r\n"); },
			[&]{ console.write("Key_C_Pressed!\r\n"); },
			[&]{ console.write("Key_D_Pressed!\r\n"); },
			[&]{ console.write("Key_E_Pressed!\r\n"); },
			[&]{ console.write("Key_F_Pressed!\r\n"); },
			[&]{ console.write("Key_J_Pressed!\r\n"); }
		);

		KEY_Scan();
		MEALY_Step();
		handleConsoleInput();
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
	LED1_On(); // RED RGB LED
	WAIT1_Waitms(50);
	LED1_Off();
}
