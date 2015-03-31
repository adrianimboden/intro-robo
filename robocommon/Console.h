#pragma once

#ifndef __cplusplus
#error sorry, this header is c++ only
#endif

#include <FixedSizeString.h>
#include <NumberConversion.h>
#include <memory>
#include <cstring>

#include "StreamHelper.h"

using ConsoleChar = unsigned char;

class DiscardInputStrategy
{
public:
	template <typename TIOStream>
	void rxChar(TIOStream&, char /*c*/)
	{
	}
};

class IOStream;

class Console
{
public:
	virtual void pollInput() = 0;

	virtual IOStream* getUnderlyingIoStream() = 0;

protected:
	virtual void writeChar(unsigned char c) = 0;
};

template <typename TIOStream, typename ConsoleInputStrategy = DiscardInputStrategy>
class ConcreteConsole : public Console
{
public:
	using type = ConcreteConsole<TIOStream, ConsoleInputStrategy>;

	explicit ConcreteConsole(TIOStream ioStream, ConsoleInputStrategy inputStrategy = {})
		: ioStream(std::move(ioStream))
		, inputStrategy(std::move(inputStrategy))
	{
	}

	void pollInput()
	{
		auto optC = ioStream.readChar();
		if (optC)
		{
			inputStrategy.rxChar(ioStream, *optC);
		}
	}

	IOStream* getUnderlyingIoStream() override
	{
		return &ioStream;
	}

protected:
	void writeChar(unsigned char c) override
	{
		ioStream.writeChar(c);
	}

private:
	TIOStream ioStream;
	ConsoleInputStrategy inputStrategy;
};

template <typename TIOStream, typename ConsoleInputStrategy = DiscardInputStrategy>
ConcreteConsole<TIOStream, ConsoleInputStrategy> makeConsole(TIOStream ioStream, ConsoleInputStrategy inputStrategy = {})
{
	return ConcreteConsole<TIOStream, ConsoleInputStrategy>{ioStream, inputStrategy};
}
