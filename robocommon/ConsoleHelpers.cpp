#include "ConsoleHelpers.h"

#include <IOStream.h>
#include <FreeRTOS.h>

#include <array>


constexpr int16_t tabSize = 2;
void fillUp(IOStream& ioStream, int16_t columnSize, int16_t alreadyUsed)
{
	for (auto j = 0; j < (columnSize - alreadyUsed) + tabSize; ++j)
	{
		ioStream.write(" ");
	}
};

template <typename T>
void writeColumn(IOStream& ioStream, const T& content, int16_t columnSize)
{
	auto writtenChars = ioStream.write(content);
	fillUp(ioStream, columnSize, writtenChars);
};

void showStat(IOStream& ioStream)
{
	std::array<TaskStatus_t, 10> statusArr;
	uint32_t totalRunTime;
	auto size = uxTaskGetSystemState(statusArr.data(), statusArr.size(), &totalRunTime);

	auto stateToString = [](eTaskState state)
	{
		switch (state)
		{
		case eRunning: return "RUNNING";
		case eReady: return "READY";
		case eBlocked: return "BLOCKED";
		case eSuspended: return "SUSPENDED";
		case eDeleted: return "DELETED";
		}
		return "UNKNOWN";
	};

	size_t maxTaskNameLength = 0;
	for (auto i = 0u; i < size; ++i)
	{
		maxTaskNameLength = std::max(maxTaskNameLength, strlen(statusArr[i].pcTaskName));
	}

	writeColumn(ioStream, "Task", maxTaskNameLength);
	writeColumn(ioStream, "State", 8);
	writeColumn(ioStream, "%", 3);
	ioStream.write("\n");

	for (auto i = 0u; i < size; ++i)
	{
		writeColumn(ioStream, statusArr[i].pcTaskName, maxTaskNameLength);
		writeColumn(ioStream, stateToString(statusArr[i].eCurrentState), 8);
		writeColumn(ioStream, statusArr[i].ulRunTimeCounter / (totalRunTime/100), 3);
		ioStream.write("\n");
	}
}
