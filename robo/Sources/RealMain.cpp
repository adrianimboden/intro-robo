/*!
 * \file RealMain.cpp
 * \brief RealMain
 */

#include "RealMain.h"

#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

#include "RoboConsole.h"

#include "Platform.h"
#include "EventQueue.h"
#include "CriticalSection.h"
#include "WAIT1.h"
#include "EventHandler.h"
#include "Event.h"
#include "LED.h"
#include "Keys.h"
#include "Buzzer.h"
#include "KeyDebounce.h"
#include "RTOS.h"
#include "FRTOS1.h"
#include "BT1.h"
#include "MainControl.h"

extern "C" {
#include "Reflectance.h"
}

void doLedHeartbeat(void);
void systemReady(void);

void TASK_events(void*)
{
	Console& console = getConsole();
	for(;;)
	{
		handleOneEvent(eventQueue,
			systemReady,
			doLedHeartbeat,
			[&]{ console.getUnderlyingIoStream()->write("Key_A_Pressed!\n"); MainControl::notifyStartMove(!MainControl::hasStartMove()); },
			[&]{ console.getUnderlyingIoStream()->write("Key_A_Long_Pressed!\n"); eventQueue.setEvent(Event::RefStartStopCalibration); },
			[&]{ console.getUnderlyingIoStream()->write("Key_A_Released!\n"); },
			[]{eventQueue.setEvent(Event::RefStartStopCalibration);}
		);
		WAIT1_WaitOSms(10);
	}
}

void TASK_keyscan(void*)
{
	for(;;)
	{
		KEY_Scan();
		WAIT1_WaitOSms(10);
	}
}

/**
 * C++ world main function
 */
void realMain()
{
	getConsole();
	PL_Init();
	eventQueue.setEvent(Event::SystemStartup);

	if (FRTOS1_xTaskCreate(TASK_console, "consoleInput", 1600, NULL, tskIDLE_PRIORITY + 3, NULL) != pdPASS) { ASSERT(false); }
	if (FRTOS1_xTaskCreate(TASK_events, "events", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, NULL) != pdPASS) { ASSERT(false); }
#if PL_HAS_KEYS && PL_NOF_KEYS>0
	if (FRTOS1_xTaskCreate(TASK_keyscan, "keyscan", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, NULL) != pdPASS) { ASSERT(false); }
#endif
	//if (FRTOS1_xTaskCreate(MainControl::task, "mainControl", 800, NULL, tskIDLE_PRIORITY+2, NULL) != pdPASS) { ASSERT(false); }

	RTOS_Run();
}


/**
 * Forward to C++ world
 */
void _main()
{
	realMain();
}

void doLedHeartbeat(void){
}

void systemReady(void){
    LED1_On();
    WAIT1_Waitms(10);
    LED1_Off();
    WAIT1_Waitms(10);
    LED1_On();
    WAIT1_Waitms(10);
    LED1_Off();
#if PL_HAS_BUZZER
    constexpr auto baseFreq = 33;
    constexpr auto baseTime = 60;
    return;

	auto f1 = []{ BUZ_BlockingBeep(baseFreq * 6, baseTime * 1); };
	auto e1 = []{ BUZ_BlockingBeep(baseFreq * 5, baseTime * 1); };
	auto d1 = []{ BUZ_BlockingBeep(baseFreq * 4, baseTime * 1); };
	auto c1 = []{ BUZ_BlockingBeep(baseFreq * 3, baseTime * 1); };
	auto b1 = []{ BUZ_BlockingBeep(baseFreq * 2, baseTime * 1); };
	auto a1 = []{ BUZ_BlockingBeep(baseFreq * 1, baseTime * 1); };

	auto e2 = []{ BUZ_BlockingBeep(baseFreq * 5, baseTime * 2); };
	auto c2 = []{ BUZ_BlockingBeep(baseFreq * 3, baseTime * 2); };

	auto e4 = []{ BUZ_BlockingBeep(baseFreq * 5, baseTime * 4); };
	auto a4 = []{ BUZ_BlockingBeep(baseFreq * 1, baseTime * 4); };

	for (auto&& fn : std::initializer_list<void(*)()>{a1, b1, c1, d1, e2, e2, f1, f1, f1, f1, e4, f1, f1, f1, f1, e4, d1, d1, d1, d1, c2, c2, e1, e1, e1, e1, a4})
	{
		fn();
		FRTOS1_vTaskDelay(30/portTICK_RATE_MS);
	}

#endif
}
