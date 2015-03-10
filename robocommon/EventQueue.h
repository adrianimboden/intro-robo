#pragma once

#ifndef __cplusplus
#error sorry, this header is c++ only
#endif

#include <array>
#include <type_traits>

namespace detail
{
	template <typename EventEnumType>
	struct Traits
	{
		using ArrayElementType = uint32_t;

		static constexpr size_t ArrayElementTypeBits = sizeof(ArrayElementType) * CHAR_BIT;

		using EventUnderlyingType = typename std::underlying_type<EventEnumType>::type;

		static constexpr size_t getAmountOfArrayEntries()
		{
			return static_cast<EventUnderlyingType>(EventEnumType::AmountOfEvents) / ArrayElementTypeBits + 1;
		}

		static constexpr ArrayElementType getBitmaskFor(EventEnumType event)
		{
			return 0x1 << (static_cast<EventUnderlyingType>(event) % ArrayElementTypeBits);
		}

		//using EventQueueArray = std::array<ArrayElementType, getAmountOfArrayEntries()>;
		using EventQueueArray = ArrayElementType[getAmountOfArrayEntries()];
	};
}

template <typename EventEnumType, typename GlobalLockGuard>
class EventQueue
{
public:
	void setEvent(EventEnumType event)
	{
		GlobalLockGuard lock;
		auto eventNo = static_cast<typename Traits::EventUnderlyingType>(event);
		data[eventNo / Traits::ArrayElementTypeBits] |= Traits::getBitmaskFor(event);
	}

	bool getAndResetEvent(EventEnumType event)
	{
		GlobalLockGuard lock;
		auto eventNo = static_cast<typename Traits::EventUnderlyingType>(event);
		auto isSet = data[eventNo / Traits::ArrayElementTypeBits] & Traits::getBitmaskFor(event);
		data[eventNo / Traits::ArrayElementTypeBits] &= ~Traits::getBitmaskFor(event);

		return isSet;
	}

private:
	using Traits = detail::Traits<EventEnumType>;

private:
	typename Traits::EventQueueArray data{};
};
