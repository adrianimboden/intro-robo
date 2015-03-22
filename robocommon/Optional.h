#pragma once

#include <type_traits>

template <typename T>
class optional
{
public:
	optional() = default;

	template <typename... Args>
	optional(Args... args)
	{
		new(&storage) T(args...);
		isInitialized = true;
	}

	~optional()
	{
		reset();
	}

	optional(const optional& other)
	{
		if (other)
		{
			new(&storage) T(*other); //copy constructor
			isInitialized = true;
		}
	}

	optional(optional&& other)
	{
		if (other)
		{
			new(&storage) T(std::move(*other)); //copy constructor
			isInitialized = true;
		}
	}

	optional& operator=(const optional& other)
	{
		if (this != &other)
		{
			reset();
			if (other)
			{
				new(&storage) T(*other); //copy constructor
				isInitialized = true;
			}
		}
	}

	optional& operator=(optional&& other)
	{
		if (this != &other)
		{
			reset();
			if (other)
			{
				new(&storage) T(std::move(*other)); //copy constructor
				isInitialized = true;
			}
		}
	}

	bool is_initialized() const
	{
		return isInitialized;
	}

	explicit operator bool() const
	{
		return isInitialized;
	}

	void reset()
	{
		if (isInitialized)
		{
			operator*().~T();
			isInitialized = false;
		}
	}

	T& operator *()
	{
		return *static_cast<T*>(static_cast<void*>(&storage));
	}

	const T& operator *() const
	{
		return *static_cast<const T*>(static_cast<const void*>(&storage));
	}

private:
	bool isInitialized = false;
	typename std::aligned_storage<sizeof(T), alignof(T)>::type storage;
};
