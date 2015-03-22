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

	//first line
	strategy.rxChar('a');
	strategy.rxChar('b');
	strategy.rxChar('c');
	ASSERT_THAT(line, Eq(""));
	strategy.rxChar('\n');
	ASSERT_THAT(line, Eq("abc"));

	//second line
	strategy.rxChar('d');
	strategy.rxChar('e');
	strategy.rxChar('f');
	ASSERT_THAT(line, Eq("abc"));
	strategy.rxChar('\n');
	ASSERT_THAT(line, Eq("def"));
}
