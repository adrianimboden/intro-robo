#include "Assert.h"

#include "CriticalSection.h"

void AssertImpl()
{
	DisableInterrupts disableInterrupts;
	while (true)
		;
}
