#include "RemoteControlConsole.h"

#include <Platform.h>
#include <LineInputStrategy.h>
#include <CircularBuffer.h>
#include <HistoryController.h>
#include <ConsoleHelpers.h>
#include <CdcIOStream.h>
#include <AutoArgsCommand.h>
#include <LineEndingNormalizerIOStream.h>

extern "C" {
#include <CDC1.h>
}

CommandParser& getCommandParser()
{
	static auto parser = makeParser(
		cmd("help", [&](IOStream& ioStream)
		{
			String<10> cmds[10] = {};
			getCommandParser().getAvailableCommands(cmds, 10);
			ioStream.write("available commands:\n");
			for (const auto& cmd : cmds)
			{
				if (cmd.size() > 0)
				{
					ioStream.write(cmd);
					ioStream.write("\n");
				}
			}
		}),
		cmd("echo", [&](IOStream& ioStream, const String<80>& param)
		{
			ioStream.write(param);
			ioStream.write("\n");
		}),
		cmd("add", [&](IOStream& ioStream, int32_t lhs, int32_t rhs)
		{
			ioStream.write(lhs + rhs);
			ioStream.write("\n");
		}),
		cmd("mult", [&](IOStream& ioStream, int32_t lhs, int32_t rhs)
		{
			ioStream.write(lhs * rhs);
			ioStream.write("\n");
		}),
		cmd("showstat", showStat)
	);

	return parser;
}

Console& getConsole()
{
	static auto console = makeConsole(
			LineEndingNormalizerIOStream<
			CdcStaticIOStream<
				CDC1_RecvChar,
				CDC1_SendChar
			>
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
	console.getUnderlyingIoStream()->write("console ready...");
	for(;;)
	{
		console.pollInput();
	}
}
