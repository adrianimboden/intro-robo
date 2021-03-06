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
#include <LegacyArgsCommand.h>
#include <LineEndingNormalizerIOStream.h>

#include "Event.h"
#include "Buzzer.h"
#include "QuadCalib.h"
#include "Tacho.h"
#include "Drive.h"
#include "Pid.h"
#include "Ultrasonic.h"
#include "Accel.h"
#include "RNet_App.h"
#include "RNET1.h"
#if PL_HAS_MUSIC_SHIELD
#include "Music.h"
#include "VS1053.h"
#endif

#include <BT1.h>

CommandParser& getCommandParser()
{
	static auto parser = makeParser(
		cmd("help", [&](IOStream& ioStream)
		{
			std::array<String<10>, 20>cmds{};
			getCommandParser().getAvailableCommands(cmds.data(), cmds.size());
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
		cmd("refstat", REF_PrintStatus),
		cmd("startcalib",[&](){eventQueue.setEvent(Event::RefStartStopCalibration);}),
		cmd("stopcalib",[&](){eventQueue.setEvent(Event::RefStartStopCalibration);}),
		cmd("motstat", MOT_CmdStatus),
		cmd("motdir", MOT_CmdDir),
		cmd("motduty", MOT_CmdDuty),
		cmd("startstop", []{ MainControl::notifyStartMove(!MainControl::hasStartMove()); }),
		cmd("setSpeed", MainControl::setSpeed),
		legacyCmd(BUZ_ParseCommand),
		legacyCmd(QUADCALIB_ParseCommand),
		legacyCmd(DRV_ParseCommand),
		legacyCmd(PID_ParseCommand),
		legacyCmd(TACHO_ParseCommand),
		legacyCmd(US_ParseCommand)
#if PL_HAS_ACCEL
		, legacyCmd(ACCEL_ParseCommand)
#endif
		, legacyCmd(RNETA_ParseCommand)
#if PL_HAS_MUSIC_SHIELD
		, legacyCmd(MUSIC_ParseCommand)
		, legacyCmd(VS_ParseCommand)
#endif
	);

	return parser;
}

Console& getConsole()
{
	static auto console = makeConsole(
		LineEndingNormalizerIOStream<
			CdcStaticIOStream<
				//Serial2_RecvChar,
				Serial1_RecvChar,
				//Serial2_SendChar
				Serial1_SendChar
			>
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
	console.getUnderlyingIoStream()->write("console ready...\r\n\r\n");
	for(;;)
	{
		console.pollInput();
		WAIT1_WaitOSms(1);
	}
}
