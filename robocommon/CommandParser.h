#pragma once

#ifndef __cplusplus
#error sorry, this header is c++ only
#endif

#include "FixedSizeString.h"
#include "NumberConversion.h"
#include "IOStream.h"

#include <tuple>

template <typename Fn, typename IOStream, typename... Params>
class RemoveIoStreamParamAdapter
{
public:
	RemoveIoStreamParamAdapter(IOStream& ioStream, Fn fn)
		: ioStream(ioStream)
		, fn(std::move(fn))
	{
	}

	void operator()(Params... params) const
	{
		fn(ioStream, params...);
	}

private:
	IOStream& ioStream;
	Fn fn;
};

template <typename T>
struct function_traits : public function_traits<decltype(&T::operator())>
{};

//! Free function
template <typename ReturnType, typename... Args>
struct function_traits<ReturnType(*)(Args...)>
{
	enum { arity = sizeof...(Args) };

	using result_type = ReturnType;
	using arguments = std::tuple<Args...>;
	static constexpr size_t AmountOfArguments = std::tuple_size<arguments>::value;

	template <size_t i>
	struct arg
	{
		using type = typename std::tuple_element<i, arguments>::type;
	};

	template <typename Fn>
	struct GetIoStreamRemovedAdapter
	{
		using type = RemoveIoStreamParamAdapter<Fn, Args...>;
	};
};

//! Member function
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...) const> : public function_traits<ReturnType(*)(Args...)>
{
};

template <typename Fn, size_t AmountOfArguments>
struct FirstParameterIsIOStreamImpl : public std::is_same<typename function_traits<Fn>::template arg<0>::type, IOStream&> { };

template <typename Fn>
struct FirstParameterIsIOStreamImpl<Fn, 0> : public std::false_type { }; //no params -> no iostream param


template <typename Fn>
struct FirstParameterIsIOStream : public FirstParameterIsIOStreamImpl<Fn, function_traits<Fn>::AmountOfArguments>
{ };

namespace detail
{
	constexpr size_t MaxCommandLength = 80;
	constexpr size_t MaxParamLength = 10;

	template <typename T>
	struct AlwaysFalse
	{
		enum { value = 0 };
	};

	template <size_t ExpectedAmountOfArguments>
	class Parameters
	{
	public:
		static optional<Parameters> parseParameters(const String<MaxCommandLength>* pCmdToExecute)
		{
			Parameters parameters{pCmdToExecute};
			auto success = tryInitFromCmd(*pCmdToExecute, parameters.parameters);
			if (success)
			{
				return parameters;
			}
			else
			{
				return { };
			}
		}

		template <size_t Index>
		String<MaxCommandLength> getParam() const
		{
			return String<MaxCommandLength>{parameters[Index].first, parameters[Index].second};
		}

	private:
		using ArrayOfParameters = std::array<std::pair<String<MaxCommandLength>::const_iterator, String<MaxCommandLength>::const_iterator>, ExpectedAmountOfArguments>;

		explicit Parameters(const String<MaxCommandLength>* pCmdToExecute)
			: pCmdToExecute(pCmdToExecute)
		{
		}

		static bool tryInitFromCmd(const String<MaxCommandLength>& cmdToExecute, ArrayOfParameters& parameters)
		{
			enum State
			{
				ReadCmd,
				ReadDelimiter,
				ReadParam,
				ReadQuotedParam,
			};

			auto state = ReadCmd;

			auto currParam = 0u;

			decltype(cmdToExecute.begin()) paramStart = cmdToExecute.begin();
			char usedQuotationCharacter = '\0';

			const auto cmdSize = cmdToExecute.size();
			for (auto i = 0u; i <= cmdSize; ++i)
			{
				char c = (i < cmdSize) ? cmdToExecute[i] : '\0';
				switch (state)
				{
				case ReadCmd:
					{
						if (isspace(c))
						{
							state = ReadDelimiter;
						}
						else
						{
							state = ReadCmd;
						}
					}
					break;
				case ReadDelimiter:
					{
						if (isspace(c))
						{
							state = ReadDelimiter;
						}
						else if (c == '\"' || c == '\'')
						{
							if (cmdToExecute.begin() + i + 1 > cmdToExecute.end())
								return false; //quote at end of string

							paramStart = cmdToExecute.begin() + i + 1;
							usedQuotationCharacter = c;
							state = ReadQuotedParam;
						}
						else
						{
							paramStart = cmdToExecute.begin() + i;
							state = ReadParam;
						}
					}
					break;
				case ReadParam:
					{
						if (isspace(c) || c == '\0')
						{
							if (currParam >= parameters.size())
								return false; //too many arguments

							parameters[currParam] = {paramStart, cmdToExecute.begin() + i};
							++currParam;
							state = ReadDelimiter;
						}
						else
						{
							state = ReadParam;
						}
					}
					break;
				case ReadQuotedParam:
					{
						if (c == usedQuotationCharacter)
						{
							if (currParam >= parameters.size())
								return false; //too many arguments

							parameters[currParam] = {paramStart, cmdToExecute.begin() + i};
							++currParam;
							state = ReadDelimiter;
						}
						else
						{
							state = ReadQuotedParam;
						}
					}
					break;
				}
			}

			return (currParam == ExpectedAmountOfArguments);
		}

