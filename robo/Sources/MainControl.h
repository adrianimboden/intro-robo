#pragma once

#include <CircularBuffer.h>
#include <atomic>

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

	static void notifyEdgeDetected();
	static void notifyStartMove();

	static void setSpeed(int8_t wantedSpeed);

private:
	void step();

private:
	std::atomic_bool edgeDetected;
	std::atomic_bool startMove;
	State state;

	std::atomic<int8_t> speed;

	static MainControl globalMainControl;
};

