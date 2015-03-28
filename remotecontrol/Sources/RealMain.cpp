/*!
 * \file RealMain.cpp
 * \brief RealMain
 */

#include "RealMain.h"

#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

#include <RemoteControlConsole.h>

#include <Console.h>
#include <LineInputStrategy.h>
#include <CommandParser.h>
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
#include "RTOS.h"
#include "FRTOS1.h"

#include "AS1.h"

void doLedHeartbeat(void);

void consoleAndKeyTask(void*)
{
	Console& console = getConsole();

	auto handleConsoleInput = [&]
    {
		byte inputChar;
		if (AS1_RecvChar(&inputChar) == ERR_OK)
		{
			console.rxChar(inputChar);
		}
    };

	console.write("Up and running\r\n");
	console.rxChar('t');
	console.rxChar('e');
	console.rxChar('s');
	console.rxChar('t');
	console.rxChar('\n');

	for(;;){
		handleOneEvent(eventQueue,
			[]{},
			doLedHeartbeat,
			[&]{ console.write("Key_A_Pressed!\r\n");  },
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
}

/**
 * C++ world main function
 */
void realMain()
{
	RTOS_Init();


	if (FRTOS1_xTaskCreate(consoleAndKeyTask, "consoleAndKeyTask", 400, NULL, tskIDLE_PRIORITY, NULL) != pdPASS)
	{
		ASSERT(false);
	}
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
