#pragma once

#ifndef __cplusplus
#error sorry, this header is c++ only
#endif

#include <array>

#include "CircularBuffer.h"

template <typename T, size_t ScrollbackLength>
class HistoryController
{
public:
	void newLine(const T& line)
	{
		resetDownBuffer();
		upBuffer.push_back(line);
	}

	T up(const T& currentLine)
	{
		auto line = upBuffer.pop_back();
		if (!line)
		{
			return currentLine;
		}

		if (currHistLine)
			downBuffer.push_back(*currHistLine);

		if (!nonHistLine)
			nonHistLine = currentLine;

		currHistLine = *line;

		return *line;
	}

	T down(const T& currentLine)
	{
		auto line = downBuffer.pop_back();
		if (!line)
		{
			if (nonHistLine)
			{
				auto toReturn = *nonHistLine;
				resetDownBuffer();
				return toReturn;
			}
			else
			{
				return currentLine;
			}
		}

		if (currHistLine)
			upBuffer.push_back(*currHistLine);

		currHistLine = *line;

		return *line;
	}

private:
	void resetDownBuffer()
	{
		if (currHistLine)
			upBuffer.push_back(*currHistLine);

		nonHistLine.reset();
		currHistLine.reset();

		while (true)
		{
			auto line = downBuffer.pop_back();
			if (!line)
				break;

			upBuffer.push_back(*line);
		}
	}

private:
	optional<T> nonHistLine;
	CircularBuffer<T, ScrollbackLength, CircularBufferFullStrategy::OverwriteOldest> upBuffer;
	optional<T> currHistLine;
	CircularBuffer<T, ScrollbackLength, CircularBufferFullStrategy::OverwriteOldest> downBuffer;
};
