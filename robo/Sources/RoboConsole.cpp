#include "RoboConsole.h"

#include <LineInputStrategy.h>
#include <CircularBuffer.h>
#include <HistoryController.h>

#include <BT1.h>

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
	while (BT1_SendChar(c) == ERR_TXFULL)
		;
}

using MyLineInputStrategy = LineInputStrategy<40, CommandExecutorLineSink, EchoConsole, HistoryController<String<40>, 2>>;
using RemoteControlConsole = ConcreteConsole<decltype(writeCharToSerialPort)*, MyLineInputStrategy>;

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

struct RemoteControlConsoleData
{
	RemoteControlConsoleData(CommandParser& parser)
		: parser(parser)
		, console {
			writeCharToSerialPort,
			MyLineInputStrategy {
				CommandExecutorLineSink{&parser},
				EchoConsole{&console}
			}
		}
	{
	}

	CommandParser& parser;
	RemoteControlConsole console;
};

RemoteControlConsoleData& getData()
{
	static auto parser = makeParser(
		[&](const String<80>& error)
		{
			getData().console.write(error);
			getData().console.write("\r\n");
		},
		cmd("help", [&]()
		{
			String<10> cmds[10] = {};
			getData().parser.getAvailableCommands(cmds, 10);
			getData().console.write("available commands:\r\n");
			for (const auto& cmd : cmds)
			{
				if (cmd.size() > 0)
				{
					getData().console.write(cmd);
					getData().console.write("\r\n");
				}
			}
		}),
		cmd("echo", [&](const String<80>& param)
		{
			getData().console.write(param);
			getData().console.write("\r\n");
		}),
		cmd("add", [&](int32_t lhs, int32_t rhs)
		{
			getData().console.write(lhs + rhs);
			getData().console.write("\r\n");
		}),
		cmd("mult", [&](int32_t lhs, int32_t rhs)
		{
			getData().console.write(lhs * rhs);
			getData().console.write("\r\n");
		})
	);

	static RemoteControlConsoleData data{parser};
	return data;
}

CommandParser& getCommandParser()
{
	return getData().parser;
}

Console& getConsole()
{
	return getData().console;
}
