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
#include "Keys.h"
#include "Mealy.h"

extern "C" {
//#include "CLS1.h"
}

void doLedHeartbeat(void);

void Key_A_Pressed(void);
void Key_B_Pressed(void);
void Key_C_Pressed(void);
void Key_D_Pressed(void);
void Key_E_Pressed(void);
void Key_F_Pressed(void);
void Key_J_Pressed(void);

/**
 * C++ world main function
 */
void realMain()
{
	new(&eventQueue)MainEventQueue();
	for(;;){
		handleOneEvent(eventQueue, []{}, doLedHeartbeat, Key_A_Pressed, Key_B_Pressed, Key_C_Pressed, Key_D_Pressed, Key_E_Pressed, Key_F_Pressed, Key_J_Pressed);
		#if PL_HAS_KEYS && PL_NOF_KEYS>0
			KEY_Scan(); /* scan keys */
		#endif
		#if PL_HAS_MEALY
			MEALY_Step();
		#endif
		WAIT1_Waitms(100);
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

void Key_A_Pressed(void){
	//CLS1_SendStr((const uint8_t* )"Key_A_Pressed!\r\n", CLS1_GetStdio()->stdOut);
}

void Key_B_Pressed(void){
	//CLS1_SendStr((const uint8_t* )"Key_B_Pressed!\r\n", CLS1_GetStdio()->stdOut);
}
void Key_C_Pressed(void){
	//CLS1_SendStr((const uint8_t* )"Key_C_Pressed!\r\n", CLS1_GetStdio()->stdOut);
}
void Key_D_Pressed(void){
	//CLS1_SendStr((const uint8_t* )"Key_D_Pressed!\r\n", CLS1_GetStdio()->stdOut);
}
void Key_E_Pressed(void){
	//CLS1_SendStr((const uint8_t* )"Key_E_Pressed!\r\n", CLS1_GetStdio()->stdOut);
}
void Key_F_Pressed(void){
	//CLS1_SendStr((const uint8_t* )"Key_F_Pressed!\r\n", CLS1_GetStdio()->stdOut);
}
void Key_J_Pressed(void){
	//CLS1_SendStr((const uint8_t* )"Key_J_Pressed!\r\n", CLS1_GetStdio()->stdOut);
}
