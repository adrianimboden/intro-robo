#ifndef TESTASSERT_H
#define TESTASSERT_H

#include <exception>

class AssertionFailedException : public std::exception
{
public:
	AssertionFailedException() noexcept
	{
	}

	~AssertionFailedException() noexcept
	{
	}

	const char* what() const noexcept override
	{
		return "assertion failed";
	}
};

inline void testAssert(bool ok)
{
	if (!ok)
	{
		throw AssertionFailedException();
	}
}

#define ASSERT(...) testAssert(__VA_ARGS__);

#endif // TESTASSERT_H
