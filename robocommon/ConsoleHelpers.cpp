#include "ConsoleHelpers.h"

#include "Console.h"
#include <FreeRTOS.h>

#include <array>


constexpr int16_t tabSize = 2;
void fillUp(Console& console, int16_t columnSize, int16_t alreadyUsed)
{
	for (auto j = 0; j < (columnSize - alreadyUsed) + tabSize; ++j)
	{
		console.write(" ");
	}
};

template <typename T>
void writeColumn(Console& console, const T& content, int16_t columnSize)
{
	auto writtenChars = console.write(content);
	fillUp(console, columnSize, writtenChars);
};

void showStat(Console& console)
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

	writeColumn(console, "Task", maxTaskNameLength);
	writeColumn(console, "State", 8);
	writeColumn(console, "%", 3);
	console.write("\r\n");

	for (auto i = 0u; i < size; ++i)
	{
		writeColumn(console, statusArr[i].pcTaskName, maxTaskNameLength);
		writeColumn(console, stateToString(statusArr[i].eCurrentState), 8);
		writeColumn(console, statusArr[i].ulRunTimeCounter / (totalRunTime/100), 3);
		console.write("\r\n");
	}
}
