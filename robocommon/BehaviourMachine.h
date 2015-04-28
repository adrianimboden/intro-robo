#ifndef BEHAVIOURMACHINE_H
#define BEHAVIOURMACHINE_H

#include <utility>
#include <tuple>

#include <Optional.h>

class Behaviour
{
public:
	bool wantsToTakeControl() const;
	void step(bool suppress);
};

/**
 * index of behaviour == priority of behaviour
 * higher the index => higher priority
 */
template <typename... Behaviours>
class Arbitrator
{
public:
	explicit Arbitrator(std::tuple<Behaviours...> behaviours)
		: behaviours(std::move(behaviours))
	{
	}

	//sentinel
	template<std::size_t I = 0, typename Fn, typename... Tp>
	static typename std::enable_if<(I == sizeof...(Tp)), void>::type findBehaviourThatWantsToTakeContro(Fn /*fn*/, std::tuple<Tp...>& /*t*/) {}

	template<std::size_t I = 0, typename Fn, typename... Tp>
	static typename std::enable_if<(I < sizeof...(Tp)), void>::type findBehaviourThatWantsToTakeContro(Fn fn, std::tuple<Tp...>& t)
	{
		auto&& behaviour = std::get<I>(t);

		if (behaviour.wantsToTakeControl())
		{
			fn(I);
		}

		findBehaviourThatWantsToTakeContro<I + 1, Fn, Tp...>(fn, t);
	}


	//sentinel
	template<std::size_t I = 0, typename... Tp>
	static typename std::enable_if<(I == sizeof...(Tp)), void>::type stepBehaviour(uint8_t /*index*/, bool /*suppress*/, std::tuple<Tp...>& /*t*/) {}

	template<std::size_t I = 0, typename... Tp>
	static typename std::enable_if<(I < sizeof...(Tp)), void>::type stepBehaviour(uint8_t index, bool suppress, std::tuple<Tp...>& t)
	{
		if (index == I)
		{
			auto&& behaviour = std::get<I>(t);

			behaviour.step(suppress);
			return;
		}

		stepBehaviour<I + 1, Tp...>(index, suppress, t);
	}

	void step()
	{
		uint8_t newIndex = std::numeric_limits<uint8_t>::max();
		findBehaviourThatWantsToTakeContro([&](uint8_t index)
		{
			newIndex = index;
		}, behaviours);

		if (currentBehaviour == newIndex)
		{
			stepBehaviour(currentBehaviour, false, behaviours);
		}
		else if (currentBehaviour != newIndex)
		{
			stepBehaviour(currentBehaviour, true, behaviours);
			currentBehaviour = newIndex;
			stepBehaviour(currentBehaviour, false, behaviours);
		}
	}

private:
	uint8_t currentBehaviour = 0;
	std::tuple<Behaviours...> behaviours;
};

template <typename... Behaviours>
Arbitrator<Behaviours...> makeArbitrator(std::tuple<Behaviours...>&& behaviours)
{
	return Arbitrator<Behaviours...>(std::forward<std::tuple<Behaviours...>>(behaviours));
}

#endif // BEHAVIOURMACHINE_H
