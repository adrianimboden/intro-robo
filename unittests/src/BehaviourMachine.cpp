#include <gmock/gmock.h>
#include "TestAssert.h"

#include <BehaviourMachine.h>
#include <tuple>

using namespace testing;

class TestBehaviour
{
public:
	TestBehaviour(uint32_t& stepCounter, uint32_t& suppressCounter, bool& takeControl)
		: stepCounter(stepCounter)
		, suppressCounter(suppressCounter)
		, takeControl(takeControl)
	{
	}

	bool wantsToTakeControl() const
	{
		return takeControl;
	}

	void step(bool suppress)
	{
		if (!suppress)
		{
			++stepCounter;
		}
		else
		{
			++suppressCounter;
		}
	}

private:
	uint32_t& stepCounter;
	uint32_t& suppressCounter;
	bool& takeControl;
};

TEST(BehaviourMachineTest, when_regaining_priority_then_the_step_function_will_be_called)
{
	uint32_t dummy;

	uint32_t lowPrioCounter = 0;
	bool lowPrioControl = false;
	uint32_t highPrioCounter = 0;
	bool highPrioControl = false;

	auto arbitrator = makeArbitrator(std::make_tuple(
		TestBehaviour(lowPrioCounter, dummy, lowPrioControl),
		TestBehaviour(highPrioCounter, dummy, highPrioControl)
	));

	arbitrator.step();
	ASSERT_THAT(lowPrioCounter, Eq(0));
	ASSERT_THAT(highPrioCounter, Eq(0));

	lowPrioControl = true;
	arbitrator.step();
	ASSERT_THAT(lowPrioCounter, Eq(1));
	ASSERT_THAT(highPrioCounter, Eq(0));

	highPrioControl = true;
	arbitrator.step();
	ASSERT_THAT(lowPrioCounter, Eq(1));
	ASSERT_THAT(highPrioCounter, Eq(1));

	highPrioControl = false;
	arbitrator.step();
	ASSERT_THAT(lowPrioCounter, Eq(2));
	ASSERT_THAT(highPrioCounter, Eq(1));
}

TEST(BehaviourMachineTest, when_loosing_prio_then_step_without_suppress_will_not_be_called_anymore)
{
	uint32_t dummy;

	uint32_t lowPrioCounter = 0;
	bool lowPrioControl = false;

	auto arbitrator = makeArbitrator(std::make_tuple(
		TestBehaviour(lowPrioCounter, dummy, lowPrioControl)
	));

	arbitrator.step();
	ASSERT_THAT(lowPrioCounter, Eq(0));

	lowPrioControl = true;
	arbitrator.step();
	ASSERT_THAT(lowPrioCounter, Eq(1));

	lowPrioControl = false;
	arbitrator.step();
	ASSERT_THAT(lowPrioCounter, Eq(1));
}
