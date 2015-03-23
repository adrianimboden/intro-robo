#include <gmock/gmock.h>
#include "TestAssert.h"

#include <Optional.h>

#include <memory>

using namespace testing;

TEST(Optional, when_default_initialized_then_value_is_not_available)
{
	optional<int> opt;

	ASSERT_THAT(opt.is_initialized(), Eq(false));
	ASSERT_THAT(static_cast<bool>(opt), Eq(false));
}

TEST(Optional, when_initialized_with_value_then_is_available)
{
	optional<int> opt{5};

	ASSERT_THAT(opt.is_initialized(), Eq(true));
	ASSERT_THAT(static_cast<bool>(opt), Eq(true));
}

TEST(Optional, when_initialized_with_value_then_value_can_be_retrieved)
{
	optional<int> opt{5};

	ASSERT_THAT(*opt, Eq(5));
}

TEST(Optional, when_initialized_with_value_then_value_can_be_changed)
{
	optional<int> opt{5};

	*opt = 6;

	ASSERT_THAT(*opt, Eq(6));
}

TEST(Optional, when_initialized_with_value_then_optional_can_be_reset_again)
{
	optional<int> opt{5};

	opt.reset();

	ASSERT_THAT(static_cast<bool>(opt), Eq(false));
}

class ConstructorAndDestructorCounter
{
public:
	struct Counters
	{
		size_t defaultConstruct = 0;
		size_t copyConstruct = 0;
		size_t moveConstruct = 0;
		size_t destruct = 0;
	};

	ConstructorAndDestructorCounter(Counters* pCounters)
		: pCounters(pCounters)
	{
		++pCounters->defaultConstruct;
	}

	ConstructorAndDestructorCounter(const ConstructorAndDestructorCounter& other)
		: pCounters(other.pCounters)
	{
		++pCounters->copyConstruct;
	}

	ConstructorAndDestructorCounter(ConstructorAndDestructorCounter&& other)
		: pCounters(other.pCounters)
	{
		++pCounters->moveConstruct;
	}

	~ConstructorAndDestructorCounter()
	{
		++pCounters->destruct;
	}

private:
	Counters* pCounters;
};

TEST(Optional, when_inplace_constructing_constructor_and_destructor_will_be_called_properly)
{
	ConstructorAndDestructorCounter::Counters counters;
	{
		optional<ConstructorAndDestructorCounter> opt{&counters};
		ASSERT_THAT(counters.defaultConstruct, Eq(1));
	}
	ASSERT_THAT(counters.destruct, Eq(1));
}

TEST(Optional, when_resetting_then_the_destructor_will_be_called_exactly_once)
{
	ConstructorAndDestructorCounter::Counters counters;
	{
		optional<ConstructorAndDestructorCounter> opt{&counters};
		ASSERT_THAT(counters.defaultConstruct, Eq(1));
		opt.reset();
		ASSERT_THAT(counters.destruct, Eq(1));
	}
	ASSERT_THAT(counters.destruct, Eq(1));
}

TEST(Optional, when_copying_optional_around_then_constructor_or_destructor_calls_are_made_appropriately)
{
	ConstructorAndDestructorCounter::Counters counters;
	{
		optional<ConstructorAndDestructorCounter> opt{&counters};

		{
			optional<ConstructorAndDestructorCounter> moveTarget = opt;
		}
		ASSERT_THAT(counters.destruct, Eq(1));
	}

	ASSERT_THAT(counters.destruct, Eq(2));
	ASSERT_THAT(counters.defaultConstruct, Eq(1));
	ASSERT_THAT(counters.copyConstruct, Eq(1));
}

TEST(Optional, when_moving_optional_around_then_no_constructor_or_destructor_call_is_necessary)
{
	ConstructorAndDestructorCounter::Counters counters;
	{
		optional<ConstructorAndDestructorCounter> opt{&counters};

		{
			optional<ConstructorAndDestructorCounter> moveTarget = std::move(opt);
		}
		ASSERT_THAT(counters.destruct, Eq(1));
	}

	ASSERT_THAT(counters.destruct, Eq(2));
	ASSERT_THAT(counters.defaultConstruct, Eq(1));
	ASSERT_THAT(counters.moveConstruct, Eq(1));
}
