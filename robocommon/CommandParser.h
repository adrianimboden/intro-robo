#pragma once

#ifndef __cplusplus
#error sorry, this header is c++ only
#endif

#include <FixedSizeString.h>
#include <NumberConversion.h>

#include <tuple>

template <typename T>
struct function_traits : public function_traits<decltype(&T::operator())>
{};

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...) const>
{
	enum { arity = sizeof...(Args) };

	using result_type = ReturnType;
	using arguments = std::tuple<Args...>;
	static constexpr size_t AmountOfArguments = std::tuple_size<arguments>::value;

	template <size_t i>
	struct arg
	{
		typedef typename std::tuple_element<i, arguments>::type type;
	};
};

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
			auto success = initSplitPoints(*pCmdToExecute, parameters.splitPoints);
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
		String<MaxParamLength> getParam() const
		{
			return String<MaxParamLength>{
				splitPoints[Index] + 1,
				((Index + 1) >= ExpectedAmountOfArguments) ? pCmdToExecute->end() : splitPoints[Index + 1]
			};
		}

	private:
		using ArrayOfSplitPoints = std::array<String<MaxCommandLength>::const_iterator, ExpectedAmountOfArguments>;

		explicit Parameters(const String<MaxCommandLength>* pCmdToExecute)
			: pCmdToExecute(pCmdToExecute)
		{
		}

		static bool initSplitPoints(const String<MaxCommandLength>& cmdToExecute, ArrayOfSplitPoints& splitPoints)
		{
			if (ExpectedAmountOfArguments > 0)
			{
				splitPoints[0] = findNextSplitter(cmdToExecute.begin(), cmdToExecute.end());
				if (splitPoints[0] == cmdToExecute.end())
				{
					return false; //not enough parameters
				}
				for (auto i = size_t{1}; i < ExpectedAmountOfArguments; ++i)
				{
					splitPoints[i] = findNextSplitter(splitPoints[i - 1] + 1, cmdToExecute.end());

					if (splitPoints[i] == cmdToExecute.end())
					{
						return false; //not enough parameters
					}
				}
			}

			return true;
		}

		static String<MaxCommandLength>::const_iterator findNextSplitter(String<MaxCommandLength>::const_iterator start, String<MaxCommandLength>::const_iterator end)
		{
			auto it = start;
			while (it != end)
			{
				if (*it == ' ')
				{
					break;
				}
				++it;
			}
			return it;
		}

	private:
		const String<MaxCommandLength>* pCmdToExecute;
		ArrayOfSplitPoints splitPoints;
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

		template <typename T>
		static auto parseParam(const String<MaxParamLength>& param, T& value) -> typename std::enable_if<std::is_integral<T>::value, bool>::type
		{
			auto result = stringToNumber<T>(param);
			if (!result)
			{
				return false;
			}
			value = *result;
			return true;
		}

		template <size_t MaxSize>
		static bool parseParam(const String<MaxParamLength>& param, String<MaxSize>& value)
		{
			value = String<MaxSize>{param.begin(), std::min(MaxSize, param.size())};
			return true; //possibly stripped
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

		bool execute(const String<MaxCommandLength>& cmdToExecute)
		{
			auto parameters = detail::Parameters<traits::AmountOfArguments>::parseParameters(&cmdToExecute);

			if (!parameters)
				return false; //error during parsing

			return Executor<Fn, traits::AmountOfArguments>::executeImpl(fn, cmdToExecute, *parameters);
		}

		String<80> getSyntax() const
		{
			String<80> syntax;
			syntax.append(cmd);
			Executor<Fn, traits::AmountOfArguments>::appendParams(syntax);

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
		static void doHandleCommand(const ErrorHandler& errorHandler, const String<MaxCommandLength>& cmdToExecute, const std::tuple<Commands...>& commands)
		{
			auto command = std::get<std::tuple_size<std::tuple<Commands...>>::value - Count>(commands);

			if (command.matches(cmdToExecute))
			{
				bool success = command.execute(cmdToExecute);
				if (!success)
				{
					String<80> err{"error. syntax: "};
					err.append(command.getSyntax());
					errorHandler(err);
				}
			}
			else
			{
				HandleCommandRecursive<Count - 1>::doHandleCommand(errorHandler, cmdToExecute, commands);
			}
		}
	};

	template <>
	struct HandleCommandRecursive<0>
	{
		//terminating case
		template <typename ErrorHandler, typename... Commands>
		static void doHandleCommand(const ErrorHandler& errorHandler, const String<MaxCommandLength>& cmdToExecute, const std::tuple<Commands...>& /*commands*/)
		{
			String<80> error = cmdToExecute;
			error.append(" not found");
			errorHandler(error);
		}
	};

	template <typename ErrorHandler, typename... Commands>
	void handleCommandRecursive(const ErrorHandler& errorHandler, const String<MaxCommandLength>& cmdToExecute, const std::tuple<Commands...>& commands)
	{
		HandleCommandRecursive<std::tuple_size<std::tuple<Commands...>>::value>::doHandleCommand(errorHandler, cmdToExecute, commands);
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
	virtual void executeCommand(const String<detail::MaxCommandLength>& command) = 0;
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

	void executeCommand(const String<detail::MaxCommandLength>& command) override
	{
		detail::handleCommandRecursive(errorHandler, command, commands);
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
