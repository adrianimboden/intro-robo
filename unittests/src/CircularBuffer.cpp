#include <gmock/gmock.h>
#include "TestAssert.h"

#include <CircularBuffer.h>

using namespace testing;

TEST(CircularBuffer, when_an_element_is_pushed_back_then_the_element_can_be_popped_front)
{
	CircularBuffer<int, 2> circularBuffer;
	circularBuffer.push_back(5);

	ASSERT_THAT(*circularBuffer.pop_front(), Eq(5));
}

TEST(CircularBuffer, when_pop_is_executed_on_an_emty_buffer_then_the_optional_is_empty)
{
	CircularBuffer<int, 2> circularBuffer;

	ASSERT_THAT(circularBuffer.pop_front().is_initialized(), Eq(false));
}

TEST(CircularBuffer, buffer_wraps_around_correctly_when_buffer_has_one_element_free)
{
	CircularBuffer<int, 2> circularBuffer;

	for (auto i = 1u; i <= 10; ++i)
	{
		circularBuffer.push_back(i);
		ASSERT_THAT(*circularBuffer.pop_front(), Eq(i));
	}
}

TEST(CircularBuffer, buffer_wraps_around_correctly_when_buffer_is_always_full)
{
	CircularBuffer<int, 2> circularBuffer;

	circularBuffer.push_back(1);
	for (auto i = 2u; i <= 10; ++i)
	{
		circularBuffer.push_back(i);
		ASSERT_THAT(*circularBuffer.pop_front(), Eq(i - 1));
	}
	ASSERT_THAT(*circularBuffer.pop_front(), Eq(10));
}

TEST(CircularBuffer, DiscardPushedElement_when_buffer_is_full_then_elements_are_being_discarded)
{
	CircularBuffer<size_t, 2, CircularBufferFullStrategy::DiscardPushedElement> circularBuffer;

	circularBuffer.push_back(1);
	circularBuffer.push_back(2);
	circularBuffer.push_back(3);
	ASSERT_THAT(*circularBuffer.pop_front(), Eq(1));
	ASSERT_THAT(*circularBuffer.pop_front(), Eq(2));
}

TEST(CircularBuffer, OverwriteOldest_when_buffer_is_full_then_the_oldest_elements_are_being_overwritten)
{
	CircularBuffer<size_t, 2, CircularBufferFullStrategy::OverwriteOldest> circularBuffer;

	circularBuffer.push_back(1);
	circularBuffer.push_back(2);
	circularBuffer.push_back(3);
	ASSERT_THAT(*circularBuffer.pop_front(), Eq(2));
	ASSERT_THAT(*circularBuffer.pop_front(), Eq(3));
}

TEST(CircularBuffer, pop_back)
{
	CircularBuffer<size_t, 2, CircularBufferFullStrategy::OverwriteOldest> circularBuffer;

	circularBuffer.push_back(1);
	circularBuffer.push_back(2);
	ASSERT_THAT(*circularBuffer.pop_back(), Eq(2));
	ASSERT_THAT(*circularBuffer.pop_back(), Eq(1));
	ASSERT_THAT(circularBuffer.pop_back().is_initialized(), Eq(false));
}

TEST(CircularBuffer, pop_back_after_overflow)
{
	CircularBuffer<size_t, 2, CircularBufferFullStrategy::OverwriteOldest> circularBuffer;

	circularBuffer.push_back(1);
	circularBuffer.push_back(2);
	circularBuffer.push_back(3);
	ASSERT_THAT(*circularBuffer.pop_back(), Eq(3));
	ASSERT_THAT(*circularBuffer.pop_back(), Eq(2));
	ASSERT_THAT(circularBuffer.pop_back().is_initialized(), Eq(false));
}

TEST(CircularBuffer, pop_back_after_maxing_out)
{
	CircularBuffer<size_t, 5, CircularBufferFullStrategy::OverwriteOldest> circularBuffer;

	for (auto i = 0; i < 5; ++i)
	{
		circularBuffer.push_back(i);
	}
	ASSERT_THAT(*circularBuffer.pop_back(), Eq(4));
	ASSERT_THAT(*circularBuffer.pop_back(), Eq(3));
	ASSERT_THAT(*circularBuffer.pop_back(), Eq(2));
	ASSERT_THAT(*circularBuffer.pop_back(), Eq(1));
	ASSERT_THAT(*circularBuffer.pop_back(), Eq(0));
	ASSERT_THAT(circularBuffer.pop_back().is_initialized(), Eq(false));
}

TEST(CircularBuffer, circular_buffer_is_iterable)
{
	CircularBuffer<size_t, 5, CircularBufferFullStrategy::OverwriteOldest> circularBuffer;

	circularBuffer.push_back(1);
	circularBuffer.push_back(2);
	circularBuffer.push_back(3);

	auto it = circularBuffer.begin();
	ASSERT_THAT(*it, Eq(1));
	++it;

	ASSERT_THAT(*it, Eq(2));
	++it;

	ASSERT_THAT(*it, Eq(3));
}

TEST(CircularBuffer, circular_buffer_is_iterable_using_foreach)
{
	CircularBuffer<size_t, 5, CircularBufferFullStrategy::OverwriteOldest> circularBuffer;

	circularBuffer.push_back(1);
	circularBuffer.push_back(2);
	circularBuffer.push_back(3);

	size_t i = 0;
	for (auto&& elem : circularBuffer)
	{
		++i;
		ASSERT_THAT(elem, Eq(i));
	}
}
