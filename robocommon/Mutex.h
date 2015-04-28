/*
 * Mutex
 *
 *  Created on: Apr 28, 2015
 *      Author: adrian
 */


#ifndef MUTEX_
#define MUTEX_

#include <FreeRTOS.h>
#include <semphr.h>


class Mutex
{
public:
	explicit Mutex()
	{
		semaphore = xSemaphoreCreateMutex();
	}

	~Mutex()
	{
		vSemaphoreDelete(semaphore);
	}

	Mutex(const Mutex&) = delete;
	Mutex& operator =(const Mutex&) = delete;

	Mutex(Mutex&&) = default;
	Mutex& operator =(Mutex&&) = default;

private:
	friend class ScopedGuard;
	SemaphoreHandle_t semaphore;
};

class ScopedGuard
{
public:
	explicit ScopedGuard(Mutex& mutex)
		: mutex(mutex)
	{
		xSemaphoreTake(mutex.semaphore, portMAX_DELAY);
	}

	~ScopedGuard()
	{
		xSemaphoreGive(mutex.semaphore);
	}

	ScopedGuard(const ScopedGuard&) = delete;
	ScopedGuard(ScopedGuard&&) = delete;
	ScopedGuard& operator =(const ScopedGuard&) = delete;
	ScopedGuard& operator =(ScopedGuard&&) = delete;

private:
	Mutex& mutex;
};




#endif /* MUTEX_ */
