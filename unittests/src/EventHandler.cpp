#include <gmock/gmock.h>

#include <EventQueue.h>
#include <EventHandler.h>

using namespace testing;


enum class Event : uint8_t
{
	Button1Pressed,
	Button2Pressed,
	AmountOfEvents //!< must be last
};

struct EmptyLock
{
};
using Queue = EventQueue<Event, EmptyLock>;

TEST(EventHandler, When_I_set_an_event_the_handle_function_calls_my_callback_for_variadic_arguments) {
	Queue queue;

	queue.setEvent(Event::Button1Pressed);

	auto button1Events = size_t{};
	handleOneEvent(queue,
		[&]{ ++button1Events; },
		[]{ }
	);
	ASSERT_THAT(button1Events, Eq(1));
}

TEST(EventHandler, When_I_set_an_event_the_handle_function_calls_my_callback_for_a_tuple) {
	Queue queue;

	queue.setEvent(Event::Button1Pressed);

	auto button1Events = size_t{};
	handleOneEvent(queue,
		std::make_tuple(
			[&]{ ++button1Events; },
			[]{ }
		)
	);
	ASSERT_THAT(button1Events, Eq(1));
}

TEST(EventHandler, When_I_set_two_events_the_higher_priority_will_be_handled_first) {
	Queue queue;

	queue.setEvent(Event::Button2Pressed);
	queue.setEvent(Event::Button1Pressed);

	auto button1Events = size_t{};
	auto button2Events = size_t{};
	auto doHandle = [&]{
		handleOneEvent(queue,
			std::make_tuple(
				[&]{ ++button1Events; },
				[&]{ ++button2Events; }
			)
		);
	};

	doHandle();
	ASSERT_THAT(button1Events, Eq(1));
	ASSERT_THAT(button2Events, Eq(0));
	doHandle();
	ASSERT_THAT(button1Events, Eq(1));
	ASSERT_THAT(button2Events, Eq(1));
}
