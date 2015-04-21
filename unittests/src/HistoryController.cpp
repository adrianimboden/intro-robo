#include <gmock/gmock.h>
#include "TestAssert.h"

#include <HistoryController.h>

using namespace testing;

TEST(HistoryController, when_scrolling_up_and_down_then_the_correct_entries_show_up)
{
	HistoryController<uint32_t, 2> historyController;

	historyController.newLine(1);
	historyController.newLine(2);

	ASSERT_THAT(historyController.up(0), Eq(2));
	ASSERT_THAT(historyController.up(0), Eq(1));

	ASSERT_THAT(historyController.down(0), Eq(2));
}

TEST(HistoryController, when_scrolling_down_after_up_the_begunLine_is_not_lost)
{
	HistoryController<uint32_t, 2> historyController;

	historyController.newLine(1);

	ASSERT_THAT(historyController.up(55), Eq(1));
	ASSERT_THAT(historyController.down(1), Eq(55));
}

TEST(HistoryController, when_the_buffer_is_full_then_the_newest_entries_remain)
{
	HistoryController<uint32_t, 2> historyController;

	historyController.newLine(1);
	historyController.newLine(2);
	historyController.newLine(3);
	historyController.newLine(4);

	ASSERT_THAT(historyController.up(0), Eq(4));
	ASSERT_THAT(historyController.up(4), Eq(3));
	ASSERT_THAT(historyController.up(3), Eq(3));
}

TEST(HistoryController, complexInteraction)
{
	HistoryController<uint32_t, 4> historyController;

	historyController.newLine(1);
	ASSERT_THAT(historyController.up(10), Eq(1));
	ASSERT_THAT(historyController.down(1), Eq(10));
	historyController.newLine(10);
	ASSERT_THAT(historyController.up(20), Eq(10));
	ASSERT_THAT(historyController.down(10), Eq(20));
	historyController.newLine(20);
	ASSERT_THAT(historyController.up(30), Eq(20));
	ASSERT_THAT(historyController.up(20), Eq(10));
	ASSERT_THAT(historyController.up(10), Eq(1));
	ASSERT_THAT(historyController.up(1), Eq(1));
	ASSERT_THAT(historyController.up(1), Eq(1));
	ASSERT_THAT(historyController.down(1), Eq(10));
	ASSERT_THAT(historyController.down(10), Eq(20));
	ASSERT_THAT(historyController.down(20), Eq(30));
	ASSERT_THAT(historyController.down(30), Eq(30));
}

TEST(HistoryController, when_going_up_and_down_twice_without_newline_then_the_behaviour_is_correct)
{
	HistoryController<uint32_t, 4> historyController;

	historyController.newLine(1);
	ASSERT_THAT(historyController.up(10), Eq(1));
	ASSERT_THAT(historyController.down(1), Eq(10));

	ASSERT_THAT(historyController.up(10), Eq(1));
	ASSERT_THAT(historyController.down(1), Eq(10));
}

TEST(HistoryController, when_going_up_and_down_two_times_twice_without_newline_then_the_behaviour_is_correct)
{
	HistoryController<uint32_t, 4> historyController;

	historyController.newLine(1);
	historyController.newLine(2);
	ASSERT_THAT(historyController.up(10), Eq(2));
	ASSERT_THAT(historyController.up(1), Eq(1));
	ASSERT_THAT(historyController.down(1), Eq(2));
	ASSERT_THAT(historyController.down(1), Eq(10));

	ASSERT_THAT(historyController.up(10), Eq(2));
	ASSERT_THAT(historyController.up(1), Eq(1));
	ASSERT_THAT(historyController.down(1), Eq(2));
	ASSERT_THAT(historyController.down(1), Eq(10));
}

TEST(HistoryController, when_calling_a_command_from_history_then_the_history_will_be_preserved)
{
	HistoryController<uint32_t, 4> historyController;

	historyController.newLine(1);
	historyController.newLine(2);
	ASSERT_THAT(historyController.up(0), Eq(2));
	ASSERT_THAT(historyController.up(2), Eq(1));

	historyController.newLine(1);
	ASSERT_THAT(historyController.up(0), Eq(1));
	ASSERT_THAT(historyController.up(1), Eq(2));
	ASSERT_THAT(historyController.up(2), Eq(1));
}

TEST(HistoryController, when_calling_a_command_from_history_and_the_queue_overflows_then_the_history_will_be_preserved)
{
	HistoryController<uint32_t, 2> historyController;

	historyController.newLine(1);
	historyController.newLine(2);
	ASSERT_THAT(historyController.up(0), Eq(2));
	ASSERT_THAT(historyController.up(2), Eq(1));

	historyController.newLine(1);
	ASSERT_THAT(historyController.up(0), Eq(1));
	ASSERT_THAT(historyController.up(1), Eq(2));

	historyController.newLine(1);
	ASSERT_THAT(historyController.up(0), Eq(1));
	ASSERT_THAT(historyController.up(1), Eq(1));
}
