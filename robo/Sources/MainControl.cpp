#include "MainControl.h"

#include <Drive.h>
#include <Reflectance.h>
#include <FreeRTOS.h>
#include <LED.h>
#include <Timer.h>
#include <WAIT1.h>

#include <BehaviourMachine.h>
#include "RoboConsole.h"
#include <random>

constexpr auto MAX_TURNING_SPEED = 25*74;
constexpr auto MAX_FIGHT_SPEED = 100*74;
constexpr auto START_FIGHT_SPEED = 100*74;

constexpr auto EnemyDistanceLimit = 50;
constexpr auto MAX_SPEED = 100*74;

static bool shouldTurn = false;

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
	struct ScanVariant
	{
		ScanVariant(uint32_t timeoutMs, double left, double right)
		: timeoutMs(timeoutMs)
		, left(left*MAX_SPEED)
		, right(right*MAX_SPEED)
		{
		}

		uint32_t timeoutMs;
		int32_t left;
		int32_t right;
	};

	std::array<ScanVariant, 6> scanVariants = {{
		ScanVariant{1000, 0.2, 0.3},
		ScanVariant{1000, 0.3, 0.2},
		ScanVariant{1000, 0.2, 0.4},
		ScanVariant{1000, 0.4, 0.2},
		ScanVariant{500, 0.0, 0.7},
		ScanVariant{500, 0.7, 0.0},
	}};

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
			auto scanVariant = scanVariants[currentStrategy];
			DRV_SetSpeed(scanVariant.left, scanVariant.right);
			if ((TMR_ValueMs() - startStrategyTime) > scanVariant.timeoutMs)
			{
				startStrategyTime = TMR_ValueMs();
				std::uniform_int_distribution<uint8_t> distribution(0, scanVariants.size()-1);
				currentStrategy = distribution(randomGenerator);
				*getConsole().getUnderlyingIoStream() << "new variant: " << static_cast<uint32_t>(currentStrategy) << "\n";
			}
		}
	}

private:
	uint32_t startStrategyTime = 0;
	std::ranlux24 randomGenerator;
	uint8_t currentStrategy = 0;
};

class ConstantSpeedFightBehaviour
{
public:
	bool wantsToTakeControl() const
	{
		return isFighting || ((MainControl::getEnemyDistance()<EnemyDistanceLimit) && MainControl::hasStartMove());
	}

	void step(bool suppress)
	{
		if (suppress)
		{
			isFighting = false;
			DRV_SetSpeed(0,0);
		}
		else
		{
			if (!isFighting || (MainControl::getEnemyDistance()<EnemyDistanceLimit))
			{
				startFightTime = TMR_ValueMs();
				isFighting = true;
				DRV_SetSpeed(MAX_FIGHT_SPEED, MAX_FIGHT_SPEED);
			}
			else
			{
				if ((TMR_ValueMs() - startFightTime) > 500)
				{
					isFighting = false;
				}
			}
		}
	}

private:
	bool isFighting = false;
	uint32_t startFightTime = 0;
};

class StaggeringFightBehaviour
{
public:
	bool wantsToTakeControl() const
	{
		return ((MainControl::getEnemyDistance()<EnemyDistanceLimit) && MainControl::hasStartMove());
	}

	void step(bool suppress)
	{
		if (suppress)
		{
			state = State::Start;
			DRV_SetSpeed(0,0);
		}
		else
		{
			constexpr auto speedPerMSec = (80*76/1000);

			int32_t maxSpeed = MainControl::getSpeed();
			if (maxSpeed == 0) maxSpeed = MAX_FIGHT_SPEED;

			int32_t speed = START_FIGHT_SPEED + ((TMR_ValueMs() - stateRampTime)) * speedPerMSec;
			if (speed >= maxSpeed) speed = MAX_FIGHT_SPEED;

			switch (state)
			{
			case State::Start:
				stateRampTime = TMR_ValueMs();
				state = State::Ramp;
				break;
			case State::Ramp:
				DRV_SetSpeed(speed,speed);
				state = (speed == maxSpeed) ? State::StartMax : State::Ramp;
				break;
			case State::StartMax:
				stateMaxTime = TMR_ValueMs();
				state = State::Max;
			case State::Max:
				DRV_SetSpeed(maxSpeed,maxSpeed);
				if (stateMaxTime > 50)
				{
					state = State::StartBackMax;
				}
				break;
			case State::StartBackMax:
				stateMaxTime = TMR_ValueMs();
				state = State::BackMax;
				break;
			case State::BackMax:
				DRV_SetSpeed(-maxSpeed/10,-maxSpeed/10);
				if (stateMaxTime > 10)
				{
					state = State::StartMax;
				}
				break;
			}
		}
	}

private:
	enum class State
	{
		Start,
		Ramp,
		StartMax,
		Max,
		StartBackMax,
		BackMax
	};

	State state = State::Start;
	uint32_t stateRampTime = 0;
	uint32_t stateMaxTime = 0;
};

class StopBehaviour
{
	enum class State
	{
		Idle,
		StartReversing,
		Reversing,
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
			DRV_SetSpeed(0,0);
		}
		else
		{
			state = [&]()->State
			{
				switch (state)
				{
				case State::Idle: return idle();
				case State::StartReversing: return startReversing();
				case State::Reversing: return reversing();
				}
				ASSERT(false);
				return State::Idle;
			}();
		}
	}

	State idle()
	{
		return State::StartReversing;
	}

	State startReversing()
	{
		SpeedState nonZeroSpeedState{static_cast<int32_t>(MAX_SPEED), static_cast<int32_t>(MAX_SPEED)};
		//SpeedState nonZeroSpeedState{0, 0};
		for (auto lastSpeed : DRV_GetLastSpeeds())
		{
			if (lastSpeed.left > 0 && lastSpeed.right > 0)
			{
				nonZeroSpeedState = lastSpeed;
			}
		}

		DRV_SetSpeed(-0.7*nonZeroSpeedState.left, -0.7*nonZeroSpeedState.right);
		startReversingTime = TMR_ValueMs();
		return State::Reversing;
	}

	State reversing()
	{
		if ((TMR_ValueMs() - startReversingTime) < 300)
		{
			return State::Reversing;
		}
		else
		{
			DRV_SetSpeed(0, 0);
			shouldTurn = true;
			return State::Idle;
		}
	}

private:
	State state = State::Idle;
	uint32_t startReversingTime;
};

class TurnBehaviour
{
	enum class State
	{
		Idle,
		StartTurning,
		Turning
	};

public:
	bool wantsToTakeControl() const
	{
		return ((state != State::Idle) || (MainControl::hasStartMove() && shouldTurn));
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
		return State::StartTurning;
	}


	State startTurning()
	{
		startTurningTime = TMR_ValueMs();
		return State::Turning;
	}

	State turning()
	{
		if ((TMR_ValueMs()-startTurningTime < 900))
		{
			DRV_SetSpeed(-MAX_TURNING_SPEED,MAX_TURNING_SPEED);
			return State::Turning;
		}
		else
		{
			DRV_SetSpeed(0, 0);
			shouldTurn = false;
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
		TurnBehaviour(),
		ConstantSpeedFightBehaviour(),
		StopBehaviour(),
		StopMotorsBehaviour())
	);

	//uint8_t counter = 0;
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
	if (cm < EnemyDistanceLimit)
	{
		*getConsole().getUnderlyingIoStream() << cm << "\n";
	}

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
