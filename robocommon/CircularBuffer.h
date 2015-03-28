#pragma once

#ifndef __cplusplus
#error sorry, this header is c++ only
#endif

#include <array>

#include <Optional.h>

enum class CircularBufferFullStrategy
{
	DiscardPushedElement,
	OverwriteOldest,
};

class NoLockingStrategy
{
public:
	void* lock()
	{
		return nullptr;
	}
};

template <
		typename T,
		size_t MaxSize,
		CircularBufferFullStrategy FullStrategy = CircularBufferFullStrategy::DiscardPushedElement,
		typename LockingStrategy = NoLockingStrategy
		 >
class CircularBuffer
{
public:
	static_assert(MaxSize >= 1, "circular buffer must be at least one element big");

	using element_type = T;

	explicit CircularBuffer(LockingStrategy lockingStrategy = {})
		: lockingStrategy(lockingStrategy)
	{
	}

	void push_back(T element)
	{
		auto lock = lockingStrategy.lock();
		(void)lock;
		if (currSize >= MaxSize)
		{
			if (FullStrategy == CircularBufferFullStrategy::DiscardPushedElement)
			{
				return; //discard
			}
			if (FullStrategy == CircularBufferFullStrategy::OverwriteOldest)
			{
				impl_pop_front(); //take out oldest and discard it
			}
		}

		data[pushPos] = std::move(element);
		incrementAndWrapAround(pushPos);
		currSize += 1;
	}

	optional<T> pop_back()
	{
		auto lock = lockingStrategy.lock();
		(void)lock;

		if (currSize == 0)
		{
			return { };
		}

		decrementAndWrapAround(pushPos);
		currSize -= 1;

		return std::move(data[pushPos]);
	}

	optional<T> pop_front()
	{
		auto lock = lockingStrategy.lock();
		(void)lock;
		return impl_pop_front();
	}

	size_t size() const
	{
		auto lock = lockingStrategy.lock();
		return currSize;
	}

private:
	optional<T> impl_pop_front()
	{
		if (currSize == 0)
		{
			return { };
		}

		auto element = std::move(data[popPos]);

		incrementAndWrapAround(popPos);
		currSize -= 1;

		return element;
	}

	void incrementAndWrapAround(size_t& pos)
	{
		pos += 1;
		pos %= MaxSize;
	}

	void decrementAndWrapAround(size_t& pos)
	{
		pos -= 1;
		pos %= MaxSize;
	}

private:
	mutable LockingStrategy lockingStrategy;

	size_t currSize = 0;
	size_t pushPos = 0;
	size_t popPos = 0;
	std::array<T, MaxSize> data{};
};
