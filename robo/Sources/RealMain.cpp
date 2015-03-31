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
			[&]{ console.getUnderlyingIoStream()->write("Key_A_Pressed!\r\n"); },
			[&]{ console.getUnderlyingIoStream()->write("Key_A_Long_Pressed!\r\n"); },
			[&]{ console.getUnderlyingIoStream()->write("Key_A_Released!\r\n"); }
		);
	}
}

void TASK_keyscan(void*)
{
	for(;;)
	{
		KEY_Scan();
	}
}

/**
 * C++ world main function
 */
void realMain()
{
	PL_Init();
	eventQueue.setEvent(Event::SystemStartup);

	if (FRTOS1_xTaskCreate(TASK_console, "consoleInput", 800, NULL, tskIDLE_PRIORITY, NULL) != pdPASS) { ASSERT(false); }
	if (FRTOS1_xTaskCreate(TASK_events, "events", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL) != pdPASS) { ASSERT(false); }
#if PL_HAS_KEYS && PL_NOF_KEYS>0
	if (FRTOS1_xTaskCreate(TASK_keyscan, "keyscan", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL) != pdPASS) { ASSERT(false); }
#endif

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
	LED1_Neg();
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
    BUZ_Beep(300, 500);
#endif
}
