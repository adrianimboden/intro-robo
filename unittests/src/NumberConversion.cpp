#include <gmock/gmock.h>
#include "TestAssert.h"

#include <NumberConversion.h>

using namespace testing;

template <size_t MaxSize>
std::ostream& operator<<(std::ostream& strm, const String<MaxSize>& str)
{
	return strm << str.begin();
}

TEST(NumberConversion, _uint8_t)
{
	ASSERT_THAT(numberToString<uint8_t>(0), Eq("0"));
	ASSERT_THAT(numberToString<uint8_t>(128), Eq("128"));
	ASSERT_THAT(numberToString<uint8_t>(255), Eq("255"));
}

TEST(NumberConversion, _int8_t)
{
	ASSERT_THAT(numberToString<int8_t>(0), Eq("0"));
	ASSERT_THAT(numberToString<int8_t>(127), Eq("127"));
	ASSERT_THAT(numberToString<int8_t>(-127), Eq("-127"));
	ASSERT_THAT(numberToString<int8_t>(-128), Eq("-128"));
}

TEST(NumberConversion, _uint16_t)
{
	ASSERT_THAT(numberToString<uint16_t>(0), Eq("0"));
	ASSERT_THAT(numberToString<uint16_t>(32768), Eq("32768"));
	ASSERT_THAT(numberToString<uint16_t>(65535), Eq("65535"));
}

TEST(NumberConversion, _int16_t)
{
	ASSERT_THAT(numberToString<int16_t>(0), Eq("0"));
	ASSERT_THAT(numberToString<int16_t>(32767), Eq("32767"));
	ASSERT_THAT(numberToString<int16_t>(-32767), Eq("-32767"));
	ASSERT_THAT(numberToString<int16_t>(-32768), Eq("-32768"));
}

TEST(NumberConversion, _uint32_t)
{
	ASSERT_THAT(numberToString<uint32_t>(0), Eq("0"));
	ASSERT_THAT(numberToString<uint32_t>(2147483648), Eq("2147483648"));
	ASSERT_THAT(numberToString<uint32_t>(4294967295), Eq("4294967295"));
}

TEST(NumberConversion, _int32_t)
{
	ASSERT_THAT(numberToString<int32_t>(0), Eq("0"));
	ASSERT_THAT(numberToString<int32_t>(2147483647), Eq("2147483647"));
	ASSERT_THAT(numberToString<int32_t>(-2147483647), Eq("-2147483647"));
	ASSERT_THAT(numberToString<int32_t>(-2147483648), Eq("-2147483648"));
}

TEST(NumberConversion, _uint64_t)
{
	ASSERT_THAT(numberToString<uint64_t>(0), Eq("0"));
	ASSERT_THAT(numberToString<uint64_t>(9223372036854775807u), Eq("9223372036854775807"));
	ASSERT_THAT(numberToString<uint64_t>(18446744073709551615u), Eq("18446744073709551615"));
}

TEST(NumberConversion, _int64_t)
{
	ASSERT_THAT(numberToString<int64_t>(0), Eq("0"));
	ASSERT_THAT(numberToString<int64_t>(9223372036854775807), Eq("9223372036854775807"));
	ASSERT_THAT(numberToString<int64_t>(-9223372036854775806), Eq("-9223372036854775806"));
	ASSERT_THAT(numberToString<int64_t>(-9223372036854775807), Eq("-9223372036854775807"));
}
