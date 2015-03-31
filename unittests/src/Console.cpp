#include <gmock/gmock.h>
#include "TestAssert.h"

#include <Console.h>
#include <IOStream.h>

using namespace testing;

TEST(Console, when_i_receive_data_it_will_be_forwarded_to_input_strategy)
{
	class StoreInStringstreamInputStrategy
	{
	public:
		StoreInStringstreamInputStrategy(std::stringstream* pStrm)
			: pStrm(pStrm)
		{
		}

		void rxChar(IOStream&, char c)
		{
			*pStrm << c;
		}

	private:
		std::stringstream* pStrm;
	};

	std::stringstream inStrm;

	auto console = makeConsole(makeFnIoStream([](char){}, []()->optional<char>{ return 'a'; }), StoreInStringstreamInputStrategy{&inStrm});

	console.pollInput();
	ASSERT_THAT(inStrm.str(), StrEq("a"));
}
