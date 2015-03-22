#pragma once

#ifndef __cplusplus
#error sorry, this header is c++ only
#endif

void AssertImpl();

#ifndef ASSERT
#define ASSERT(ok) do { if (!(ok)) AssertImpl(); } while (false)
#endif
