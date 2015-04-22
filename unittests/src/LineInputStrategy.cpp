#include <gmock/gmock.h>
#include "TestAssert.h"

#include <LineInputStrategy.h>
#include "HistoryController.h"
#include <IOStream.h>

using namespace testing;

TEST(LineInputStrategy, lines_will_be_reported_to_line_sink)
{
	struct TestLineSink
	{
	public:
		TestLineSink(String<10>* pLine)
			: pLine(pLine)
		{
		}

		void lineCompleted(IOStream&, const String<10>& line)
		{
			*pLine = line;
		}

	private:
		String<10>* pLine;
	};

	String<10> line;
	LineInputStrategy<10, TestLineSink> strategy{TestLineSink{&line}};

	class DummyIo : public IOStream
	{
	public:
		optional<char> readChar() override
		{
			return {};
		}

		void writeChar(char /*c*/) override
		{
		}
	};

	DummyIo dummy;

	//first line
	strategy.rxChar(dummy, 'a');
	strategy.rxChar(dummy, 'b');
	strategy.rxChar(dummy, 'c');
	ASSERT_THAT(line, Eq(""));
	strategy.rxChar(dummy, '\n');
	ASSERT_THAT(line, Eq("abc"));

	//second line
	strategy.rxChar(dummy, 'd');
	strategy.rxChar(dummy, 'e');
	strategy.rxChar(dummy, 'f');
	ASSERT_THAT(line, Eq("abc"));
	strategy.rxChar(dummy, '\n');
	ASSERT_THAT(line, Eq("def"));
}

using NewLineFn = std::function<void(const String<10>& line)>;
using UpFn = std::function<String<10>(const String<10>& line)>;
using DownFn = std::function<String<10>(const String<10>& line)>;

class IOStreamMock : public IOStream
{
public:
	MOCK_METHOD0(readChar, optional<char>());
	MOCK_METHOD1(writeChar, void(char c));
};

class LineSinkMock
{
public:
	MOCK_METHOD2(lineCompleted, void(IOStreamMock&, const String<10>& line));
};


class LineSinkForwarder
{
public:
	explicit LineSinkForwarder(LineSinkMock* pMock)
		: pMock(pMock)
	{
	}

	template <typename TIOStream>
	void lineCompleted(TIOStream& ioStream, const String<10>& line)
	{
		pMock->lineCompleted(ioStream, line);
	}

private:
	LineSinkMock* pMock;
};


class HistoryControllerMock
{
public:
	MOCK_METHOD1(newLine, void(const String<10>& line));
	MOCK_METHOD1(up, String<10>(const String<10>& line));
	MOCK_METHOD1(down, String<10>(const String<10>& line));
};

class TestHistoryController
{
public:
	TestHistoryController(const NewLineFn& fnNewLine, const UpFn& fnUp, const DownFn& fnDown)
		: fnNewLine(fnNewLine)
		, fnUp(fnUp)
		, fnDown(fnDown)
	{
	}

	void newLine(const String<10>& line)
	{
		fnNewLine(line);
	}

	String<10> up(const String<10>& line)
	{
		return fnUp(line);
	}

	String<10> down(const String<10>& line)
	{
		return fnDown(line);
	}

private:
	NewLineFn fnNewLine;
	UpFn fnUp;
	DownFn fnDown;
};

void checkHistoryControllerUsage(const std::vector<char>& input, const NewLineFn& fnNewLine, const UpFn& fnUp, const DownFn& fnDown)
{
	LineInputStrategy<10, DiscardingLineSink<10>, DiscardingEchoConsole<10>, TestHistoryController> lineInputStrategy{
		DiscardingLineSink<10>{},
		DiscardingEchoConsole<10>{},
		TestHistoryController{
			fnNewLine,
			fnUp,
			fnDown
		}
	};

	for (auto c : input)
	{
		int dummy;
		lineInputStrategy.rxChar(dummy, c);
	}
}

