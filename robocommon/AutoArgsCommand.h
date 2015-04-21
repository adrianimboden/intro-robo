#pragma once

#include "CommandParser.h"
#include "Optional.h"
#include "FixedSizeString.h"

namespace detail
{

template <typename Fn>
class AutoArgsCommand
{
public:
	using traits = function_traits<Fn>;

	AutoArgsCommand(String<10> cmd, Fn fn)
		: cmd(cmd)
		, fn(fn)
	{
	}

	bool executeIfMatching(IOStream& ioStream, const String<MaxCommandLength>& cmdToExecute)
	{
		if (matches(cmdToExecute))
		{
			auto result = executeImpl(ioStream, cmdToExecute,
				FirstParameterIsIOStream<Fn>()
			);

			if (!result)
			{
				ioStream << "error: " << getSyntax();
			}
			return true;
		}
		else
		{
			return false;
		}
	}

	const String<10>& getCmd() const
	{
		return cmd;
	}

private:
	String<80> getSyntax() const
	{
		return getSyntaxImpl(FirstParameterIsIOStream<Fn>());
	}

	bool matches(const String<MaxCommandLength>& cmdToExecute)
	{
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

	//! called in case when the stored function does not want do do I/O
	bool executeImpl(IOStream& /*ioStream*/, const String<MaxCommandLength>& cmdToExecute, std::false_type)
	{
		auto parameters = detail::Parameters<traits::AmountOfArguments>::parseParameters(&cmdToExecute);

		if (!parameters)
			return false; //error during parsing

		return Executor<Fn, traits::AmountOfArguments>::executeImpl(fn, cmdToExecute, *parameters);
	}

	//! called in case when the stored function *does* want do do I/O
	bool executeImpl(IOStream& ioStream, const String<MaxCommandLength>& cmdToExecute, std::true_type)
	{
		using Adapter = typename traits::template GetIoStreamRemovedAdapter<Fn>::type;
		auto parameters = detail::Parameters<traits::AmountOfArguments - 1>::parseParameters(&cmdToExecute);

		if (!parameters)
			return false; //error during parsing

		return Executor<Adapter, traits::AmountOfArguments - 1>::executeImpl(Adapter{ioStream, fn}, cmdToExecute, *parameters);
	}

	//! called in case when the stored function does not want do do I/O
	String<80> getSyntaxImpl(std::false_type) const
	{
		String<80> syntax;
		syntax.append(cmd);
		Executor<Fn, traits::AmountOfArguments>::appendParams(syntax);

		return syntax;
	}

	//! called in case when the stored function *does* want do do I/O
	String<80> getSyntaxImpl(std::true_type) const
	{
		using Adapter = typename traits::template GetIoStreamRemovedAdapter<Fn>::type;
		String<80> syntax;
		syntax.append(cmd);
		Executor<Adapter, traits::AmountOfArguments - 1>::appendParams(syntax);

		return syntax;
	}

private:
	String<10> cmd;
	Fn fn;
};

}

template <typename Fn>
detail::AutoArgsCommand<Fn> cmd(const String<10>& cmd, Fn fn)
{
	return detail::AutoArgsCommand<Fn>(cmd, fn);
}
