#pragma once

#ifndef __cplusplus
#error sorry, this header is c++ only
#endif

#include <limits>

namespace detail
{
	template <typename T, typename NextLower, size_t MaxSize>
	using EnableInRange = typename std::enable_if<
		(MaxSize <= std::numeric_limits<T>::max() &&
		MaxSize > (std::is_same<NextLower, void>::value ?
				0 :
				std::numeric_limits<typename std::conditional<std::is_same<NextLower, void>::value, T, NextLower>::type>::max()
			)
		), T>::type;

	template <size_t MaxSize> auto findSmallestIntegerFor() -> EnableInRange<uint8_t, void, MaxSize>;
	template <size_t MaxSize> auto findSmallestIntegerFor() -> EnableInRange<uint16_t, uint8_t, MaxSize>;
	template <size_t MaxSize> auto findSmallestIntegerFor() -> EnableInRange<uint32_t, uint16_t, MaxSize>;
}

/**
 * Returns the smalles integer type that can hold a value up to MaxSize
 */
template <size_t MaxSize>
struct FindSmallestIntegerFor
{
	using type = decltype(detail::findSmallestIntegerFor<MaxSize>());
};

static_assert(std::is_same<typename FindSmallestIntegerFor<1>::type, uint8_t>::value, "something is wrong in detail::findSmallestIntegerFor");
static_assert(std::is_same<typename FindSmallestIntegerFor<255>::type, uint8_t>::value, "something is wrong in detail::findSmallestIntegerFor");
static_assert(std::is_same<typename FindSmallestIntegerFor<256>::type, uint16_t>::value, "something is wrong in detail::findSmallestIntegerFor");
static_assert(std::is_same<typename FindSmallestIntegerFor<65535>::type, uint16_t>::value, "something is wrong in detail::findSmallestIntegerFor");
static_assert(std::is_same<typename FindSmallestIntegerFor<65536>::type, uint32_t>::value, "something is wrong in detail::findSmallestIntegerFor");
