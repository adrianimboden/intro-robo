#include "RemoteControlConsole.h"

#include <Platform.h>
#include <LineInputStrategy.h>
#include <CircularBuffer.h>
#include <HistoryController.h>
#include <ConsoleHelpers.h>
#include <CdcIOStream.h>
#include <AutoArgsCommand.h>
#include <LegacyArgsCommand.h>
#include <LineEndingNormalizerIOStream.h>

extern "C" {
#include <CDC1.h>
}

#include "RNet_App.h"
#include "Remote.h"

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
		cmd("showstat", showStat),
		legacyCmd(RNETA_ParseCommand),
		legacyCmd(REMOTE_ParseCommand)
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
				SimpleEchoConsole{}
			)
	);

	return console;
}

void TASK_console(void*)
{
	Console& console = getConsole();
	console.getUnderlyingIoStream()->write("console ready...\n\n");
	for(;;)
	{
		console.pollInput();
		WAIT1_WaitOSms(1);
	}
}
