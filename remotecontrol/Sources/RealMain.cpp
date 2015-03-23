/*!
 * \file RealMain.cpp
 * \brief RealMain
 */

#include "RealMain.h"

#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

#include <Console.h>
#include <LineInputStrategy.h>
#include <CommandParser.h>
#include "Platform.h"
#include "EventQueue.h"
#include "Console.h"
#include "LineInputStrategy.h"
#include "CriticalSection.h"
#include "WAIT1.h"
#include "EventHandler.h"
#include "Event.h"
#include "LED.h"
#include "Keys.h"
#include "Mealy.h"

#include "AS1.h"

void doLedHeartbeat(void);

class EchoConsole;

class CommandExecutorLineSink
{
public:
	explicit CommandExecutorLineSink(CommandParser* pCommandParser)
		: pCommandParser(pCommandParser)
	{
	}

	void lineCompleted(const String<80>& line)
	{
		pCommandParser->executeCommand(line);
	}

private:
	CommandParser* pCommandParser;
};

void writeCharToSerialPort(unsigned char c)
{
	while (AS1_SendChar(c) == ERR_TXFULL)
		;
}

using MyLineInputStrategy = LineInputStrategy<80, CommandExecutorLineSink, EchoConsole>;
using RemoteControlConsole = Console<decltype(writeCharToSerialPort)*, MyLineInputStrategy>;

class EchoConsole
{
public:
	explicit EchoConsole(RemoteControlConsole* pConsole)
		: pConsole(pConsole)
	{
	}

	template <typename... Args>
	void write(Args... args)
	{
		pConsole->write(args...);
	}

private:
	RemoteControlConsole* pConsole;
};

/**
 * C++ world main function
 */
void realMain()
{
	RemoteControlConsole* pConsole;

	CommandParser* pParser = nullptr;
	auto parser = makeParser(
		[&](const String<80>& error)
		{
			pConsole->write(error);
			pConsole->write("\r\n");
		},
		cmd("help", [&]()
		{
			String<10> cmds[10] = {};
			pParser->getAvailableCommands(cmds, 10);
			pConsole->write("available commands:\r\n");
			for (const auto& cmd : cmds)
			{
				if (cmd.size() > 0)
				{
					pConsole->write(cmd);
					pConsole->write("\r\n");
				}
			}
		}),
		cmd("echo", [&](const String<80>& param)
		{
			pConsole->write(param);
			pConsole->write("\r\n");
		}),
		cmd("add", [&](int32_t lhs, int32_t rhs)
		{
			pConsole->write(lhs + rhs);
			pConsole->write("\r\n");
		}),
		cmd("mult", [&](int32_t lhs, int32_t rhs)
		{
			pConsole->write(lhs * rhs);
			pConsole->write("\r\n");
		})
	);
	pParser = &parser;

	RemoteControlConsole console{writeCharToSerialPort, MyLineInputStrategy{CommandExecutorLineSink{&parser}, EchoConsole{&console}} };
	pConsole = &console;

	auto handleConsoleInput = [&]
    {
		byte inputChar;
		if (AS1_RecvChar(&inputChar) == ERR_OK)
		{
			console.rxChar(inputChar);
		}
    };

	console.write("Up and running\r\n");

	new(&eventQueue)MainEventQueue();
	for(;;){
		handleOneEvent(eventQueue,
			[]{},
			doLedHeartbeat,
			[&]{ console.write("Key_A_Pressed!\r\n"); pParser->executeCommand("help"); },
			[&]{ console.write("Key_B_Pressed!\r\n"); },
			[&]{ console.write("Key_C_Pressed!\r\n"); },
			[&]{ console.write("Key_D_Pressed!\r\n"); },
			[&]{ console.write("Key_E_Pressed!\r\n"); },
			[&]{ console.write("Key_F_Pressed!\r\n"); },
			[&]{ console.write("Key_J_Pressed!\r\n"); }
		);

		KEY_Scan();
		MEALY_Step();
		handleConsoleInput();
	}

	eventQueue.~EventQueue();
}

/**
 * Forward to C++ world
 */
void _main()
{
	realMain();
}

void doLedHeartbeat(void){
	LED1_On(); // RED RGB LED
	WAIT1_Waitms(50);
	LED1_Off();
}
