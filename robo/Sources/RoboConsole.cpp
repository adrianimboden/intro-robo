#include "RoboConsole.h"

#include <Platform.h>
#include <LineInputStrategy.h>
#include <CircularBuffer.h>
#include <HistoryController.h>
#include <ConsoleHelpers.h>
#include <CdcIOStream.h>
#include <Reflectance.h>
#include <Motor.h>
#include <MainControl.h>
#include <AutoArgsCommand.h>
#include "Event.h"

#include <BT1.h>

CommandParser& getCommandParser()
{
	static auto parser = makeParser(
		[&](IOStream& ioStream, const String<80>& error)
		{
			ioStream.write(error);
			ioStream.write("\r\n");
		},
		cmd("help", [&](IOStream& ioStream)
		{
			String<10> cmds[10] = {};
			getCommandParser().getAvailableCommands(cmds, 10);
			ioStream.write("available commands:\r\n");
			for (const auto& cmd : cmds)
			{
				if (cmd.size() > 0)
				{
					ioStream.write(cmd);
					ioStream.write("\r\n");
				}
			}
		}),
		cmd("showstat", showStat),
		cmd("refstat", REF_PrintStatus),
		cmd("startcalib",[&](){eventQueue.setEvent(Event::RefStartStopCalibration);}),
		cmd("stopcalib",[&](){eventQueue.setEvent(Event::RefStartStopCalibration);}),
		cmd("motstat", MOT_CmdStatus),
		cmd("motdir", MOT_CmdDir),
		cmd("motduty", MOT_CmdDuty),
		cmd("start", MainControl::notifyStartMove),
		cmd("setSpeed", MainControl::setSpeed)
	);

	return parser;
}

Console& getConsole()
{
	static auto console = makeConsole(
		CdcStaticIOStream<
			Serial2_RecvChar,
			Serial2_SendChar
		>{},
		makeLineInputStrategy<
			20 /*max cmdline size*/
			>(
				CommandExecutorLineSink{&getCommandParser()},
				SimpleEchoConsole{},
				HistoryController<
					String<20>, //line size
					5 //history length
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
