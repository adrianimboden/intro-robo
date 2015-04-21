#include "MainControl.h"

#include <Motor.h>
#include <Reflectance.h>
#include <FreeRTOS.h>

MainControl MainControl::globalMainControl;

void MainControl::task(void*)
{
	setSpeed(10);
	for (;;)
	{
		if (REF_GetLineValue() < 100)
		{
			notifyEdgeDetected();
		}

		globalMainControl.step();
	}
}

void MainControl::notifyEdgeDetected()
{
	globalMainControl.edgeDetected.store(true);
}

void MainControl::notifyStartMove()
{
	globalMainControl.startMove.store(true);
}


void MainControl::setSpeed(int8_t wantedSpeed)
{
	globalMainControl.speed.store(wantedSpeed);
}

void MainControl::step()
{
	bool hasEdgeDetected = [&]
	{
		if (edgeDetected.load())
		{
			edgeDetected.store(false);
			return true;
		}
		return false;
	}();

	bool hasStartMove = [&]
	{
		if (startMove.load())
		{
			startMove.store(false);
			return true;
		}
		return false;
	}();

	auto wantedSpeed = speed.load();

	auto idle = [&]
	{
		vTaskDelay(1000);
		if (hasStartMove)
		{
			MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT), wantedSpeed);
			MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), wantedSpeed);

			return State::Moving;
		}
		return State::Idle;
	};

	auto moving = [&]
	{
		if (hasEdgeDetected)
		{
			MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT), 0);
			MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), 0);
			return State::SafeBackup;
		}

		return State::Moving;
	};

	auto safeBackup = [&]
	{
		MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT), -wantedSpeed);
		MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), -wantedSpeed);
		vTaskDelay(100);
		MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT), 0);
		MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), 0);
		return State::Idle;
	};

	switch (state)
	{
	case State::Idle: state = idle(); return;
	case State::Moving: state = moving(); return;
	case State::SafeBackup: state = safeBackup(); return;
	}
}
