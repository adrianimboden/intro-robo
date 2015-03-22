#ifndef TESTASSERT_H
#define TESTASSERT_H


inline void testAssert(bool ok)
{
	if (!ok)
	{
		FAIL();
	}
}

#define ASSERT(...) testAssert(__VA_ARGS__);

#endif // TESTASSERT_H
