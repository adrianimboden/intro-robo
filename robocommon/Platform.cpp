/*!
 * \file Platform.c
 * \brief Platform implementation module.
 *
 * This implements the platform module.
 * Here the platform gets initialized.
 */

#include "Platform.h"
#include "LED.h"
#include "Timer.h"
#include "Keys.h"
#include "Trigger.h"
#include "Buzzer.h"
#include "Debounce.h"
#include "RTOS.h"
#include "Reflectance.h"
#include "Motor.h"
#include "NVM_Config.h"
#include "QuadCalib.h"
#include "Ultrasonic.h"
#include "Accel.h"
#include "RNet_App.h"
#include "Remote.h"
extern "C"{

#if PL_HAS_MOTOR_QUAD
#include "Q4CRight.h"
#include "Q4CLeft.h"
#endif
}
#if PL_HAS_MOTOR_TACHO
#include "Tacho.h"
#endif
#if PL_HAS_DRIVE
#include "Drive.h"
#endif
#if PL_HAS_PID
#include "Pid.h"
#endif

#if PL_HAS_MUSIC_SHIELD
#include "Music.h"
#endif

#if PL_HAS_SPI
extern "C"{
  #include "SPI.h"
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

void PL_Init(void){

#if PL_HAS_LED
	LED_Init();
#endif
#if PL_HAS_EVENT
	/* do nothing */
#endif
#if PL_HAS_TIMER
	TMR_Init();
#endif
#if PL_HAS_KEYS
	KEY_Init();
#endif
#if PL_HAS_TRIGGER
  TRG_Init();
#endif
#if PL_HAS_BUZZER
  BUZ_Init();
#endif
#if PL_HAS_DEBOUNCE
  DBNC_Init();
#endif
#if PL_HAS_RTOS
  RTOS_Init();
#endif
#if PL_HAS_LINE_SENSOR
  REF_Init();
#endif
#if PL_HAS_MOTOR
  MOT_Init();
#endif
#if PL_HAS_CONFIG_NVM
  NVMC_Init();
#endif
#if PL_HAS_QUAD_CALIBRATION
  //
#endif
#if PL_HAS_MOTOR_QUAD
  Q4CLeft_Init();
  Q4CRight_Init();
#endif
#if PL_HAS_MOTOR_TACHO
  TACHO_Init();
#endif
#if PL_HAS_DRIVE
  DRV_Init();
#endif
#if PL_HAS_PID
  PID_Init();
#endif
#if PL_HAS_ULTRASONIC
  US_Init();
#endif
#if PL_HAS_ACCEL
  ACCEL_Init();
#endif
#if PL_HAS_RADIO
  RNETA_Init();
#endif
#if PL_HAS_REMOTE
  REMOTE_Init();
#endif
#if PL_HAS_MUSIC_SHIELD
  MUSIC_Init();
#endif
#if PL_HAS_SPI
  SPI_Init();
#endif
}

void PL_Deinit(void){
#if PL_HAS_SPI
  SPI_Deinit();
#endif
#if PL_HAS_MUSIC_SHIELD
  MUSIC_Deinit();
#endif
#if PL_HAS_REMOTE
  REMOTE_Deinit();
#endif
#if PL_HAS_RADIO
  RNETA_Deinit();
#endif
#if PL_HAS_ACCEL
  ACCEL_Deinit();
#endif
#if PL_HAS_ULTRASONIC
  US_Deinit();
#endif
#if PL_HAS_PID
  PID_Deinit();
#endif
#if PL_HAS_DRIVE
  DRV_Deinit();
#endif
#if PL_HAS_MOTOR_TACHO
	TACHO_Deinit();
#endif

#if PL_HAS_MOTOR_QUAD
  Q4CLeft_Deinit();
  Q4CRight_Deinit();
#endif

#if PL_HAS_QUAD_CALIBRATION
  //
#endif
#if PL_HAS_CONFIG_NVM
  NVMC_Deinit();
#endif
#if PL_HAS_MOTOR
  MOT_Deinit();
#endif
#if PL_HAS_LINE_SENSOR
  REF_Deinit();
#endif
#if PL_HAS_RTOS
  RTOS_Deinit();
#endif
#if PL_HAS_DEBOUNCE
  DBNC_Deinit();
#endif
#if PL_HAS_BUZZER
  BUZ_Init();
#endif
#if PL_HAS_TRIGGER
  TRG_Init();
#endif
#if PL_HAS_KEYS
	KEY_Deinit();
#endif
#if PL_HAS_TIMER
	TMR_Deinit();
#endif
#if PL_HAS_EVENT
	/* do nothing */
#endif
#if PL_HAS_LED
	LED_Deinit();
#endif

}

#ifdef __cplusplus
}  /* extern "C" */
#endif


