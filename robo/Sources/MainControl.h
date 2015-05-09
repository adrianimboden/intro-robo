#pragma once

#include <CircularBuffer.h>
#include <Mutex.h>
#include <atomic>

struct Config
{
	enum FleeDirection {
		Left, Right
	};

	FleeDirection fleeDir;
};

class MainControl
{
public:
	enum class State
	{
		Idle,
		Moving,
		SafeBackup
	};

public:
	static void task(void*);

	static void notifyEdgeDetected(bool detected);
	static void notifyStartMove(bool start);
	static void notifyEnemyDetected(uint16_t cm);
	static void notifyStopMotors(bool stop);

	static void setSpeed(int8_t wantedSpeed);

	static bool hasEdgeDetected();
	static int16_t getSpeed();
	static bool hasStartMove();
	static bool hasStopMotors();
	static uint16_t getEnemyDistance();

	static void setConfig(Config config);
	static Config getConfig();

private:
	std::atomic_bool edgeDetected;
	std::atomic_bool startMove;
	std::atomic_bool stopMotors;
	std::atomic_uint_fast16_t enemyDistance;
	State state;

	Mutex configMutex;
	Config config;

	std::atomic<int8_t> speed;

	static MainControl globalMainControl;
};

