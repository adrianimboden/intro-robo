#include "RemoteControlConsole.h"

#include <Platform.h>
#include <LineInputStrategy.h>
#include <CircularBuffer.h>
#include <HistoryController.h>
#include <ConsoleHelpers.h>
#include <CdcIOStream.h>

extern "C" {
#include <CDC1.h>
}

CommandParser& getCommandParser()
{
	static auto parser = makeParser(
		[&](const String<80>& error)
		{
			getConsole().write(error);
			getConsole().write("\r\n");
		},
		cmd("help", [&]()
		{
			String<10> cmds[10] = {};
			getCommandParser().getAvailableCommands(cmds, 10);
			getConsole().write("available commands:\r\n");
			for (const auto& cmd : cmds)
			{
				if (cmd.size() > 0)
				{
					getConsole().write(cmd);
					getConsole().write("\r\n");
				}
			}
		}),
		cmd("echo", [&](const String<80>& param)
		{
			getConsole().write(param);
			getConsole().write("\r\n");
		}),
		cmd("add", [&](int32_t lhs, int32_t rhs)
		{
			getConsole().write(lhs + rhs);
			getConsole().write("\r\n");
		}),
		cmd("mult", [&](int32_t lhs, int32_t rhs)
		{
			getConsole().write(lhs * rhs);
			getConsole().write("\r\n");
		}),
		cmd("showstat", []{ showStat(getConsole()); })
	);

	return parser;
}

Console& getConsole()
{
	static auto console = makeConsole(
		CdcStaticIOStream<
			CDC1_RecvChar,
			CDC1_SendChar
		>{},
		makeLineInputStrategy<
			20 /*max cmdline size*/
			>(
				CommandExecutorLineSink{&getCommandParser()},
				SimpleEchoConsole{},
				HistoryController<
					String<20>, //line size
					2 //history length
				>{}
			)
	);

	return console;
}

void TASK_console(void*)
{
	Console& console = getConsole();
	console.write("console ready...");
	for(;;)
	{
		console.pollInput();
	}
}
