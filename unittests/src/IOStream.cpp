#include <gmock/gmock.h>
#include "TestAssert.h"

#include <IOStream.h>

using namespace testing;

template <typename... Args>
void checkOutputOfWrite(const std::string& str, Args... args)
{
	std::stringstream strm;
	auto writeTxChar = [&](char c)
	{
		strm << c;
	};

	auto ioStream = makeFnIoStream(writeTxChar, []()->optional<char>{ return {}; });

	ioStream.write(args...);

	ASSERT_THAT(strm.str(), StrEq(str));
}

TEST(IOStream, when_i_write_a_number_then_it_will_be_written_as_text)
{
	checkOutputOfWrite("5", static_cast<uint16_t>(5));
	checkOutputOfWrite("5", static_cast<uint32_t>(5));
}

TEST(IOStream, when_i_write_a_text_then_it_will_be_written_as_text)
{
	checkOutputOfWrite("Test", "Test");
	checkOutputOfWrite("Test", String<4>{"Test"});
}

TEST(IOStream, when_i_write_raw_data_then_it_will_be_written_as_data)
{
	std::array<uint8_t, 3> data{'a', 'b', 'c'};
	checkOutputOfWrite("abc", data.data(), 3); //plain array (as pointer)
	checkOutputOfWrite("abc", data); //std::array
}

TEST(IOStream, when_i_write_an_array_then_it_will_be_written_as_data)
{
	uint8_t data[] = {'a', 'b', 'c'};

	std::stringstream strm;
	auto writeTxChar = [&](char c)
	{
		strm << c;
	};

	auto ioStream = makeFnIoStream(writeTxChar, []()->optional<char>{ return {}; });

	ioStream.write(data);

	ASSERT_THAT(strm.str(), StrEq("abc"));
}
