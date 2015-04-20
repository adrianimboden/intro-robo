/**
 * \file
 * \brief Motor driver implementation.
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This module a driver for up to two small DC motors.
 */

#include "Platform.h"
#if PL_HAS_MOTOR
#include "Motor.h"
#include "DIRR.h"
#include "DIRL.h"
#include "PWMR.h"
#include "PWML.h"
#include "UTIL1.h"

static MOT_MotorDevice motorL, motorR;

MOT_MotorDevice *MOT_GetMotorHandle(MOT_MotorSide side) {
  if (side==MOT_MOTOR_LEFT) {
    return &motorL;
  } else {
    return &motorR;
  }
}

static uint8_t PWMLSetRatio16(uint16_t ratio) {
  return PWML_SetRatio16(ratio);
}

static uint8_t PWMRSetRatio16(uint16_t ratio) {
  return PWMR_SetRatio16(ratio);
}

static void DirLPutVal(bool val) {
  DIRL_PutVal(!val);
}

static void DirRPutVal(bool val) {
  DIRR_PutVal(val);
}

void MOT_SetVal(MOT_MotorDevice *motor, uint16_t val) {
  motor->currPWMvalue = val;
  motor->SetRatio16(val);
}

uint16_t MOT_GetVal(MOT_MotorDevice *motor) {
  return motor->currPWMvalue;
}

void MOT_SetSpeedPercent(MOT_MotorDevice *motor, MOT_SpeedPercent percent) {
  /*! \todo See lab guide about this function */
  uint32_t val;

  if (percent>100) { /* make sure we are within 0..100 */
    percent = 100;
  } else if (percent<-100) {
    percent = -100;
  }
  motor->currSpeedPercent = percent; /* store value */
  if (percent<0) {
    MOT_SetDirection(motor, MOT_DIR_BACKWARD);
    percent = -percent; /* make it positive */
  } else {
    MOT_SetDirection(motor, MOT_DIR_FORWARD);
  }
  val = ((100-percent)*0xffff)/100; /* H-Bridge is low active! */
  MOT_SetVal(motor, (uint16_t)val);
}

void MOT_UpdatePercent(MOT_MotorDevice *motor, MOT_Direction dir) {
  motor->currSpeedPercent = ((0xffff-motor->currPWMvalue)*100)/0xffff;
  if (dir==MOT_DIR_BACKWARD) {
    motor->currSpeedPercent = -motor->currSpeedPercent;
  }
}

void MOT_ChangeSpeedPercent(MOT_MotorDevice *motor, MOT_SpeedPercent relPercent) {
  relPercent += motor->currSpeedPercent; /* make absolute number */
  if (relPercent>100) { /* check for overflow */
    relPercent = 100;
  } else if (relPercent<-100) { /* and underflow */
    relPercent = -100;
  }
  MOT_SetSpeedPercent(motor, relPercent);  /* set speed. This will care about the direction too */
}

void MOT_SetDirection(MOT_MotorDevice *motor, MOT_Direction dir) {
  /*! \todo Check if directions are working properly with your hardware */
  if (dir==MOT_DIR_BACKWARD) {
    motor->DirPutVal(0);
    if (motor->currSpeedPercent>0) {
      motor->currSpeedPercent = -motor->currSpeedPercent;
    }
  } else if (dir==MOT_DIR_FORWARD) {
    motor->DirPutVal(1);
    if (motor->currSpeedPercent<0) {
      motor->currSpeedPercent = -motor->currSpeedPercent;
    }
  }
}

MOT_Direction MOT_GetDirection(MOT_MotorDevice *motor) {
  if (motor->currSpeedPercent<0) {
    return MOT_DIR_BACKWARD;
  } else {
    return MOT_DIR_FORWARD;
  }
}

void MOT_CmdStatus(IOStream& ioStream)
{
	ioStream << "Motor\r\n";
	ioStream << " Motor L: " << motorL.currSpeedPercent << "%" << " 0x" << numberToHex(MOT_GetVal(&motorL)) << "\r\n";
	ioStream << " Motor R: " << motorR.currSpeedPercent << "%" << " 0x" << numberToHex(MOT_GetVal(&motorR)) << "\r\n";
}

void MOT_CmdDir(IOStream& out, const String<1>& motor, const String<8>& cmd)
{
	if (motor != 'L' && motor != 'R')
	{
		out << "usage: cmd L|R forward|backward\r\n";
		return;
	}
	MOT_MotorDevice& motorDev = (motor == 'L') ? motorL : motorR;

	if (cmd != "forward" || cmd != "backward")
	{
		out << "usage: cmd L|R forward|backward\r\n";
	}
	enum class Dir{Forward, Backward};
	Dir dir = (cmd == "forward" ? Dir::Forward : Dir::Backward);

	if (dir == Dir::Forward)
	{
		MOT_SetDirection(&motorDev, MOT_DIR_FORWARD);
	}
	else
	{
		MOT_SetDirection(&motorDev, MOT_DIR_BACKWARD);
	}
}

void MOT_CmdDuty(IOStream& out, const String<1>& motor, int8_t duty)
{
	if (motor != 'L' && motor != 'R')
	{
		out << "usage: cmd L|R 0-100\r\n";
		return;
	}
	MOT_MotorDevice& motorDev = motor == 'L' ? motorL : motorR;

	if (duty > 100)
	{
		out << "usage: cmd L|R 0-100\r\n";
		return;
	}

    MOT_SetSpeedPercent(&motorDev, duty);
}

void MOT_Deinit(void) {
  /*! \todo What could you do here? */
}

void MOT_Init(void) {
  motorL.DirPutVal = DirLPutVal;
  motorR.DirPutVal = DirRPutVal;
  motorL.SetRatio16 = PWMLSetRatio16;
  motorR.SetRatio16 = PWMRSetRatio16;
  MOT_SetSpeedPercent(&motorL, 0);
  MOT_SetSpeedPercent(&motorR, 0);
  PWML_Enable();
  PWMR_Enable();
}
#endif /* PL_HAS_MOTOR */
