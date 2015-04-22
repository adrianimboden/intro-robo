#pragma once

#ifndef __cplusplus
#error sorry, this header is c++ only
#endif

#include "StreamHelper.h"

class IOStream
{
public:
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

	virtual optional<char> readChar() = 0;
	virtual void writeChar(char c) = 0;
};

template <typename TIOStream, typename TVal>
typename std::enable_if<std::is_base_of<IOStream, TIOStream>::value || std::is_same<IOStream, TIOStream>::value, TIOStream&>::type operator<<(TIOStream& out, const TVal& val)
{
	out.write(val);
	return out;
}

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
