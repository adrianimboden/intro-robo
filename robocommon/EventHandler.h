#pragma once

#ifndef __cplusplus
#error sorry, this header is c++ only
#endif

#include <tuple>

namespace detail
{
	template <typename Queue, size_t Count>
	struct HandleOneEventRecursive
	{
		template <typename... HandlerFns>
		static void doHandleEvent(Queue& queue, const std::tuple<HandlerFns...>& handlers)
		{
			constexpr auto EventNo = Queue::AmountOfEvents - Count;
			auto eventHasBeenSet = queue.getAndResetEvent(static_cast<typename Queue::EnumType>(EventNo));
			if (eventHasBeenSet)
			{
				std::get<EventNo>(handlers)();
			}
			else
			{
				HandleOneEventRecursive<Queue, Count - 1>::doHandleEvent(queue, handlers);
			}
		}
	};

	template <typename Queue>
	struct HandleOneEventRecursive<Queue, 0>
	{
		template <typename... HandlerFns>
		static void doHandleEvent(Queue&, const std::tuple<HandlerFns...>&)
		{
			//terminating case
		}
	};

	template <typename Queue, typename... HandlerFns>
	void handleOneEventRecursive(Queue& queue, const std::tuple<HandlerFns...>& handlers)
	{
		HandleOneEventRecursive<Queue, Queue::AmountOfEvents>::doHandleEvent(queue, handlers);
	}
}

template <typename Queue, typename... HandlerFns>
void handleOneEvent(Queue& queue, std::tuple<HandlerFns...> handlers)
{
	static_assert(std::tuple_size<std::tuple<HandlerFns...>>::value == Queue::AmountOfEvents, "the amount of handlers must match the amount of events");
	detail::handleOneEventRecursive(queue, handlers);
}

template <typename Queue, typename... HandlerFns>
void handleOneEvent(Queue& queue, HandlerFns... handlers)
{
	handleOneEvent(queue, std::make_tuple(handlers...));
}
