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

//#include <Console.h>
//#include <LineInputStrategy.h>
//#include <CommandParser.h>
#include "Platform.h"
#include "EventQueue.h"
#include "CriticalSection.h"
#include "WAIT1.h"
#include "EventHandler.h"
#include "Event.h"
#include "LED.h"
#include "Keys.h"
#include "KeyDebounce.h"
#include "RTOS.h"
#include "FRTOS1.h"
#include "LegacyArgsCommand.h"

//#include "AS1.h"
extern "C" {
#include "CDC1.h"
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

			[&]{ console.getUnderlyingIoStream()->write("Key_A_Pressed!\n"); },
			[&]{ console.getUnderlyingIoStream()->write("Key_A_Long_Pressed!\n"); },
			[&]{ console.getUnderlyingIoStream()->write("Key_A_Released!\n"); },
			[&]{ console.getUnderlyingIoStream()->write("Key_A_Released_Long!\n"); },

			[&]{ console.getUnderlyingIoStream()->write("Key_B_Pressed!\n"); },
			[&]{ console.getUnderlyingIoStream()->write("Key_B_Long_Pressed!\n"); },
			[&]{ console.getUnderlyingIoStream()->write("Key_B_Released!\n"); },
			[&]{ console.getUnderlyingIoStream()->write("Key_B_Released_Long!\n"); },

			[&]{ console.getUnderlyingIoStream()->write("Key_C_Pressed!\n"); },
			[&]{ console.getUnderlyingIoStream()->write("Key_C_Long_Pressed!\n"); },
			[&]{ console.getUnderlyingIoStream()->write("Key_C_Released!\n"); },
			[&]{ console.getUnderlyingIoStream()->write("Key_C_Released_Long!\n"); },

			[&]{ console.getUnderlyingIoStream()->write("Key_D_Pressed!\n"); },
			[&]{ console.getUnderlyingIoStream()->write("Key_D_Long_Pressed!\n"); },
			[&]{ console.getUnderlyingIoStream()->write("Key_D_Released!\n"); },
			[&]{ console.getUnderlyingIoStream()->write("Key_D_Released_Long!\n"); },

			[&]{ console.getUnderlyingIoStream()->write("Key_E_Pressed!\n"); },
			[&]{ console.getUnderlyingIoStream()->write("Key_E_Long_Pressed!\n"); },
			[&]{ console.getUnderlyingIoStream()->write("Key_E_Released!\n"); },
			[&]{ console.getUnderlyingIoStream()->write("Key_E_Released_Long!\n"); },

			[&]{ console.getUnderlyingIoStream()->write("Key_F_Pressed!\n"); },
			[&]{ console.getUnderlyingIoStream()->write("Key_F_Long_Pressed!\n"); },
			[&]{ console.getUnderlyingIoStream()->write("Key_F_Released!\n"); },
			[&]{ console.getUnderlyingIoStream()->write("Key_F_Released_Long!\n"); },

			[&]{ console.getUnderlyingIoStream()->write("Key_J_Pressed!\n"); },
			[&]{ console.getUnderlyingIoStream()->write("Key_J_Long_Pressed!\n"); },
			[&]{ console.getUnderlyingIoStream()->write("Key_J_Released!\n"); },
			[&]{ console.getUnderlyingIoStream()->write("Key_J_Released_Long!\n"); }
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

void TASK_usb(void*)
{
	for(;;){
		static std::array<uint8_t, CDC1_DATA_BUFF_SIZE> sendBuffer{};
		CDC1_App_Task(sendBuffer.data(), sendBuffer.size());
	}
}

/**
 * C++ world main function
 */
void realMain()
{
	getConsole();
	PL_Init();

	if (FRTOS1_xTaskCreate(TASK_console, "consoleInput", 500, NULL, tskIDLE_PRIORITY+3, NULL) != pdPASS) { ASSERT(false); }
	//if (FRTOS1_xTaskCreate(TASK_events, "events", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+3, NULL) != pdPASS) { ASSERT(false); }
#if PL_HAS_KEYS && PL_NOF_KEYS>0
	//if (FRTOS1_xTaskCreate(TASK_keyscan, "keyscan", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+3, NULL) != pdPASS) { ASSERT(false); }
#endif
	if (FRTOS1_xTaskCreate(TASK_usb, "usb", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL) != pdPASS) { ASSERT(false); }

	eventQueue.setEvent(Event::SystemStartup);
	RTOS_Run();
}

#if PL_HAS_RADIO
void APP_DebugPrint(unsigned char *str) {
#if PL_HAS_SHELL
  //CLS1_SendStr(str, CLS1_GetStdio()->stdOut);
#endif
}
#endif

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
#if PL_HAS_ACCEL
    ACCEL_LowLevelInit();
#endif
}
