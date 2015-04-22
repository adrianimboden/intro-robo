#pragma once

#include "IOStream.h"


template <typename TIOStream>
class LineEndingNormalizerIOStream final : public IOStream
{
public:
	LineEndingNormalizerIOStream(TIOStream stream = TIOStream{})
		: underlyingStream(std::move(stream))
	{
	}

	optional<char> readChar() final override
	{
		for (;;)
		{
			auto c = underlyingStream.readChar();
			if (!c)
			{
				return {};
			}

			if (*c == '\r' || *c == '\n')
			{
				if (firstNewLineChar == '\0')
				{
					firstNewLineChar = *c;
				}

				if (*c == firstNewLineChar)
				{
					return '\n';
				}
			}
			else
			{
				return c;
			}
		}
	}

	void writeChar(char c) final override
	{
		if (c == '\r')
		{
			return;
		}

		if (c == '\n')
		{
			underlyingStream.writeChar('\r');
			underlyingStream.writeChar('\n');
		}
		else
		{
			underlyingStream.writeChar(c);
		}
	}

private:
	char firstNewLineChar = '\0';
	TIOStream underlyingStream;
};
