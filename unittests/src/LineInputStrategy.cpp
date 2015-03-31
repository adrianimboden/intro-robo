#include <gmock/gmock.h>
#include "TestAssert.h"

#include <LineInputStrategy.h>

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

		void lineCompleted(const String<10>& line)
		{
			*pLine = line;
		}

	private:
		String<10>* pLine;
	};

	String<10> line;
	LineInputStrategy<10, TestLineSink> strategy{TestLineSink{&line}};

	int dummy;

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
