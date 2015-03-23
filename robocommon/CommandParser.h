#pragma once

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
	using CmdString = String<MaxCommandLength>;
	using ParamString = String<MaxCommandLength>;

	template <typename T>
	struct AlwaysFalse
	{
		enum { value = 0 };
	};

	template <typename Fn, size_t ArgNo>
	struct Executor
	{
		using traits = function_traits<Fn>;
		static constexpr size_t Index = (traits::AmountOfArguments - ArgNo);
		using ProcessedParamType = typename std::decay<typename std::tuple_element<Index, typename traits::arguments>::type>::type;

		template <typename... Args>
		static bool executeImpl(const Fn& fn, const CmdString& cmdToExecute, std::array<CmdString::const_iterator, traits::AmountOfArguments> splitPoints, Args... args)
		{
			ParamString param{
				splitPoints[Index] + 1,
				((Index + 1) >= traits::AmountOfArguments) ? cmdToExecute.end() : splitPoints[Index + 1]
			};

			ProcessedParamType parsedParam;
			bool parseOk = parseParam(param, parsedParam);
			if (!parseOk)
			{
				return false;
			}
			return Executor<Fn, ArgNo-1>::executeImpl(fn, cmdToExecute, splitPoints, args..., std::move(parsedParam));
		}

		template <size_t MaxSize>
		static void appendParams(String<MaxSize>& value)
		{
			value.append(" ");
			appendParamTypeName(value, static_cast<ProcessedParamType*>(nullptr));
			return Executor<Fn, ArgNo-1>::appendParams(value);
		}

		template <typename T>
		static auto parseParam(const ParamString& param, T& value) -> typename std::enable_if<std::is_integral<T>::value, bool>::type
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
		static bool parseParam(const ParamString& param, String<MaxSize>& value)
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
		static bool executeImpl(const Fn& fn, const CmdString& /*cmdToExecute*/, std::array<CmdString::const_iterator, traits::AmountOfArguments> /*splitPoints*/, Args... args)
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

		bool matches(const CmdString& cmdToExecute)
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

		bool execute(const CmdString& cmdToExecute)
		{
			std::array<CmdString::const_iterator, traits::AmountOfArguments> splitPoints;

			if (traits::AmountOfArguments > 0)
			{
				splitPoints[0] = findNextSplitter(cmdToExecute.begin(), cmdToExecute.end());
				if (splitPoints[0] == cmdToExecute.end())
				{
					return false; //not enough parameters
				}
				for (auto i = size_t{1}; i < traits::AmountOfArguments; ++i)
				{
					if (splitPoints[i - 1] == cmdToExecute.end())
					{
						return false; //not enough parameters
					}

					splitPoints[i] = findNextSplitter(splitPoints[i - 1] + 1, cmdToExecute.end());
				}
			}

			return Executor<Fn, traits::AmountOfArguments>::executeImpl(fn, cmdToExecute, splitPoints);
		}

		String<80> getSyntax() const
		{
			String<80> syntax;
			syntax.append(cmd);
			Executor<Fn, traits::AmountOfArguments>::appendParams(syntax);

			return syntax;
		}

	private:
		CmdString::const_iterator findNextSplitter(CmdString::const_iterator start, CmdString::const_iterator end)
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
		String<10> cmd;
		Fn fn;
	};

	template <size_t Count>
	struct HandleCommandRecursive
	{
		template <typename ErrorHandler, typename... Commands>
		static void doHandleCommand(const ErrorHandler& errorHandler, const CmdString& cmdToExecute, const std::tuple<Commands...>& commands)
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
		static void doHandleCommand(const ErrorHandler& errorHandler, const CmdString& cmdToExecute, const std::tuple<Commands...>& /*commands*/)
		{
			String<80> error = cmdToExecute;
			error.append(" not found");
			errorHandler(error);
		}
	};

	template <typename ErrorHandler, typename... Commands>
	void handleCommandRecursive(const ErrorHandler& errorHandler, const CmdString& cmdToExecute, const std::tuple<Commands...>& commands)
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
	virtual void executeCommand(const detail::CmdString& command) = 0;
};

template <typename ErrorHandler, typename... Commands>
class ConcreteCommandParser : public CommandParser
{
public:
	explicit ConcreteCommandParser(ErrorHandler errorHandler, std::tuple<Commands...> commands)
		: errorHandler(std::move(errorHandler))
		, commands(std::move(commands))
	{
	}

	void executeCommand(const detail::CmdString& command) override
	{
		detail::handleCommandRecursive(errorHandler, command, commands);
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
