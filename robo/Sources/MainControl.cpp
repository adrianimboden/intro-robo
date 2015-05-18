#include "MainControl.h"

#include <Drive.h>
#include <Reflectance.h>
#include <FreeRTOS.h>
#include <LED.h>
#include <Timer.h>
#include <WAIT1.h>
#include "Remote.h"
#if PL_HAS_MUSIC_SHIELD
#include "Music.h"
#endif

#include <BehaviourMachine.h>

constexpr auto MAX_TURNING_SPEED = 25*74;
constexpr auto MAX_BACKUP_SPEED = -60*74;

MainControl MainControl::globalMainControl;

class StopMotorsBehaviour
{
public:
	bool wantsToTakeControl() const
	{
		return MainControl::hasStopMotors();
	}

	void step(bool suppress)
	{
		if (suppress)
		{
			DRV_SetSpeed(0,0);
		}
		else
		{
			DRV_SetSpeed(0,0);
			MainControl::notifyStartMove(false);
		}
	}
};

class ScanEnemyBehaviour
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
			DRV_SetSpeed(0,0);
		}
		else
		{
			DRV_SetSpeed(-MAX_TURNING_SPEED,MAX_TURNING_SPEED);
		}
	}
};

class ForwardBehaviour
{
public:
	bool wantsToTakeControl() const
	{
		return ((MainControl::getEnemyDistance()<80) && MainControl::hasStartMove());
	}

	void step(bool suppress)
	{
		if (suppress)
		{
			DRV_SetSpeed(0,0);
		}
		else
		{
			auto speed = MainControl::getSpeed();
			if(speed == 0) speed = 60*74;
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
		StartBackup,
		Backup,
		StartTurning,
		Turning
	};

public:
	bool wantsToTakeControl() const
	{
		return ((state != State::Idle) || (MainControl::hasStartMove() && MainControl::hasEdgeDetected()) || (MainControl::hasEdgeDetected() && REMOTE_GetOnOff()));
	}

	void step(bool suppress)
	{
		if (suppress)
		{
			state = State::Idle;
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
				case State::StartBackup: return startBackup();
				case State::Backup: return backup();
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
		DRV_SetSpeed(0,0);
#if PL_HAS_MUSIC_SHIELD
		if(!MUSIC_IsPlaying())	MUSIC_PlayTheme(MUSIC_FOOL);
#endif
		return State::StartBackup;
	}

	State startBackup()
	{
		startTurningTime = TMR_ValueMs();
		return State::Backup;
	}

	State backup()
	{
		if ((TMR_ValueMs()-startTurningTime < 100))
		{
			DRV_SetSpeed(MAX_BACKUP_SPEED,MAX_BACKUP_SPEED);
			return State::Backup;
		}
		else
		{
			DRV_SetSpeed(0, 0);
			return State::StartTurning;
		}
	}

	State startTurning()
	{
		startTurningTime = TMR_ValueMs();
		return State::Turning;
	}

	State turning()
	{
		if ((TMR_ValueMs()-startTurningTime < 300))
		{
			DRV_SetSpeed(-MAX_TURNING_SPEED,MAX_TURNING_SPEED);
			return State::Turning;
		}
		else
		{
			DRV_SetSpeed(0, 0);
			return State::Idle;
		}
	}

private:
	State state = State::Idle;
	uint32_t startTurningTime;
};

void MainControl::task(void*)
{
	auto arbitrator = makeArbitrator(std::make_tuple(
		ScanEnemyBehaviour(),
		ForwardBehaviour(),
		StopBehaviour(),
		StopMotorsBehaviour())
	);

	uint8_t counter = 0;
	static uint8_t waitAtPowerUp = 0;
	for (;;)
	{
		if(waitAtPowerUp==0) { WAIT1_Waitms(1000); waitAtPowerUp = 1;}
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

void MainControl::notifyStopMotors(bool stop)
{
	globalMainControl.stopMotors.store(stop);
}

void MainControl::notifyEnemyDetected(uint16_t cm)
{
	globalMainControl.enemyDistance.store(cm);
}


void MainControl::setSpeed(int8_t wantedSpeed)
{
	globalMainControl.speed.store(wantedSpeed);
}

bool MainControl::hasEdgeDetected()
{
	return globalMainControl.edgeDetected.load();
}

uint16_t MainControl::getEnemyDistance()
{
	return globalMainControl.enemyDistance.load();
}

int16_t MainControl::getSpeed()
{
	return globalMainControl.speed.load()*74; //7450 are the max ticks per rounds pro sec.
}

bool MainControl::hasStartMove()
{
	return globalMainControl.startMove.load();
}

bool MainControl::hasStopMotors()
{
	return globalMainControl.stopMotors.load();
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
