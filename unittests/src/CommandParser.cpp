#include <gmock/gmock.h>
#include "TestAssert.h"

#include <CommandParser.h>

using namespace testing;

struct ParserTestData
{
	struct {
		size_t callCount = 0;
		uint16_t p1 = 0;
	} cmd1;

	struct {
		size_t callCount = 0;
	} cmd2;

	struct {
		size_t callCount = 0;
		String<10> p1;
	} cmd3;

	struct {
		size_t callCount = 0;
		String<10> p1;
		uint16_t p2 = 0;
	} cmd4;

	struct {
		size_t callCount = 0;
		String<10> p1;
		String<3> p2;
		int8_t p3 = 0;
		int16_t p4 = 0;
		int32_t p5 = 0;
	} cmd5;

	void executeCommand(const detail::CmdString& command)
	{
		auto parser = makeParser(
			cmd("cmd1", [&](uint16_t p1)
			{
				++cmd1.callCount;
				cmd1.p1 = p1;
			}),
			cmd("cmd2", [&]()
			{
				++cmd2.callCount;
			}),
			cmd("cmd3", [&](const String<10>& p1)
			{
				++cmd3.callCount;
				cmd3.p1 = p1;
			}),
			cmd("cmd4", [&](const String<10>& p1, uint16_t p2)
			{
				++cmd4.callCount;
				cmd4.p1 = p1;
				cmd4.p2 = p2;
			}),
			cmd("cmd5", [&](const String<10>& p1, String<3> p2, int8_t p3, const int16_t p4, const int32_t& p5)
			{
				++cmd5.callCount;
				cmd5.p1 = p1;
				cmd5.p2 = p2;
				cmd5.p3 = p3;
				cmd5.p4 = p4;
				cmd5.p5 = p5;
			})
		);

		parser.executeCommand(command);
	}
};

TEST(CommandParser, one_value_param)
{
	ParserTestData testParser;

	testParser.executeCommand("cmd1 5");
	ASSERT_THAT(testParser.cmd1.callCount, Eq(1));
	ASSERT_THAT(testParser.cmd1.p1, Eq(5));
}

TEST(CommandParser, no_prameter)
{
	ParserTestData testParser;

	testParser.executeCommand("cmd2");
	ASSERT_THAT(testParser.cmd2.callCount, Eq(1));
}

TEST(CommandParser, one_string_param)
{
	ParserTestData testParser;

	testParser.executeCommand("cmd3 test");
	ASSERT_THAT(testParser.cmd3.callCount, Eq(1));
	ASSERT_THAT(testParser.cmd3.p1, Eq("test"));
}

TEST(CommandParser, string_and_number_param)
{
	ParserTestData testParser;

	testParser.executeCommand("cmd4 test2 42");
	ASSERT_THAT(testParser.cmd4.callCount, Eq(1));
	ASSERT_THAT(testParser.cmd4.p1, Eq("test2"));
	ASSERT_THAT(testParser.cmd4.p2, Eq(42));
}

TEST(CommandParser, combination)
{
	ParserTestData testParser;

	testParser.executeCommand("cmd5 test3 abcdef -15, 40, -500");
	ASSERT_THAT(testParser.cmd5.callCount, Eq(1));
	ASSERT_THAT(testParser.cmd5.p1, Eq("test3"));
	ASSERT_THAT(testParser.cmd5.p2, Eq("abc"));
	ASSERT_THAT(testParser.cmd5.p3, Eq(-15));
	ASSERT_THAT(testParser.cmd5.p4, Eq(40));
	ASSERT_THAT(testParser.cmd5.p5, Eq(-500));
}
