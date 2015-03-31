#pragma once

#include <FixedSizeString.h>
#include <NumberConversion.h>
#include <StreamHelper.h>

template <size_t MaxLineLength>
class DiscardingEchoConsole
{
public:
	template <typename TIOStream>
	void write(TIOStream& /*ioStream*/, const String<MaxLineLength>& /*line*/)
	{
	}
};

class SimpleEchoConsole
{
public:
	template <typename TIOStream, typename... Args>
	void write(TIOStream& stream, Args... args)
	{
		auto fnWriteChar = [&](char c) { stream.writeChar(c); };
		StreamHelper<decltype(fnWriteChar)>::write(fnWriteChar, args...);
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

	template <typename TIOStream>
	void rxChar(TIOStream& ioStream, unsigned char c)
	{
		switch (state)
		{
		case InputState::ReceiveNormalCharacter:
			state = receiveNormalCharacter(ioStream, c);
			break;
		case InputState::ReceiveEscapeSequence:
			state = receiveEscapeCharacter(ioStream, c);
			break;
		case InputState::ReceiveControlSequence:
			state = receiveControlSequence(ioStream, c);
			break;
		}
	}

private:
	template <typename TIOStream>
	InputState receiveNormalCharacter(TIOStream& ioStream, char c)
	{
		if (isNewLine(c))
		{
			echoConsole.write(ioStream, "\r\n");
			if (currentlyReceivingLine.size() > 0)
			{
				lineSink.lineCompleted(currentlyReceivingLine);
				historyController.newLine(currentlyReceivingLine);
				currentlyReceivingLine.erase();
			}
			echoConsole.write(ioStream, "> ");
			return InputState::ReceiveNormalCharacter;
		}
		else if (isBackspace(c))
		{
			if (currentlyReceivingLine.size() > 0)
			{
				echoBackspace(ioStream);
				currentlyReceivingLine.erase(currentlyReceivingLine.size() - 1, currentlyReceivingLine.size());
			}
			return InputState::ReceiveNormalCharacter;
		}
		else if (isprint(c))
		{
			auto appendResult = currentlyReceivingLine.append(c);
			if (appendResult == StringManipulationResult::Ok)
			{
				echoConsole.write(ioStream, c);
			}
			return InputState::ReceiveNormalCharacter;
		}
		else if (c == 0x1b)
		{
			return InputState::ReceiveEscapeSequence;
		}
		else
		{ //ignore unknown characters
			return InputState::ReceiveNormalCharacter;
		}
	}

	template <typename TIOStream>
	InputState receiveEscapeCharacter(TIOStream& /*ioStream*/, char c)
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

	template <typename TIOStream>
	InputState receiveControlSequence(TIOStream& ioStream, char c)
	{
		if (c >= 0x40 && c <= 0x7e)
		{ //final character
			handleControlSequence(ioStream, c, currentControlSequence);
			return InputState::ReceiveNormalCharacter;
		}
		else
		{
			currentControlSequence.append(c);
			return InputState::ReceiveControlSequence;
		}
	}

	template <typename TIOStream>
	void handleControlSequence(TIOStream& ioStream, char escapeCharacter, const String<10>& controlSequence)
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
				updateCurrentLine(ioStream, historyController.up(currentlyReceivingLine));
			}
		}
		else if (escapeCharacter == 0x42)
		{
			for (auto i = getNumFromControlSequenceOr(1); i > 0; --i)
			{
				updateCurrentLine(ioStream, historyController.down(currentlyReceivingLine));
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

	template <typename TIOStream>
	void updateCurrentLine(TIOStream& ioStream, Line line)
	{
		for (auto i = size_t{0}; i < currentlyReceivingLine.size(); ++i)
		{
			echoBackspace(ioStream);
		}
		echoConsole.write(ioStream, line);
		currentlyReceivingLine = line;
	}

	template <typename TIOStream>
	void echoBackspace(TIOStream& ioStream)
	{
		echoConsole.write(ioStream, "\b \b");
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

template <size_t MaxLineLength, typename LineSink = DiscardingLineSink<MaxLineLength>, typename EchoConsole = DiscardingEchoConsole<MaxLineLength>, typename HistoryController = NoHistoryController<MaxLineLength>>
LineInputStrategy<MaxLineLength, LineSink, EchoConsole, HistoryController> makeLineInputStrategy(LineSink lineSink = {}, EchoConsole echoConsole = {}, HistoryController historyController = {})
{
	return LineInputStrategy<MaxLineLength, LineSink, EchoConsole, HistoryController>{lineSink, echoConsole, historyController};
}
