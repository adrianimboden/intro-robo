#pragma once

#ifndef __cplusplus
#error sorry, this header is c++ only
#endif

#include <FixedSizeString.h>
#include <NumberConversion.h>
#include <cstring>

using ConsoleChar = unsigned char;

template <typename T>
struct is_constsize_string : public std::false_type { };
template <size_t MaxSize>
struct is_constsize_string<String<MaxSize>> : public std::true_type { };

template <typename T>
struct is_stdarray : public std::false_type { };
template <typename TVal, size_t Size>
struct is_stdarray<std::array<TVal, Size>> : public std::true_type { };

class DiscardInputStrategy
{
public:
	void rxChar(char /*c*/)
	{
	}
};

class Console
{
public:
	virtual void rxChar(ConsoleChar c) = 0;

	template <typename T>
	uint32_t write(const T& val)
	{
		return writeImpl<T>(val);
	}

	uint32_t write(const uint8_t* pData, size_t size)
	{
		for (auto i = size_t{0}; i < size; ++i)
		{
			writeChar(pData[i]);
		}
		return size;
	}

	template <typename T, size_t Size>
	uint32_t write(T (&data)[Size])
	{
		uint32_t size = 0;
		for (auto i = size_t{0}; i < Size; ++i)
		{
			if (data[i] == '\0')
			{ //assume that it is a string in this case
				break;
			}
			++size;
			writeChar(data[i]);
		}
		return size;
	}

	template <typename T, size_t Size>
	uint32_t writeRaw(T (&data)[Size])
	{
		for (auto i = size_t{0}; i < Size; ++i)
		{
			writeChar(data[i]);
		}
		return Size;
	}

	uint32_t write(char c)
	{
		writeChar(c);
		return 1;
	}

	uint32_t write(unsigned char c)
	{
		writeChar(c);
		return 1;
	}

private:
	template <typename T>
	uint32_t writeImpl(typename std::enable_if<std::is_integral<T>::value, T>::type number)
	{
		uint32_t size = 0;
		for (auto c : numberToString(number))
		{
			++size;
			writeChar(c);
		}
		return size;
	}

	template <typename T>
	uint32_t writeImpl(typename std::enable_if<std::is_same<T, const char*>::value, T>::type pStr)
	{
		auto len = strlen(pStr);
		for (auto i = size_t{0}; i < len; ++i)
		{
			writeChar(pStr[i]);
		}
		return len;
	}

	template <typename T>
	uint32_t writeImpl(typename std::enable_if<std::is_same<T, const unsigned char*>::value, T>::type pStr)
	{
		auto len = strlen(pStr);
		for (auto i = size_t{0}; i < len; ++i)
		{
			writeChar(pStr[i]);
		}
		return len;
	}

	template <typename T>
	uint32_t writeImpl(const typename std::enable_if<is_constsize_string<T>::value, T>::type& str)
	{
		uint32_t size = 0;
		for (auto c : str)
		{
			++size;
			writeChar(c);
		}

		return size;
	}

	template <typename T>
	uint32_t writeImpl(const typename std::enable_if<is_stdarray<T>::value, T>::type& arr)
	{
		for (auto c : arr)
		{
			writeChar(c);
		}
		return arr.size();
	}


protected:
	virtual void writeChar(unsigned char c) = 0;
};

template <typename WriteTxCharFn, typename ConsoleInputStrategy = DiscardInputStrategy>
class ConcreteConsole : public Console
{
public:
	using type = ConcreteConsole<WriteTxCharFn, ConsoleInputStrategy>;

	explicit ConcreteConsole(WriteTxCharFn writeTxCharFn, ConsoleInputStrategy inputStrategy = {})
		: writeTxCharFn(std::move(writeTxCharFn))
		, inputStrategy(std::move(inputStrategy))
	{
	}

	//! to be called when a character has been received (eg. from interrupt)
	void rxChar(ConsoleChar c)
	{
		inputStrategy.rxChar(c);
	}

protected:
	void writeChar(unsigned char c) override
	{
		writeTxCharFn(c);
	}

private:
	WriteTxCharFn writeTxCharFn;
	ConsoleInputStrategy inputStrategy;
};
