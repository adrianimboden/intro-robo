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

class Console
{
public:
	virtual void pollInput() = 0;

	template <typename... Args>
	uint32_t write(Args... args)
	{
		auto fnWriteChar = [this](char c){ writeChar(c); };
		return StreamHelper<decltype(fnWriteChar)>::write(fnWriteChar, args...);
	}

	template <typename T, size_t Size>
	uint32_t write(T (&data)[Size])
	{
		auto fnWriteChar = [this](char c){ writeChar(c); };
		return StreamHelper<decltype(fnWriteChar)>::write(fnWriteChar, data);
	}

	template <typename T, size_t Size>
	uint32_t writeRaw(T (&data)[Size])
	{
		auto fnWriteChar = [this](char c){ writeChar(c); };
		return StreamHelper<decltype(fnWriteChar)>::writeRaw(fnWriteChar, data);
	}

protected:
	virtual void writeChar(unsigned char c) = 0;
};

class IOStream
{
public:
	virtual optional<char> readChar() = 0;
	virtual void writeChar(char c) = 0;
};

template <typename FnWrite, typename FnRead>
class FnIOStream final : public IOStream
{
public:
	FnIOStream(FnWrite fnWrite, FnRead fnRead)
		: fnWrite(std::move(fnWrite))
		, fnRead(std::move(fnRead))
	{
	}

	optional<char> readChar() final override
	{
		return fnRead();
	}

	void writeChar(char c) final override
	{
		fnWrite(c);
	}

private:
	FnWrite fnWrite;
	FnRead fnRead;
};

template <typename FnWrite, typename FnRead>
FnIOStream<FnWrite, FnRead> makeFnIoStream(FnWrite fnWrite, FnRead fnRead)
{
	return FnIOStream<FnWrite, FnRead>{std::move(fnWrite), std::move(fnRead)};
}


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
