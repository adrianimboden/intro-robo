#pragma once

#include <String.h>
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
		static void executeImpl(const Fn& fn, const CmdString& cmdToExecute, std::array<CmdString::const_iterator, traits::AmountOfArguments> splitPoints, Args... args)
		{
			ParamString param{
				splitPoints[Index] + 1,
				((Index + 1) >= traits::AmountOfArguments) ? cmdToExecute.end() : splitPoints[Index + 1]
			};

			ProcessedParamType parsedParam;
			parseParam(param, parsedParam);
			Executor<Fn, ArgNo-1>::executeImpl(fn, cmdToExecute, splitPoints, args..., std::move(parsedParam));
		}

		template <typename T>
		static auto parseParam(const ParamString& param, T& value) -> typename std::enable_if<std::is_integral<T>::value, void>::type
		{
			value = *stringToNumber<T>(param);
		}

		template <size_t MaxSize>
		static void parseParam(const ParamString& param, String<MaxSize>& value)
		{
			value = String<MaxSize>{param.begin(), std::min(MaxSize, param.size())};
		}

		static void parseParam(...)
		{
			static_assert(AlwaysFalse<Fn>::value, "unsupported parameter type used in command function");
		}
	};

	template <typename Fn>
	struct Executor<Fn, 0>
	{
		using traits = function_traits<Fn>;

		template <typename... Args>
		static void executeImpl(const Fn& fn, const CmdString& /*cmdToExecute*/, std::array<CmdString::const_iterator, traits::AmountOfArguments> /*splitPoints*/, Args... args)
		{
			fn(args...);
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

		void execute(const CmdString& cmdToExecute)
		{
			std::array<CmdString::const_iterator, traits::AmountOfArguments> splitPoints;

			splitPoints[0] = findNextSplitter(cmdToExecute.begin(), cmdToExecute.end());
			for (auto i = size_t{1}; i < traits::AmountOfArguments; ++i)
			{
				splitPoints[i] = findNextSplitter(splitPoints[i - 1] + 1, cmdToExecute.end());
			}

			Executor<Fn, traits::AmountOfArguments>::executeImpl(fn, cmdToExecute, splitPoints);
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
		template <typename... Commands>
		static void doHandleCommand(const CmdString& cmdToExecute, const std::tuple<Commands...>& commands)
		{
			auto command = std::get<std::tuple_size<std::tuple<Commands...>>::value - Count>(commands);

			if (command.matches(cmdToExecute))
			{
				command.execute(cmdToExecute);
			}
			else
			{
				HandleCommandRecursive<Count - 1>::doHandleCommand(cmdToExecute, commands);
			}
		}
	};

	template <>
	struct HandleCommandRecursive<0>
	{
		template <typename... Commands>
		static void doHandleCommand(const CmdString& cmdToExecute, const std::tuple<Commands...>& commands)
		{
			//terminating case
		}
	};

	template <typename... Commands>
	void handleCommandRecursive(const CmdString& cmdToExecute, const std::tuple<Commands...>& commands)
	{
		HandleCommandRecursive<std::tuple_size<std::tuple<Commands...>>::value>::doHandleCommand(cmdToExecute, commands);
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

template <typename... Commands>
class ConcreteCommandParser : public CommandParser
{
public:
	explicit ConcreteCommandParser(std::tuple<Commands...> commands)
		: commands(std::move(commands))
	{
	}

	void executeCommand(const detail::CmdString& command) override
	{
		detail::handleCommandRecursive(command, commands);
	}

private:
	std::tuple<Commands...> commands;
};

template <typename... Commands>
ConcreteCommandParser<Commands...> makeParser(Commands... commands)
{
	return ConcreteCommandParser<Commands...>(std::make_tuple(commands...));
}
