#pragma once

#ifndef __cplusplus
#error sorry, this header is c++ only
#endif

#include <type_traits>

template <typename WriteCharFn>
class StreamHelper
{
public:
	/**
	 * Write a value
	 */
	template <typename T>
	static uint32_t write(const WriteCharFn& fnWriteChar, const T& val)
	{
		return writeImpl<T>(fnWriteChar, val);
	}

	/**
	 * Write an string (null terminated) with compile time known size
	 */
	template <typename T, size_t Size>
	static uint32_t write(const WriteCharFn& fnWriteChar, T (&data)[Size])
	{
		uint32_t size = 0;
		for (auto i = size_t{0}; i < Size; ++i)
		{
			if (data[i] == '\0')
			{ //assume that it is a string in this case
				break;
			}
			++size;
			fnWriteChar(data[i]);
		}
		return size;
	}

	/**
	 * Write a dynamic length array
	 */
	static uint32_t write(const WriteCharFn& fnWriteChar, const uint8_t* pData, size_t size)
	{
		for (auto i = size_t{0}; i < size; ++i)
		{
			fnWriteChar(pData[i]);
		}
		return size;
	}

	/**
	 * Write an array with compile time known size
	 */
	template <typename T, size_t Size>
	static uint32_t writeRaw(const WriteCharFn& fnWriteChar, T (&data)[Size])
	{
		for (auto i = size_t{0}; i < Size; ++i)
		{
			fnWriteChar(data[i]);
		}
		return Size;
	}

private:
	template <typename T>
	struct is_constsize_string : public std::false_type { };
	template <size_t MaxSize>
	struct is_constsize_string<String<MaxSize>> : public std::true_type { };

	template <typename T>
	struct is_stdarray : public std::false_type { };
	template <typename TVal, size_t Size>
	struct is_stdarray<std::array<TVal, Size>> : public std::true_type { };

	template <typename T>
	static uint32_t writeImpl(const WriteCharFn& fnWriteChar, typename std::enable_if<std::is_same<T, char>::value, T>::type c)
	{
		fnWriteChar(c);
		return 1;
	}

	template <typename T>
	static uint32_t writeImpl(const WriteCharFn& fnWriteChar, typename std::enable_if<std::is_same<T, unsigned char>::value, T>::type c)
	{
		fnWriteChar(c);
		return 1;
	}

	template <typename T>
	static uint32_t writeImpl(const WriteCharFn& fnWriteChar, typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, unsigned char>::value && !std::is_same<T, char>::value, T>::type number)
	{
		uint32_t size = 0;
		for (auto c : numberToString(number))
		{
			++size;
			fnWriteChar(c);
		}
		return size;
	}

	template <typename T>
	static uint32_t writeImpl(const WriteCharFn& fnWriteChar, typename std::enable_if<std::is_same<T, const char*>::value, T>::type pStr)
	{
		auto len = strlen(pStr);
		for (auto i = size_t{0}; i < len; ++i)
		{
			fnWriteChar(pStr[i]);
		}
		return len;
	}

	template <typename T>
	static uint32_t writeImpl(const WriteCharFn& fnWriteChar, typename std::enable_if<std::is_same<T, const unsigned char*>::value, T>::type pStr)
	{
		auto len = strlen(pStr);
		for (auto i = size_t{0}; i < len; ++i)
		{
			fnWriteChar(pStr[i]);
		}
		return len;
	}

	template <typename T>
	static uint32_t writeImpl(const WriteCharFn& fnWriteChar, const typename std::enable_if<is_constsize_string<T>::value, T>::type& str)
	{
		uint32_t size = 0;
		for (auto c : str)
		{
			++size;
			fnWriteChar(c);
		}

		return size;
	}

	template <typename T>
	static uint32_t writeImpl(const WriteCharFn& fnWriteChar, const typename std::enable_if<is_stdarray<T>::value, T>::type& arr)
	{
		for (auto c : arr)
		{
			fnWriteChar(c);
		}
		return arr.size();
	}
};
