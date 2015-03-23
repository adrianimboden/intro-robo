#include <gmock/gmock.h>
#include "TestAssert.h"
#include "StringStreamer.h"
#include <memory>

#include <CommandParser.h>

using namespace testing;

template <typename T>
std::unique_ptr<T> intoUniquePtr(T data)
{
	return std::unique_ptr<T>(new T(data));
}

struct ParserTestData
{
	String<80> lastError;
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

	std::unique_ptr<CommandParser> ptrParser;

	ParserTestData()
	{
		ptrParser = intoUniquePtr(makeParser(
			[&](const String<80>& error)
			{
				lastError = error;
			},
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
		));
	}

	CommandParser& getCommandParser()
	{
		return *ptrParser;
	}
};

TEST(CommandParser, one_value_param)
{
	ParserTestData testParser;

	testParser.getCommandParser().executeCommand("cmd1 5");
	ASSERT_THAT(testParser.cmd1.callCount, Eq(1));
	ASSERT_THAT(testParser.cmd1.p1, Eq(5));
}

TEST(CommandParser, no_prameter)
{
	ParserTestData testParser;

	testParser.getCommandParser().executeCommand("cmd2");
	ASSERT_THAT(testParser.cmd2.callCount, Eq(1));
}

TEST(CommandParser, one_string_param)
{
	ParserTestData testParser;

	testParser.getCommandParser().executeCommand("cmd3 test");
	ASSERT_THAT(testParser.cmd3.callCount, Eq(1));
	ASSERT_THAT(testParser.cmd3.p1, Eq("test"));
}

TEST(CommandParser, string_and_number_param)
{
	ParserTestData testParser;

	testParser.getCommandParser().executeCommand("cmd4 test2 42");
	ASSERT_THAT(testParser.cmd4.callCount, Eq(1));
	ASSERT_THAT(testParser.cmd4.p1, Eq("test2"));
	ASSERT_THAT(testParser.cmd4.p2, Eq(42));
}

TEST(CommandParser, combination)
{
	ParserTestData testParser;

	testParser.getCommandParser().executeCommand("cmd5 test3 abcdef -15, 40, -500");
	ASSERT_THAT(testParser.cmd5.callCount, Eq(1));
	ASSERT_THAT(testParser.cmd5.p1, Eq("test3"));
	ASSERT_THAT(testParser.cmd5.p2, Eq("abc"));
	ASSERT_THAT(testParser.cmd5.p3, Eq(-15));
	ASSERT_THAT(testParser.cmd5.p4, Eq(40));
	ASSERT_THAT(testParser.cmd5.p5, Eq(-500));
}

TEST(CommandParser, error_cmd_not_found)
{
	ParserTestData testParser;

	testParser.getCommandParser().executeCommand("unknown");
	ASSERT_THAT(testParser.lastError, Eq("unknown not found"));
}

TEST(CommandParser, error_paramters_dont_match)
{
	ParserTestData testParser;

	auto checkError = [&](const char* cmd, const char* expectedError)
	{
		testParser.lastError.erase();
		testParser.getCommandParser().executeCommand(cmd);
		ASSERT_THAT(testParser.lastError, Eq(expectedError)) << testParser.lastError;
	};

	checkError("cmd1", "error. syntax: cmd1 num");
	checkError("cmd3", "error. syntax: cmd3 str");
	checkError("cmd4", "error. syntax: cmd4 str num");

	checkError("cmd1 text", "error. syntax: cmd1 num");
	checkError("cmd4 45 text", "error. syntax: cmd4 str num");
}

TEST(CommandParser, get_available_commands)
{
	ParserTestData testParser;
	std::vector<String<10>> commands;
	commands.resize(10);
	testParser.getCommandParser().getAvailableCommands(commands.data(), 10);

	ASSERT_THAT(commands[0], Eq("cmd1"));
	ASSERT_THAT(commands[1], Eq("cmd2"));
	ASSERT_THAT(commands[2], Eq("cmd3"));
	ASSERT_THAT(commands[3], Eq("cmd4"));
	ASSERT_THAT(commands[4], Eq("cmd5"));
}
