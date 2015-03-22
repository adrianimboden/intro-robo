#include <gmock/gmock.h>
#include "TestAssert.h"

#include <String.h>
#include <NumberConversion.h>

#include <cstdlib>
#include <inttypes.h>
#include <array>
#include <type_traits>

//byte AS1_RecvChar(AS1_TComData *Chr) { }
//byte AS1_SendChar(AS1_TComData Chr) { }
//byte AS1_RecvBlock(AS1_TComData *Ptr,word Size,word *Rcv) { }
//byte AS1_SendBlock(AS1_TComData *Ptr,word Size,word *Snd) { }
//byte AS1_ClearRxBuf(void) { }
//byte AS1_ClearTxBuf(void) { }
//word AS1_GetCharsInRxBuf(void) { }
//word AS1_GetCharsInTxBuf(void) { }
//void AS1_Init(void) { }

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

template <typename WriteTxCharFn, typename ConsoleInputStrategy = DiscardInputStrategy>
class Console
{
public:
	explicit Console(WriteTxCharFn writeTxCharFn, ConsoleInputStrategy inputStrategy = {})
		: writeTxCharFn(std::move(writeTxCharFn))
		, inputStrategy(std::move(inputStrategy))
	{
	}

	//! to be called when a character has been received (eg. from interrupt)
	void rxChar(ConsoleChar c)
	{
		inputStrategy.rxChar(c);
	}

	template <typename T>
	void write(const T& val)
	{
		writeImpl<T>(val);
	}

	void write(uint8_t* pData, size_t size)
	{
		for (auto i = size_t{0}; i < size; ++i)
		{
			writeTxCharFn(pData[i]);
		}
	}

private:
	template <typename T>
	void writeImpl(typename std::enable_if<std::is_integral<T>::value, T>::type number)
	{
		for (auto c : numberToString(number))
		{
			writeTxCharFn(c);
		}
	}

	template <typename T>
	void writeImpl(typename std::enable_if<std::is_same<T, const char*>::value, T>::type pStr)
	{
		auto len = strlen(pStr);
		for (auto i = size_t{0}; i < len; ++i)
		{
			writeTxCharFn(pStr[i]);
		}
	}

	template <typename T>
	void writeImpl(const typename std::enable_if<is_constsize_string<T>::value, T>::type& str)
	{
		for (auto c : str)
		{
			writeTxCharFn(c);
		}
	}

	template <typename T>
	void writeImpl(const typename std::enable_if<is_stdarray<T>::value, T>::type& arr)
	{
		for (auto c : arr)
		{
			writeTxCharFn(c);
		}
	}

private:
	WriteTxCharFn writeTxCharFn;
	ConsoleInputStrategy inputStrategy;
};

using namespace testing;

template <typename... Args>
void checkOutputOfWrite(const std::string& str, Args... args)
{
	std::stringstream strm;
	auto writeTxChar = [&](char c)
	{
		strm << c;
	};

	Console<decltype(writeTxChar)> console{writeTxChar};

	console.write(args...);

	ASSERT_THAT(strm.str(), StrEq(str));
}

TEST(Console, when_i_write_a_number_then_it_will_be_written_as_text)
{
	checkOutputOfWrite("5", static_cast<uint8_t>(5));
	checkOutputOfWrite("5", static_cast<uint16_t>(5));
	checkOutputOfWrite("5", static_cast<uint32_t>(5));
}

TEST(Console, when_i_write_a_text_then_it_will_be_written_as_text)
{
	checkOutputOfWrite("Test", "Test");
	checkOutputOfWrite("Test", String<4>{"Test"});
}

TEST(Console, when_i_write_raw_data_then_it_will_be_written_as_data)
{
	std::array<uint8_t, 3> data{'a', 'b', 'c'};
	checkOutputOfWrite("abc", data.data(), 3); //plain array
	checkOutputOfWrite("abc", data); //std::array
}

TEST(Console, when_i_receive_data_it_will_be_forwarded_to_input_strategy)
{
	class StoreInStringstreamInputStrategy
	{
	public:
		StoreInStringstreamInputStrategy(std::stringstream* pStrm)
			: pStrm(pStrm)
		{
		}

		void rxChar(char c)
		{
			*pStrm << c;
		}

	private:
		std::stringstream* pStrm;
	};

	std::stringstream inStrm;

	Console<void*, StoreInStringstreamInputStrategy> console{nullptr, StoreInStringstreamInputStrategy{&inStrm}};

	console.rxChar('a');
	ASSERT_THAT(inStrm.str(), StrEq("a"));
}
