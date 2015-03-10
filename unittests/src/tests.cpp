#include <gmock/gmock.h>

#include <EventQueue.h>

using namespace testing;


enum class Event : uint8_t
{
	Button1Pressed,
	Button2Pressed = 33,
	AmountOfEvents //!< must be last
};

struct EmptyLock
{
};

TEST(EventQueue, When_I_do_nothing_the_event_is_not_set) {
	EventQueue<Event, EmptyLock> queue;

	ASSERT_THAT(queue.getAndResetEvent(Event::Button1Pressed), Eq(false));
}

TEST(EventQueue, When_I_set_an_event_it_is_set_afterwards) {
	EventQueue<Event, EmptyLock> queue;

	queue.setEvent(Event::Button1Pressed);
	ASSERT_THAT(queue.getAndResetEvent(Event::Button1Pressed), Eq(true));
}

TEST(EventQueue, When_I_set_an_event_and_then_reset_it_it_is_no_longer_set_afterwards) {
	EventQueue<Event, EmptyLock> queue;

	queue.setEvent(Event::Button1Pressed);
	queue.getAndResetEvent(Event::Button1Pressed);
	ASSERT_THAT(queue.getAndResetEvent(Event::Button1Pressed), Eq(false));
}

TEST(EventQueue, When_I_set_multiple_events_only_the_correct_one_is_reset) {
	EventQueue<Event, EmptyLock> queue;

	queue.setEvent(Event::Button1Pressed);
	queue.setEvent(Event::Button2Pressed);
	ASSERT_THAT(queue.getAndResetEvent(Event::Button1Pressed), Eq(true));
	ASSERT_THAT(queue.getAndResetEvent(Event::Button2Pressed), Eq(true));
}
