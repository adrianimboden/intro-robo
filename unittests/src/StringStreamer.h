#pragma once

#include <String.h>
#include <iostream>

template <size_t MaxSize>
std::ostream& operator<<(std::ostream& strm, const String<MaxSize>& str)
{
	return strm << str.begin();
}
