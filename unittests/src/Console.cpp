#include <gmock/gmock.h>
#include "TestAssert.h"

#include <Console.h>

#include <cstdlib>
#include <inttypes.h>
#include <array>
#include <type_traits>




#include <CircularBuffer.h>
//byte AS1_RecvChar(AS1_TComData *Chr) { }
//byte AS1_SendChar(AS1_TComData Chr) { }
//byte AS1_RecvBlock(AS1_TComData *Ptr,word Size,word *Rcv) { }
//byte AS1_SendBlock(AS1_TComData *Ptr,word Size,word *Snd) { }
//byte AS1_ClearRxBuf(void) { }
//byte AS1_ClearTxBuf(void) { }
//word AS1_GetCharsInRxBuf(void) { }
//word AS1_GetCharsInTxBuf(void) { }
//void AS1_Init(void) { }

using namespace testing;

template <typename... Args>
void checkOutputOfWrite(const std::string& str, Args... args)
{
	std::stringstream strm;
	auto writeTxChar = [&](char c)
	{
		strm << c;
	};

	ConcreteConsole<decltype(writeTxChar)> console{writeTxChar};

	console.write(args...);

	ASSERT_THAT(strm.str(), StrEq(str));
}

TEST(Console, when_i_write_a_number_then_it_will_be_written_as_text)
{
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
	checkOutputOfWrite("abc", data.data(), 3); //plain array (as pointer)
	checkOutputOfWrite("abc", data); //std::array
}

TEST(Console, when_i_write_an_array_then_it_will_be_written_as_data)
{
	uint8_t data[] = {'a', 'b', 'c'};

	std::stringstream strm;
	auto writeTxChar = [&](char c)
	{
		strm << c;
	};

	ConcreteConsole<decltype(writeTxChar)> console{writeTxChar};

	console.write(data);

	ASSERT_THAT(strm.str(), StrEq("abc"));
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

	ConcreteConsole<void(*)(char c), StoreInStringstreamInputStrategy> console{[](char){}, StoreInStringstreamInputStrategy{&inStrm}};

	console.rxChar('a');
	ASSERT_THAT(inStrm.str(), StrEq("a"));
}
