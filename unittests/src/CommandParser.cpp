#include <gmock/gmock.h>
#include "TestAssert.h"
#include "StringStreamer.h"
#include <memory>

#include <CommandParser.h>
#include <AutoArgsCommand.h>

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
			[&](IOStream& /*ioStream*/, const String<80>& error)
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
			cmd("cmd5", [&](IOStream& ioStream, const String<10>& p1, String<3> p2, int8_t p3, const int16_t p4, const int32_t& p5)
			{
				++cmd5.callCount;
				cmd5.p1 = p1;
				cmd5.p2 = p2;
				cmd5.p3 = p3;
				cmd5.p4 = p4;
				cmd5.p5 = p5;
				ioStream.writeChar(p1[0]);
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

	auto ioStream = makeFnIoStream([](char){}, []()->optional<char>{ return {}; });
	testParser.getCommandParser().executeCommand(ioStream, "cmd1 5");
	ASSERT_THAT(testParser.cmd1.callCount, Eq(1));
	ASSERT_THAT(testParser.cmd1.p1, Eq(5));
}

TEST(CommandParser, no_prameter)
{
	ParserTestData testParser;

	auto ioStream = makeFnIoStream([](char){}, []()->optional<char>{ return {}; });
	testParser.getCommandParser().executeCommand(ioStream, "cmd2");
	ASSERT_THAT(testParser.cmd2.callCount, Eq(1));
}

TEST(CommandParser, one_string_param)
{
	ParserTestData testParser;

	auto ioStream = makeFnIoStream([](char){}, []()->optional<char>{ return {}; });
	testParser.getCommandParser().executeCommand(ioStream, "cmd3 test");
	ASSERT_THAT(testParser.cmd3.callCount, Eq(1));
	ASSERT_THAT(testParser.cmd3.p1, Eq("test"));
}

TEST(CommandParser, string_and_number_param)
{
	ParserTestData testParser;

	auto ioStream = makeFnIoStream([](char){}, []()->optional<char>{ return {}; });
	testParser.getCommandParser().executeCommand(ioStream, "cmd4 test2 42");
	ASSERT_THAT(testParser.cmd4.callCount, Eq(1));
	ASSERT_THAT(testParser.cmd4.p1, Eq("test2"));
	ASSERT_THAT(testParser.cmd4.p2, Eq(42));
}

TEST(CommandParser, combination)
{
	ParserTestData testParser;

	auto ioStream = makeFnIoStream([](char){}, []()->optional<char>{ return {}; });
	testParser.getCommandParser().executeCommand(ioStream, "cmd5 test3 abc -15 40 -500");
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

	auto ioStream = makeFnIoStream([](char){}, []()->optional<char>{ return {}; });
	testParser.getCommandParser().executeCommand(ioStream, "unknown");
	ASSERT_THAT(testParser.lastError, Eq("unknown not found"));
}

TEST(CommandParser, error_parameters_dont_match)
{
	ParserTestData testParser;

	auto checkError = [&](const char* cmd, const char* expectedError)
	{
		testParser.lastError.erase();
		auto ioStream = makeFnIoStream([](char){}, []()->optional<char>{ return {}; });
		testParser.getCommandParser().executeCommand(ioStream, cmd);
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

TEST(CommandParser, when_multiple_spaces_come_after_another_then_they_will_be_ignored)
{
	ParserTestData testParser;
	auto ioStream = makeFnIoStream([](char){}, []()->optional<char>{ return {}; });
	testParser.getCommandParser().executeCommand(ioStream, "cmd4    a    1");
	ASSERT_THAT(testParser.cmd4.p1, Eq("a"));
	ASSERT_THAT(testParser.cmd4.p2, Eq(1));
}

TEST(CommandParser, when_a_parameter_is_between_single_quotes_then_the_parameter_can_contain_spaces)
{
	ParserTestData testParser;
	auto ioStream = makeFnIoStream([](char){}, []()->optional<char>{ return {}; });
	testParser.getCommandParser().executeCommand(ioStream, "cmd4 'a b' 1");
	ASSERT_THAT(testParser.cmd4.p1, Eq("a b"));
}

TEST(CommandParser, when_a_parameter_is_between_double_quotes_then_the_parameter_can_contain_spaces)
{
	ParserTestData testParser;
	auto ioStream = makeFnIoStream([](char){}, []()->optional<char>{ return {}; });
	testParser.getCommandParser().executeCommand(ioStream, "cmd4 \"a b\" 1");
	ASSERT_THAT(testParser.cmd4.p1, Eq("a b"));
}

TEST(CommandParser, when_a_parameter_is_between_double_quotes_then_single_quotes_can_be_in_the_string)
{
	ParserTestData testParser;
	auto ioStream = makeFnIoStream([](char){}, []()->optional<char>{ return {}; });
	testParser.getCommandParser().executeCommand(ioStream, "cmd4 \"a ' b\" 1");
	ASSERT_THAT(testParser.cmd4.p1, Eq("a ' b"));
}

TEST(CommandParser, when_a_parameter_is_between_single_quotes_then_double_quotes_can_be_in_the_string)
{
	ParserTestData testParser;
	auto ioStream = makeFnIoStream([](char){}, []()->optional<char>{ return {}; });
	testParser.getCommandParser().executeCommand(ioStream, "cmd4 \'a \" b\' 1");
	ASSERT_THAT(testParser.cmd4.p1, Eq("a \" b"));
}

TEST(CommandParser, when_malicious_input_is_being_received_then_the_program_outputs_a_correct_error)
{
	ParserTestData testParser;

	auto checkError = [&](const char* cmd)
	{
		testParser.lastError.erase();
		auto ioStream = makeFnIoStream([](char){}, []()->optional<char>{ return {}; });
		testParser.getCommandParser().executeCommand(ioStream, cmd);
		ASSERT_THAT(testParser.lastError.empty(), Eq(false));
	};

	checkError("cmd4 \"a b");
	checkError("cmd3 a b");
	checkError("cmd4 \"");
	checkError("cmd4 \"\"\"");
}

TEST(CommandParser, when_a_iostream_is_excepted_then_it_will_be_passed_through)
{
	ParserTestData testParser;
	char writtenChar = '\0';
	auto ioStream = makeFnIoStream([&](char c){ writtenChar = c; }, []()->optional<char>{ return {}; });
	testParser.getCommandParser().executeCommand(ioStream, "cmd5 a b 1 2 3");
	ASSERT_THAT(writtenChar, Eq('a'));
}
