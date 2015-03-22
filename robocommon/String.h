#pragma once

#include <Assert.h>

#include <array>
#include <type_traits>

template <size_t MaxSize>
class String
{
public:
	explicit String(const char* pSrc, const char* pEnd)
	{
		currSize = (pEnd - pSrc);
		for (auto i = 0u; i < (pEnd - pSrc); ++i)
		{
			data[i] = pSrc[i];
		}
#ifdef DEBUG
		pDebugStr = &data[0];
#endif
	}

	explicit String(const char* pSrc, size_t srcSize)
		: String(pSrc, getEndPtr(pSrc, srcSize))
	{
	}

	String(const char* pSrc)
		: String(pSrc, getEndPtr(pSrc))
	{
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

	char* begin()
	{
		return data.begin();
	}

	char* end()
	{
		return data.begin() + currSize;
	}

	const char* begin() const
	{
		return data.begin();
	}

	const char* end() const
	{
		return data.begin() + currSize;
	}

	char& operator[](size_t index)
	{
		ASSERT(index < size());
		return data[index];
	}

	char operator[](size_t index) const
	{
		ASSERT(index < size());
		return data[index];
	}

private:
	static const char* getEndPtr(const char* pSrc, size_t srcSize)
	{
		ASSERT(srcSize <= MaxSize);
		return pSrc + srcSize;
	}

	static const char* getEndPtr(const char* pSrc)
	{
		const char* pEnd = pSrc;

		while (*pEnd != '\0')
		{
			++pEnd;
		}

		ASSERT((pEnd - pSrc) <= MaxSize);

		return pEnd;
	}

private:
	std::array<char, MaxSize + 1> data{};
#ifdef DEBUG
	char* pDebugStr;
#endif
	size_t currSize;
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
bool operator==(const String<OtherSize>& str, const char* pStr)
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
bool operator==(const char* pStr, const String<OtherSize>& str)
{
	return str == pStr;
}

template <size_t OtherSize>
bool operator!=(const String<OtherSize>& str, const char* pStr)
{
	return !(str == pStr);
}

template <size_t OtherSize>
bool operator!=(const char* pStr, const String<OtherSize>& str)
{
	return str != pStr;
}
