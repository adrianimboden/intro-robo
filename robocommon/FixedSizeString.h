#pragma once

#ifndef __cplusplus
#error sorry, this header is c++ only
#endif


#include <array>
#include <type_traits>
#include <cstring>

#include "Assert.h"
#include "CommonTraits.h"

enum class StringManipulationResult
{
	Ok,
	NotEnoughBufferMemory,
};

using CharT = char;

template <size_t MaxSize>
class String
{
public:
	using size_type = typename FindSmallestIntegerFor<MaxSize>::type;

	using iterator = CharT*;
	using const_iterator = const CharT*;

	explicit String()
	{
		currSize = 0;
#ifdef DEBUG
		pDebugStr = data.data();
#endif
	}

	explicit String(const CharT* pSrc, const CharT* pEnd)
	{
		ASSERT(pEnd >= pSrc);
		currSize = (pEnd - pSrc);
		for (auto i = 0u; i < (pEnd - pSrc); ++i)
		{
			data[i] = pSrc[i];
		}
#ifdef DEBUG
		pDebugStr = data.data();
#endif
	}

	explicit String(const String& other)
		: currSize(other.currSize)
		, data(other.data)
#ifdef DEBUG
		, pDebugStr(data.data())
#endif
	{
	}

	String& operator=(const String& other)
	{
		if (this != &other)
		{
			data = other.data;
			currSize = other.currSize;
		}
		return *this;
	}

	explicit String(String&& other)
		: currSize(other.currSize)
		, data(other.data)
#ifdef DEBUG
		, pDebugStr(data.data())
#endif
	{
	}

	String& operator=(String&& other)
	{
		if (this != &other)
		{
			data = other.data;
			currSize = other.currSize;
		}
		return *this;
	}

	explicit String(const CharT* pSrc, size_t srcSize)
		: String(pSrc, getEndPtr(pSrc, srcSize))
	{
	}

	String(const CharT* pSrc)
		: String(pSrc, getEndPtr(pSrc))
	{
	}

	template <int Size>
	String(const CharT arr[Size])
		: String(arr, size)
	{
	}

	String(CharT c)
		:
	#ifdef DEBUG
		pDebugStr(data.data()),
	#endif
		currSize(1)
	{
		data[0] = c;
	}

	//! implicit conversion to different size
	template <size_t MaxSizeOther>
	String(const String<MaxSizeOther>& other)
		: String(other.begin(), other.size())
	{
	}

	size_t size() const
	{
		return currSize;
	}

	bool empty() const
	{
		return currSize == 0;
	}

	CharT* begin()
	{
		return data.begin();
	}

	CharT* end()
	{
		return data.begin() + currSize;
	}

	const CharT* begin() const
	{
		return data.begin();
	}

	const CharT* end() const
	{
		return data.begin() + currSize;
	}

	CharT& operator[](size_t index)
	{
		ASSERT(index < size());
		return data[index];
	}

	CharT operator[](size_t index) const
	{
		ASSERT(index < size());
		return data[index];
	}

	StringManipulationResult append(CharT c)
	{
		if (currSize >= MaxSize)
		{
			return StringManipulationResult::NotEnoughBufferMemory;
		}
		data[currSize] = c;
		++currSize;

		return StringManipulationResult::Ok;
	}

	template <size_t MaxSizeOther>
	StringManipulationResult append(String<MaxSizeOther> toAppend)
	{
		auto len = toAppend.size();
		if (currSize + len > MaxSize)
		{
			return StringManipulationResult::NotEnoughBufferMemory;
		}
		for (auto i = size_t{0}; i < len; ++i)
		{
			data[currSize + i] = toAppend[i];
		}
		currSize += len;

		return StringManipulationResult::Ok;
	}

	StringManipulationResult append(const CharT* pStrToApend)
	{
		auto len = strlen(pStrToApend);
		if (currSize + len > MaxSize)
		{
			return StringManipulationResult::NotEnoughBufferMemory;
		}
		for (auto i = size_t{0}; i < len; ++i)
		{
			data[currSize + i] = pStrToApend[i];
		}
		currSize += len;

		return StringManipulationResult::Ok;
	}

	//! erases part of the string  (from 'from' to ('to'-1))
	void erase(size_type from, size_type to)
	{
		ASSERT(from <= to);

		to = std::min(to, currSize);
		if (from == to)
			return ;

		for (auto i = size_type{0}; i < (to - from); ++i)
		{
			auto src = (to + i);
			auto dst = from + i;
			data[dst] = (src >= currSize) ? '\0' : data[src];
		}
		for (auto pos = to; pos <= currSize; ++pos)
		{
			auto src = (pos + 1);
			data[pos] = (src >= currSize) ? '\0' : data[src];
		}
		currSize -= (to - from);
	}

	void erase()
	{
		currSize = 0;
	}

private:
	static const CharT* getEndPtr(const CharT* pSrc, size_t srcSize)
	{
		ASSERT(srcSize <= MaxSize);
		return pSrc + srcSize;
	}

	static const CharT* getEndPtr(const CharT* pSrc)
	{
		const CharT* pEnd = pSrc;

		while (*pEnd != '\0')
		{
			++pEnd;
		}

		ASSERT((pEnd - pSrc) <= MaxSize);

		return pEnd;
	}

private:
	size_type currSize;
	std::array<CharT, MaxSize + 1> data{};

#ifdef DEBUG
	CharT* pDebugStr;
#endif
};

template <size_t MaxSizeLhs, size_t MaxSizeRhs>
bool operator==(const String<MaxSizeLhs>& lhs, const String<MaxSizeRhs>& rhs)
{
	if (lhs.size() != rhs.size())
		return false;

	for (auto i = size_t{0}; i < lhs.size(); ++i)
	{
		if (lhs.begin()[i] != rhs.begin()[i])
			return false;
	}

	return true;
}

template <size_t MaxSizeLhs, size_t MaxSizeRhs>
bool operator!=(const String<MaxSizeLhs>& lhs, const String<MaxSizeRhs>& rhs)
{
	return !(lhs == rhs);
}


template <size_t OtherSize>
bool operator==(const String<OtherSize>& str, const CharT* pStr)
{
	auto pEnd = pStr;
	while (*pEnd != '\0')
		++pEnd;

	auto otherSize = (pEnd - pStr);

	if (str.size() != otherSize)
		return false;

	for (auto i = size_t{0}; i < str.size(); ++i)
	{
		if (str.begin()[i] != pStr[i])
			return false;
	}

	return true;
}

template <size_t OtherSize>
bool operator==(const CharT* pStr, const String<OtherSize>& str)
{
	return str == pStr;
}

template <size_t OtherSize>
bool operator!=(const String<OtherSize>& str, const CharT* pStr)
{
	return !(str == pStr);
}

template <size_t OtherSize>
bool operator!=(const CharT* pStr, const String<OtherSize>& str)
{
	return str != pStr;
}
