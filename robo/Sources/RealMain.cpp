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
#include "Ultrasonic.h"
#include "Accel.h"
#include "LegacyArgsCommand.h"
#include "Remote.h"
#include "Drive.h"
#if PL_HAS_MUSIC_SHIELD
extern "C" {
#include "FAT1.h"
}
#include "Music.h"
#endif

extern "C" {
#include "Reflectance.h"
}

CLS1_StdIOType io;
#if PL_HAS_MUSIC_SHIELD
FAT1_FATFS fileSystemObject;
#endif

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
			[&]{ console.getUnderlyingIoStream()->write("Key_A_Pressed!\n");
#if PL_HAS_MUSIC_SHIELD
			if(!MainControl::hasStartMove()){
				MUSIC_PlayTheme(MUSIC_TOUGH);
				WAIT1_WaitOSms(5000);
#if PL_L_HAS_SHOVEL
				DRV_SetSpeed(7000,7000);
				WAIT1_WaitOSms(100);
				DRV_SetSpeed(0,0);
				WAIT1_WaitOSms(250);
#endif

			}
#else
			if(!MainControl::hasStartMove()){
				WAIT1_WaitOSms(1000);
				BUZ_BlockingBeep(440, 500);
				WAIT1_WaitOSms(500);
				BUZ_BlockingBeep(440, 500);
				WAIT1_WaitOSms(500);
				BUZ_BlockingBeep(440, 500);
				WAIT1_WaitOSms(500);
				BUZ_BlockingBeep(880, 500);
				WAIT1_WaitOSms(500);
			}
#endif
			MainControl::notifyStartMove(!MainControl::hasStartMove());
			REMOTE_SetOnOff(!MainControl::hasStartMove());},
			[&]{ console.getUnderlyingIoStream()->write("Key_A_Long_Pressed!\n"); eventQueue.setEvent(Event::RefStartStopCalibration); },
			[&]{ console.getUnderlyingIoStream()->write("Key_A_Released!\n"); },
			[&]{ console.getUnderlyingIoStream()->write("Key_A_Released_Long!\n"); },
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

void TASK_ultrasonicScan(void*)
{
	uint16_t cm, us;
#if PL_HAS_MUSIC_SHIELD
	static uint8_t played = 0;
#endif
	for(;;)
	{
		 us = US_Measure_us();
		 cm = US_usToCentimeters(us, 22);
		 MainControl::notifyEnemyDetected(cm);

#if PL_HAS_MUSIC_SHIELD
		 if(cm<100 && cm>50 && played!=0 && !MUSIC_IsPlaying()) {
			 MUSIC_PlayTheme(MUSIC_POINT);
			 played=0;
		 }
		 else if (cm<50 && cm>10 && played!=1 && !MUSIC_IsPlaying()) {
			 MUSIC_PlayTheme(MUSIC_GO_EYES);
			 played=1;
		 }
		 else if (cm<10 && played!=2 && !MUSIC_IsPlaying()) {
			 MUSIC_PlayTheme(MUSIC_BUTT_KICK);
			 played=2;
		 }
#endif
		 WAIT1_WaitOSms(35);	// MAX 30Hz -> 33ms
	}
}

#if PL_HAS_ACCEL
void TASK_accelerationMeasure(void*)
{
	int16_t xValue, yValue, zValue = 0;
	for(;;)
	{
		ACCEL_GetValues(&xValue,&yValue,&zValue);
		MainControl::notifyStopMotors(zValue < -800);
		WAIT1_WaitOSms(250);
	}
}
#endif

/**
 * C++ world main function
 */
void realMain()
{
	getConsole();
	PL_Init();

	if (FRTOS1_xTaskCreate(TASK_console, "consoleInput", 1600, NULL, tskIDLE_PRIORITY + 3, NULL) != pdPASS) { ASSERT(false); }
	if (FRTOS1_xTaskCreate(TASK_events, "events", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, NULL) != pdPASS) { ASSERT(false); }
#if PL_HAS_KEYS && PL_NOF_KEYS>0
	if (FRTOS1_xTaskCreate(TASK_keyscan, "keyscan", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, NULL) != pdPASS) { ASSERT(false); }
#endif
	if (FRTOS1_xTaskCreate(MainControl::task, "mainControl", 800, NULL, tskIDLE_PRIORITY+2, NULL) != pdPASS) { ASSERT(false); }
	if (FRTOS1_xTaskCreate(TASK_ultrasonicScan, "ultrasonicScan", 800, NULL, tskIDLE_PRIORITY+3, NULL) != pdPASS) { ASSERT(false); }
#if PL_HAS_ACCEL
	//if (FRTOS1_xTaskCreate(TASK_accelerationMeasure, "accelerationMeasure", 800, NULL, tskIDLE_PRIORITY+3, NULL) != pdPASS) { ASSERT(false); }
#endif

	eventQueue.setEvent(Event::SystemStartup);
	RTOS_Run();
}

#if PL_HAS_RADIO
void APP_DebugPrint(unsigned char *str) {
#if PL_HAS_SHELL
  //CLS1_SendStr(str, CLS1_GetStdio()->stdOut);
	//*getConsole().getUnderlyingIoStream()<< str;
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
}

void systemReady(void){
    LED1_On();
    WAIT1_WaitOSms(10);
    LED1_Off();
    WAIT1_WaitOSms(10);
    LED1_On();
    WAIT1_WaitOSms(10);
    LED1_Off();
#if PL_HAS_ACCEL
    ACCEL_LowLevelInit();
#endif
#if PL_HAS_MUSIC_SHIELD
#if PL_HAS_SD_CARD
  FAT1_Init();
  FAT1_FRESULT fres;
  fres = FAT1_mount(&fileSystemObject, (const TCHAR*)"0" /* drive */, 0);
  if (fres != FR_OK) {
      for(;;){}
  }
#endif
    //WAIT1_WaitOSms(300);
  	MUSIC_PlayTheme(MUSIC_AHH);
    return;
#endif

#if PL_HAS_BUZZER
    constexpr auto baseFreq = 33;
    constexpr auto baseTime = 60;
    //return;

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
