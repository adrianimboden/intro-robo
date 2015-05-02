#include "MainControl.h"

//#include <Motor.h>
#include <Drive.h>
#include <Reflectance.h>
#include <FreeRTOS.h>
#include <LED.h>
#include <Timer.h>
#include <WAIT1.h>

#include <BehaviourMachine.h>

MainControl MainControl::globalMainControl;

class ForwardBehaviour
{
public:
	bool wantsToTakeControl() const
	{
		return MainControl::hasStartMove();
	}

	void step(bool suppress)
	{
		if (suppress)
		{
			//MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT), 0);
			//MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), 0);
			DRV_SetSpeed(0,0);
		}
		else
		{
			auto speed = MainControl::getSpeed();
			//MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT), speed);
			//MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), speed);
			DRV_SetSpeed(speed,speed);
		}
	}
};

class StopBehaviour
{
	enum class State
	{
		Idle,
		Stopped,
		StartTurning,
		Turning
	};

public:
	bool wantsToTakeControl() const
	{
		return ((state != State::Idle) || (MainControl::hasStartMove() && MainControl::hasEdgeDetected()));
	}

	void step(bool suppress)
	{
		if (suppress)
		{
			state = State::Idle;
			//MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT), 0);
			//MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), 0);
			DRV_SetSpeed(0,0);
		}
		else
		{
			state = [&]()->State
			{
				switch (state)
				{
				case State::Idle: return idle();
				case State::Stopped: return stopped();
				case State::StartTurning: return startTurning();
				case State::Turning: return turning();
				}
				ASSERT(false);
				return State::Idle;
			}();
		}
	}

	State idle()
	{
		return State::Stopped;
	}

	State stopped()
	{
		//MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT), 0);
		//MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), 0);
		DRV_SetSpeed(0,0);
		return State::StartTurning;
	}

	State startTurning()
	{
		startTurningTime = TMR_ValueMs();
		return State::Turning;
	}

	State turning()
	{
		if (!MainControl::hasEdgeDetected())
		{
			//MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT), 0);
			//MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), 0);
			DRV_SetSpeed(0,0);
			return State::Idle;
		}
		else
		{
			auto speed = MainControl::getSpeed();
			//MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT), speed/2);
			//MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), -speed/2);
			DRV_SetSpeed(speed/2,-speed/2);
			return State::Turning;
		}
	}

private:
	State state = State::Idle;
	uint32_t startTurningTime;
};

void MainControl::task(void*)
{
	auto arbitrator = makeArbitrator(std::make_tuple(
		ForwardBehaviour(),
		StopBehaviour())
	);

	uint8_t counter = 0;
	for (;;)
	{
		notifyEdgeDetected(REF_SeesLine());
		if (hasEdgeDetected())
		{
		    LED1_On();
		}
		else
		{
		    LED1_Off();
		}

		arbitrator.step();

		if (++counter % 100)
		{
			WAIT1_WaitOSms(1);
			counter = 0;
		}
	}
}

void MainControl::notifyEdgeDetected(bool detected)
{
	globalMainControl.edgeDetected.store(detected);
}

void MainControl::notifyStartMove(bool start)
{
	globalMainControl.startMove.store(start);
}


void MainControl::setSpeed(int8_t wantedSpeed)
{
	globalMainControl.speed.store(wantedSpeed);
}

bool MainControl::hasEdgeDetected()
{
	return globalMainControl.edgeDetected.load();
}

int16_t MainControl::getSpeed()
{
	return globalMainControl.speed.load()*74; //7450 are the max ticks per rounds pro sec.
}

bool MainControl::hasStartMove()
{
	return globalMainControl.startMove.load();
}

void MainControl::setConfig(Config config)
{
	ScopedGuard guard(globalMainControl.configMutex);
	globalMainControl.config = config;
}

Config MainControl::getConfig()
{
	ScopedGuard guard(globalMainControl.configMutex);
	return globalMainControl.config;
}
