#pragma once

#include <String.h>

class DiscardingEchoConsole
{
public:
	void write(...)
	{
	}
};

class DiscardingLineSink
{
public:
	void lineCompleted(...)
	{
	}
};

template <size_t MaxLineLength, typename LineSink = DiscardingLineSink, typename EchoConsole = DiscardingEchoConsole>
class LineInputStrategy
{
public:
	using Line = String<MaxLineLength>;

	explicit LineInputStrategy(LineSink lineSink = {}, EchoConsole echoConsole = {})
		: lineSink(lineSink)
		, echoConsole(echoConsole)
	{
	}

	void rxChar(unsigned char c)
	{
		if (isNewLine(c))
		{
			if (currentlyReceivingLine.size() > 0)
			{
				lineSink.lineCompleted(currentlyReceivingLine);
				currentlyReceivingLine.erase();
			}
			echoConsole.write("\r\n> ");
		}
		else if (isBackspace(c))
		{
			if (currentlyReceivingLine.size() > 0)
			{
				echoConsole.write("\b \b");//backwards, space, backwards
				currentlyReceivingLine.erase(currentlyReceivingLine.size() - 1, currentlyReceivingLine.size());
			}
		}
		else if (isprint(c))
		{
			auto appendResult = currentlyReceivingLine.append(c);
			if (appendResult == StringManipulationResult::Ok)
			{
				echoConsole.write(c);
			}
		}
	}

private:
	static bool isBackspace(char c)
	{
		return c=='\b' || c=='\177';
	}

	static bool isNewLine(char c)
	{
		return c=='\r' || c=='\n';
	}

private:
	Line currentlyReceivingLine;
	LineSink lineSink;
	EchoConsole echoConsole;
};
