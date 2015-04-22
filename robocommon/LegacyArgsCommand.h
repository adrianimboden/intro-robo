#pragma once

#include "CommandParser.h"
#include "Optional.h"
#include "FixedSizeString.h"


extern "C"
{

struct Adapter
{
};

struct CLS1_StdIOType
{
	Adapter* stdIn;
	Adapter* stdOut;
	Adapter* stdErr;
	Adapter* keyPressed;
};

}

constexpr const char* CLS1_CMD_HELP = "help";
constexpr const char* CLS1_CMD_STATUS = "status";

namespace detail
{
	class CppAdapter : public Adapter
	{
	public:
		virtual void sendHelpStr(const unsigned char* name, const unsigned char* text) const = 0;
		virtual void sendStr(const unsigned char* text) const = 0;
	};

	template <typename FnSendHelpStr, typename FnSendStr>
	class ConcreteAdapter : public CppAdapter
	{
	public:
		ConcreteAdapter(FnSendHelpStr fnSendHelpStr, FnSendStr fnSendStr)
			: fnSendHelpStr(fnSendHelpStr)
			, fnSendStr(fnSendStr)
		{
		}

		void sendHelpStr(const unsigned char* name, const unsigned char* text) const override
		{
			fnSendHelpStr(name, text);
		}

		void sendStr(const unsigned char* text) const override
		{
			fnSendStr(text);
		}

	private:
		FnSendHelpStr fnSendHelpStr;
		FnSendStr fnSendStr;
	};

	template <typename FnSendHelpStr, typename FnSendStr>
	ConcreteAdapter<FnSendHelpStr, FnSendStr> makeAdapter(FnSendHelpStr fnSendHelpStr, FnSendStr fnSendStr)
	{
		return ConcreteAdapter<FnSendHelpStr, FnSendStr>(fnSendHelpStr, fnSendStr);
	}
}

using ParseCommandFn = uint8_t(*)(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io);

inline void CLS1_SendHelpStr(const unsigned char* name, const unsigned char* text, const Adapter *io)
{
	static_cast<const detail::CppAdapter*>(io)->sendHelpStr(name, text);
}

inline void CLS1_SendStr(const unsigned char* text, const Adapter *io)
{
	static_cast<const detail::CppAdapter*>(io)->sendStr(text);
}


namespace detail
{

class LegacyArgsCommand
{
public:
	LegacyArgsCommand(ParseCommandFn fn)
		: fn(fn)
	{
	}

	bool executeIfMatching(IOStream& ioStream, const String<MaxCommandLength>& cmdToExecute)
	{
		auto adapter = makeAdapter(
			[&](const unsigned char* name, const unsigned char* text)
			{
				ioStream << reinterpret_cast<const char*>(name) << "\t" << reinterpret_cast<const char*>(text);
			},
			[&](const unsigned char* text)
			{
				ioStream << reinterpret_cast<const char*>(text);
			}
		);
		CLS1_StdIOType io{&adapter, &adapter, &adapter, &adapter};

		bool handled = false;
		fn(reinterpret_cast<const unsigned char*>(cmdToExecute.begin()), &handled, &io);

		if (handled)
		{
			return true;
		}
		else
		{
			if (matches(cmdToExecute))
			{
				ioStream << "could not execute command:\n";
				fn(reinterpret_cast<const unsigned char*>(CLS1_CMD_HELP), &handled, &io);
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	String<10> getCmd() const
	{
		String<10> cmd;

		auto adapter = makeAdapter(
			[&](const unsigned char* name, const unsigned char* /*text*/)
			{
				if (cmd.empty())
				{
					cmd = reinterpret_cast<const char*>(name);
				}
			},
			[](const unsigned char* /*text*/)
			{
			}
		);
		CLS1_StdIOType io{&adapter, &adapter, &adapter, &adapter};

		bool handled = false;
		fn(reinterpret_cast<const unsigned char*>(CLS1_CMD_HELP), &handled, &io);

		return cmd;
	}

private:
	bool matches(const String<MaxCommandLength>& cmdToExecute)
	{
		auto cmd = getCmd();
		if (cmd.size() > cmdToExecute.size())
			return false;

		for (auto i = size_t{0}; i < cmd.size(); ++i)
		{
			if (cmd[i] != cmdToExecute[i])
			{
				return false;
			}
		}
		return //either
			((cmd.size() == cmdToExecute.size()) || //the command matches, or:
			(cmdToExecute[cmd.size()] == ' '));		//there are parameters after the command (space)
	}

private:
	ParseCommandFn fn;
};

}

inline detail::LegacyArgsCommand legacyCmd(ParseCommandFn fn)
{
	return detail::LegacyArgsCommand(fn);
}
