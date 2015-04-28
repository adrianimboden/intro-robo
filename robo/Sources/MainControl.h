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

	static void setSpeed(int8_t wantedSpeed);

	static bool hasEdgeDetected();
	static int8_t getSpeed();
	static bool hasStartMove();

	static void setConfig(Config config);
	static Config getConfig();

private:
	std::atomic_bool edgeDetected;
	std::atomic_bool startMove;
	State state;

	Mutex configMutex;
	Config config;

	std::atomic<int8_t> speed;

	static MainControl globalMainControl;
};