void checkHistoryControllerUsageUsingMock(HistoryControllerMock& mock, const std::vector<char>& input)
{
	LineInputStrategy<10, DiscardingLineSink<10>, DiscardingEchoConsole<10>, TestHistoryController> lineInputStrategy{
		DiscardingLineSink<10>{},
		DiscardingEchoConsole<10>{},
		TestHistoryController{
			[&](const String<10>& line){ mock.newLine(line); },
			[&](const String<10>& line){ return mock.up(line); },
			[&](const String<10>& line){ return mock.down(line); }
		}
	};

	for (auto c : input)
	{
		int dummy;
		lineInputStrategy.rxChar(dummy, c);
	}
}

TEST(LineInputStrategy, when_pressing_up_then_it_will_be_reported_to_history_controller)
{
	size_t counter = 0;

	checkHistoryControllerUsage(
		{0x1b, 0x5b, '5', 0x41},
		[](const String<10>&){},
		[&](const String<10>&){ counter += 1; return ""; },
		[](const String<10>&){ return ""; }
	);

	ASSERT_THAT(counter, Eq(5));
}

TEST(LineInputStrategy, when_pressing_down_then_it_will_be_reported_to_history_controller)
{
	size_t counter = 0;

	checkHistoryControllerUsage(
		{0x1b, 0x5b, '5', 0x42},
		[](const String<10>&){},
		[](const String<10>&){ return ""; },
		[&](const String<10>&){ counter += 1; return ""; }
	);

	ASSERT_THAT(counter, Eq(5));
}

TEST(LineInputStrategy, when_no_amount_is_provided_it_defaults_to_one)
{
	size_t counter = 0;

	checkHistoryControllerUsage(
		{0x1b, 0x5b, 0x42},
		[](const String<10>&){},
		[](const String<10>&){ return ""; },
		[&](const String<10>&){ counter += 1; return ""; }
	);

	ASSERT_THAT(counter, Eq(1));
}

TEST(LineInputStrategy, when_entering_a_new_line_then_newLine_will_be_called)
{
	HistoryControllerMock mock;

	EXPECT_CALL(mock, newLine(_));

	checkHistoryControllerUsageUsingMock(mock, {'a', '\n'});
}

TEST(LineInputStrategy, when_pressing_up_then_the_historyController_gets_notified)
{
	HistoryControllerMock mock;

	InSequence seq;
	EXPECT_CALL(mock, newLine(String<10>("a")));
	EXPECT_CALL(mock, newLine(String<10>("b")));
	EXPECT_CALL(mock, newLine(String<10>("c")));
	EXPECT_CALL(mock, up(String<10>(""))).WillOnce(Return("c"));
	EXPECT_CALL(mock, newLine(String<10>("c")));

	checkHistoryControllerUsageUsingMock(mock, {
		'a', '\n',
		'b', '\n',
		'c', '\n',
		0x1b, 0x5b, 0x41,
		'\n',
	});
}

TEST(LineInputStrategy, HistoryController)
{
	IOStreamMock ioStream;
	LineSinkMock lineSink;

	LineInputStrategy<10, LineSinkForwarder, DiscardingEchoConsole<10>, HistoryController<String<10>, 5>> lineInputStrategy{
		LineSinkForwarder{&lineSink},
		DiscardingEchoConsole<10>{},
		HistoryController<String<10>, 5>{}
	};

	InSequence seq;

	EXPECT_CALL(lineSink, lineCompleted(_, String<10>{"a"}));
	for (auto&& c : std::vector<char>{'a', '\n'})
	{
		lineInputStrategy.rxChar(ioStream, c);
	}

	EXPECT_CALL(lineSink, lineCompleted(_, String<10>{"b"}));
	for (auto&& c : std::vector<char>{'b', '\n'})
	{
		lineInputStrategy.rxChar(ioStream, c);
	}

	for (auto i = 0; i < 3; ++i)
	{
		EXPECT_CALL(lineSink, lineCompleted(_, String<10>{"b"}));
		for (auto&& c : std::vector<char>{0x1b, 0x5b, 0x41, '\n'})
		{
			lineInputStrategy.rxChar(ioStream, c);
		}
	}

	EXPECT_CALL(lineSink, lineCompleted(_, String<10>{"b"}));
	for (auto&& c : std::vector<char>{0x1b, 0x5b, 0x41, '\n'})
	{
		lineInputStrategy.rxChar(ioStream, c);
	}
}