	private:
		const String<MaxCommandLength>* pCmdToExecute;
		ArrayOfParameters parameters;
	};

	template <typename Fn, size_t ArgNo>
	struct Executor
	{
		using traits = function_traits<Fn>;
		static constexpr size_t Index = (traits::AmountOfArguments - ArgNo);
		using ProcessedParamType = typename std::decay<typename std::tuple_element<Index, typename traits::arguments>::type>::type;

		template <typename... Args>
		static bool executeImpl(const Fn& fn, const String<MaxCommandLength>& cmdToExecute, const detail::Parameters<traits::AmountOfArguments>& parameters, Args... args)
		{
			auto param = parameters.template getParam<Index>();

			ProcessedParamType parsedParam;
			bool parseOk = parseParam(param, parsedParam);
			if (!parseOk)
			{
				return false;
			}
			return Executor<Fn, ArgNo-1>::executeImpl(fn, cmdToExecute, parameters, args..., std::move(parsedParam));
		}

		template <size_t MaxSize>
		static void appendParams(String<MaxSize>& value)
		{
			value.append(" ");
			appendParamTypeName(value, static_cast<ProcessedParamType*>(nullptr));
			return Executor<Fn, ArgNo-1>::appendParams(value);
		}

		template <typename ParamStringType, typename T>
		static auto parseParam(const ParamStringType& param, T& value) -> typename std::enable_if<std::is_integral<T>::value, bool>::type
		{
			auto result = stringToNumber<T>(param);
			if (!result)
			{
				return false;
			}
			value = *result;
			return true;
		}

		template <typename ParamStringType, size_t MaxSize>
		static bool parseParam(const ParamStringType& param, String<MaxSize>& value)
		{
			if (param.size() > MaxSize)
			{
				return false;
			}

			value = String<MaxSize>{param.begin(), std::min(MaxSize, param.size())};
			return true;
		}

		static bool parseParam(...)
		{
			static_assert(AlwaysFalse<Fn>::value, "unsupported parameter type used in command function");
			return false;
		}


		template <typename T, size_t MaxSize>
		static auto appendParamTypeName(String<MaxSize>& value, T* /*dummy*/) -> typename std::enable_if<std::is_integral<T>::value, void>::type
		{
			value.append("num");
		}

		template <size_t MaxSizeValue, size_t MaxSizeDummy>
		static void appendParamTypeName(String<MaxSizeValue>& value, String<MaxSizeDummy>* /*dummy*/)
		{
			value.append("str");
		}
	};

	template <typename Fn>
	struct Executor<Fn, 0>
	{
		using traits = function_traits<Fn>;

		template <typename... Args>
		static bool executeImpl(const Fn& fn, const String<MaxCommandLength>& /*cmdToExecute*/, const detail::Parameters<traits::AmountOfArguments>& /*parameters*/, Args... args)
		{
			fn(args...);
			return true;
		}

		template <size_t MaxSize>
		static void appendParams(String<MaxSize>& /*value*/)
		{
		}
	};

	template <typename Fn>
	class Command
	{
	public:
		using traits = function_traits<Fn>;

