#pragma once

#include <FixedSizeString.h>
#include <NumberConversion.h>

template <size_t MaxLineLength>
class DiscardingEchoConsole
{
public:
	void write(const String<MaxLineLength>& /*line*/)
	{
	}
};

template <size_t MaxLineLength>
class DiscardingLineSink
{
public:
	void lineCompleted(const String<MaxLineLength>& /*line*/)
	{
	}
};

template <size_t MaxLineLength>
class NoHistoryController
{
public:
	void newLine(const String<MaxLineLength>& /*line*/)
	{
	}

	String<MaxLineLength> up(const String<MaxLineLength>& currentLine)
	{
		return currentLine;
	}

	String<MaxLineLength> down(const String<MaxLineLength>& currentLine)
	{
		return currentLine;
	}
};

template <size_t MaxLineLength, typename LineSink = DiscardingLineSink<MaxLineLength>, typename EchoConsole = DiscardingEchoConsole<MaxLineLength>, typename HistoryController = NoHistoryController<MaxLineLength>>
class LineInputStrategy
{
private:
	enum class InputState
	{
		ReceiveNormalCharacter,
		ReceiveEscapeSequence,
		ReceiveControlSequence
	};
public:
	using Line = String<MaxLineLength>;

	explicit LineInputStrategy(LineSink lineSink = {}, EchoConsole echoConsole = {}, HistoryController historyController = {})
		: lineSink(lineSink)
		, echoConsole(echoConsole)
		, historyController(historyController)
	{
	}

	void rxChar(unsigned char c)
	{
		switch (state)
		{
		case InputState::ReceiveNormalCharacter:
			state = receiveNormalCharacter(c);
			break;
		case InputState::ReceiveEscapeSequence:
			state = receiveEscapeCharacter(c);
			break;
		case InputState::ReceiveControlSequence:
			state = receiveControlSequence(c);
			break;
		}
	}

private:
	InputState receiveNormalCharacter(char c)
	{
		if (isNewLine(c))
		{
			echoConsole.write("\r\n");
			if (currentlyReceivingLine.size() > 0)
			{
				lineSink.lineCompleted(currentlyReceivingLine);
				historyController.newLine(currentlyReceivingLine);
				currentlyReceivingLine.erase();
			}
			echoConsole.write("> ");
			return InputState::ReceiveNormalCharacter;
		}
		else if (isBackspace(c))
		{
			if (currentlyReceivingLine.size() > 0)
			{
				echoBackspace();
				currentlyReceivingLine.erase(currentlyReceivingLine.size() - 1, currentlyReceivingLine.size());
			}
			return InputState::ReceiveNormalCharacter;
		}
		else if (isprint(c))
		{
			auto appendResult = currentlyReceivingLine.append(c);
			if (appendResult == StringManipulationResult::Ok)
			{
				echoConsole.write(c);
			}
			return InputState::ReceiveNormalCharacter;
		}
		else if (c == 0x1b)
		{
			return InputState::ReceiveEscapeSequence;
		}
		else
		{
			ASSERT(false);
			return InputState::ReceiveNormalCharacter;
		}
	}

	InputState receiveEscapeCharacter(char c)
	{
		if (c == 0x5b)
		{ //control sequence introducer
			currentControlSequence.erase();
			return InputState::ReceiveControlSequence;
		}

		if (c >= 0x40 && c <= 0x5f)
		{ //final character
			//we dont do anything with these control characters (yet?)
			return InputState::ReceiveNormalCharacter;
		}

		ASSERT(false);
		return InputState::ReceiveNormalCharacter;
	}

	InputState receiveControlSequence(char c)
	{
		if (c >= 0x40 && c <= 0x7e)
		{ //final character
			handleControlSequence(c, currentControlSequence);
			return InputState::ReceiveNormalCharacter;
		}
		else
		{
			currentControlSequence.append(c);
			return InputState::ReceiveControlSequence;
		}
	}

	void handleControlSequence(char escapeCharacter, const String<10>& controlSequence)
	{
		auto getNumFromControlSequenceOr = [&](size_t def)
		{
			if (controlSequence.empty())
			{
				return def;
			}
			auto num = stringToNumber<size_t>(controlSequence);
			if (num)
			{
				return *num;
			}
			else
			{
				return def;
			}
		};

		if (escapeCharacter == 0x41)
		{
			for (auto i = getNumFromControlSequenceOr(1); i > 0; --i)
			{
				updateCurrentLine(historyController.up(currentlyReceivingLine));
			}
		}
		else if (escapeCharacter == 0x42)
		{
			for (auto i = getNumFromControlSequenceOr(1); i > 0; --i)
			{
				updateCurrentLine(historyController.down(currentlyReceivingLine));
			}
		}
		else if (escapeCharacter == 0x43)
		{
			//right
		}
		else if (escapeCharacter == 0x44)
		{
			//left
		}
	}

	void updateCurrentLine(Line line)
	{
		for (auto i = size_t{0}; i < currentlyReceivingLine.size(); ++i)
		{
			echoBackspace();
		}
		echoConsole.write(line);
		currentlyReceivingLine = line;
	}

	void echoBackspace()
	{
		echoConsole.write("\b \b");
	}

	static bool isBackspace(char c)
	{
		return c=='\b' || c=='\177';
	}

	static bool isNewLine(char c)
	{
		return c=='\r' || c=='\n';
	}

private:
	InputState state = InputState::ReceiveNormalCharacter;
	String<10> currentControlSequence;
	Line currentlyReceivingLine;
	LineSink lineSink;
	EchoConsole echoConsole;
	HistoryController historyController;
};
