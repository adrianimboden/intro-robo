#pragma once

#ifndef __cplusplus
#error sorry, this header is c++ only
#endif

#include "Console.h"

class CdcPolymorphicIOStream final : public IOStream
{
public:
	using ReceiveFn = uint8_t(*)(uint8_t *Chr);
	using SendFn = uint8_t(*)(uint8_t Chr);

	CdcPolymorphicIOStream(ReceiveFn fnReceive, SendFn fnSend)
		: fnReceive(fnReceive)
		, fnSend(fnSend)
	{
	}

	optional<char> readChar() final override
	{
		byte inputChar;
		if (fnReceive(&inputChar) == ERR_OK)
		{
			return inputChar;
		}
		else
		{
			return {};
		}
	}

	void writeChar(char c) final override
	{
		while (fnSend(c) == ERR_TXFULL);
	}

private:
	ReceiveFn fnReceive;
	SendFn fnSend;
};

template <uint8_t(*ReceiveFn)(uint8_t *Chr), uint8_t(*SendFn)(uint8_t Chr)>
class CdcStaticIOStream final : public IOStream
{
public:
	optional<char> readChar() final override
	{
		byte inputChar;
		if (ReceiveFn(&inputChar) == ERR_OK)
		{
			return inputChar;
		}
		else
		{
			return {};
		}
	}

	void writeChar(char c) final override
	{
		while (SendFn(c) == ERR_TXFULL);
	}
};