		Command(String<10> cmd, Fn fn)
			: cmd(cmd)
			, fn(fn)
		{
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

		bool execute(IOStream& ioStream, const String<MaxCommandLength>& cmdToExecute)
		{
			return executeImpl(ioStream, cmdToExecute,
				FirstParameterIsIOStream<Fn>()
			);
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

		String<80> getSyntax() const
		{
			return getSyntaxImpl(FirstParameterIsIOStream<Fn>());
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

		const String<10>& getCmd() const
		{
			return cmd;
		}

	private:
		String<10> cmd;
		Fn fn;
	};

	template <size_t Count>
	struct HandleCommandRecursive
	{
		template <typename ErrorHandler, typename... Commands>
		static void doHandleCommand(IOStream& ioStream, const ErrorHandler& errorHandler, const String<MaxCommandLength>& cmdToExecute, const std::tuple<Commands...>& commands)
		{
			auto command = std::get<std::tuple_size<std::tuple<Commands...>>::value - Count>(commands);

			if (command.matches(cmdToExecute))
			{
				bool success = command.execute(ioStream, cmdToExecute);
				if (!success)
				{
					String<80> err{"error. syntax: "};
					err.append(command.getSyntax());
					errorHandler(err);
				}
			}
			else
			{
				HandleCommandRecursive<Count - 1>::doHandleCommand(ioStream, errorHandler, cmdToExecute, commands);
			}
		}
	};

	template <>
	struct HandleCommandRecursive<0>
	{
		//terminating case
		template <typename ErrorHandler, typename... Commands>
		static void doHandleCommand(IOStream& /*ioStream*/, const ErrorHandler& errorHandler, const String<MaxCommandLength>& cmdToExecute, const std::tuple<Commands...>& /*commands*/)
		{
			String<80> error = cmdToExecute;
			error.append(" not found");
			errorHandler(error);
		}
	};

	template <typename ErrorHandler, typename... Commands>
	void handleCommandRecursive(IOStream& ioStream, const ErrorHandler& errorHandler, const String<MaxCommandLength>& cmdToExecute, const std::tuple<Commands...>& commands)
	{
		HandleCommandRecursive<std::tuple_size<std::tuple<Commands...>>::value>::doHandleCommand(ioStream, errorHandler, cmdToExecute, commands);
	}
}

template <typename Fn>
detail::Command<Fn> cmd(const String<10>& cmd, Fn fn)
{
	return detail::Command<Fn>(cmd, fn);
}

class CommandParser
{
public:
	virtual void executeCommand(IOStream& ioStream, const String<detail::MaxCommandLength>& command) = 0;
	virtual void getAvailableCommands(String<10> list[], size_t maxElements) = 0;
};

namespace detail
{

template <typename... Commands>
using AvailableCommands = std::array<String<10>, std::tuple_size<std::tuple<Commands...>>::value>;

template <size_t no>
struct AddCommandImpl
{
	template <typename... Commands>
	static void addCommand(std::tuple<Commands...> commands, AvailableCommands<Commands...>& cmds)
	{
		constexpr auto Index = std::tuple_size<std::tuple<Commands...>>::value - no;
		cmds[Index] = std::get<Index>(commands).getCmd();
		AddCommandImpl<no - 1>::addCommand(commands, cmds);
	}
};

template <>
struct AddCommandImpl<0>
{
	template <typename... Commands>
	static void addCommand(std::tuple<Commands...> /*commands*/, AvailableCommands<Commands...>& /*cmds*/)
	{
	}
};

}

template <typename ErrorHandler, typename... Commands>
class ConcreteCommandParser : public CommandParser
{
public:
	explicit ConcreteCommandParser(ErrorHandler errorHandler, std::tuple<Commands...> commands)
		: errorHandler(std::move(errorHandler))
		, commands(std::move(commands))
	{
	}

	void executeCommand(IOStream& ioStream, const String<detail::MaxCommandLength>& command) override
	{
		return executeCommandImpl(ioStream, command, FirstParameterIsIOStream<ErrorHandler>());
	}

	//! called in case when the help function does not want do do I/O
	void executeCommandImpl(IOStream& ioStream, const String<detail::MaxCommandLength>& command, std::false_type)
	{
		detail::handleCommandRecursive(ioStream, errorHandler, command, commands);
	}

	//! called in case when the help function *does* want do do I/O
	void executeCommandImpl(IOStream& ioStream, const String<detail::MaxCommandLength>& command, std::true_type)
	{
		using Adapter = typename function_traits<ErrorHandler>::template GetIoStreamRemovedAdapter<ErrorHandler>::type;
		detail::handleCommandRecursive(ioStream, Adapter{ioStream, errorHandler}, command, commands);
	}


	detail::AvailableCommands<Commands...> getAvailableCommands()
	{
		detail::AvailableCommands<Commands...> availableCommands;

		detail::AddCommandImpl<std::tuple_size<std::tuple<Commands...>>::value>::addCommand(commands, availableCommands);

		return availableCommands;
	}

	void getAvailableCommands(String<10> cmds[], size_t maxElements) override
	{
		auto availableCommands = getAvailableCommands();
		ASSERT(availableCommands.size() <= maxElements);
		for (auto i = size_t{0}; i < availableCommands.size(); ++i)
		{
			cmds[i] = availableCommands[i];
		}
	}

private:
	ErrorHandler errorHandler;
	std::tuple<Commands...> commands;
};

template <typename ErrorHandler, typename... Commands>
ConcreteCommandParser<ErrorHandler, Commands...> makeParser(ErrorHandler errorHandler, Commands... commands)
{
	return ConcreteCommandParser<ErrorHandler, Commands...>(errorHandler, std::make_tuple(commands...));
}

class CommandExecutorLineSink
{
public:
	explicit CommandExecutorLineSink(CommandParser* pCommandParser)
		: pCommandParser(pCommandParser)
	{
	}

	template <typename TIOStream>
	void lineCompleted(TIOStream& ioStream, const String<80>& line)
	{
		pCommandParser->executeCommand(ioStream, line);
	}

private:
	CommandParser* pCommandParser;
};
