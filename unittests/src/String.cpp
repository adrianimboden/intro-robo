#include <gmock/gmock.h>
#include "TestAssert.h"
#include "StringStreamer.h"

#include <String.h>

using namespace testing;

TEST(String, when_i_create_a_string_by_null_terminated_string_then_the_size_is_correct)
{
	String<10> str("abc");

	ASSERT_THAT(str.size(), Eq(3));
}

TEST(String, when_i_create_a_string_by_given_length_then_the_size_is_correct)
{
	String<10> str("abcde", 3);

	ASSERT_THAT(str.size(), Eq(3));
}

TEST(String, when_i_compare_a_string_of_same_max_size_to_a_string_then_they_compare_equal)
{
	String<10> strA("abc");
	String<10> strB("abc");

	ASSERT_THAT(strA == strB, Eq(true));
	ASSERT_THAT(strA != strB, Eq(false));
}

TEST(String, when_i_compare_a_string_of_same_max_size_to_a_string_then_they_compare_unequal)
{
	String<10> strA("abc");
	String<10> strB("def");

	ASSERT_THAT(strA == strB, Eq(false));
	ASSERT_THAT(strA != strB, Eq(true));
}

TEST(String, when_i_compare_a_string_of_different_max_size_to_a_string_then_they_compare_equal)
{
	String<5> strA("abc");
	String<10> strB("abc");

	ASSERT_THAT(strA == strB, Eq(true));
}

TEST(String, when_i_compare_a_string_of_different_max_size_to_a_string_then_they_compare_unequal)
{
	String<5> strA("abc");
	String<10> strB("def");

	ASSERT_THAT(strA == strB, Eq(false));
}

TEST(String, when_i_copy_a_string_to_a_same_size_string_then_the_strings_compare_equal)
{
	String<5> strA("abc");
	String<5> strB = strA;

	ASSERT_THAT(strA == strB, Eq(true));
}

TEST(String, when_i_copy_a_string_to_a_different_size_string_then_the_strings_compare_equal)
{
	String<5> strA("abc");
	String<10> strB = strA;

	ASSERT_THAT(strA == strB, Eq(true));
}

TEST(String, when_i_compare_a_string_to_a_char_pointer_then_the_strings_compare_equal)
{
	String<5> strA("abc");

	ASSERT_THAT(strA == "abc", Eq(true));
	ASSERT_THAT("abc" == strA, Eq(true));

	ASSERT_THAT(strA != "abc", Eq(false));
	ASSERT_THAT("abc" != strA, Eq(false));
}

TEST(String, when_i_compare_a_string_to_a_char_pointer_then_the_strings_compare_unequal)
{
	String<5> strA("abc");

	ASSERT_THAT("def" == strA, Eq(false));
	ASSERT_THAT(strA == "def", Eq(false));

	ASSERT_THAT("def" != strA, Eq(true));
	ASSERT_THAT(strA != "def", Eq(true));
}

TEST(String, when_i_access_by_index_then_I_can_change_the_content)
{
	String<5> str("abc");

	str[0] = 'd';

	ASSERT_THAT(str, Eq("dbc"));
}

TEST(String, append_single_char)
{
	String<4> str("abc");

	str.append('d');
	ASSERT_THAT(str, Eq("abcd"));
}

TEST(String, when_append_is_not_possible_then_the_result_indicates_this)
{
	String<3> str("abc");

	ASSERT_THAT(str.append('d'), StringManipulationResult::NotEnoughBufferMemory);
	ASSERT_THAT(str, Eq("abc"));
}

template <typename TString>
void checkEraseResult(TString str, size_t from, size_t to, const char* result)
{
	TString modified = str;
	modified.erase(from, to);
	EXPECT_THAT(modified, Eq(result)) << "\"" << str << "\".erase(" << from << ", " << to << ") => " << result;
}

TEST(String, erase_operation)
{
	checkEraseResult<String<3>>("abc", 1, 3, "a");
	checkEraseResult<String<3>>("abc", 0, 3, "");
	checkEraseResult<String<3>>("abc", 0, 0, "abc");
	checkEraseResult<String<3>>("abc", 0, 1, "bc");
	checkEraseResult<String<3>>("abc", 1, 2, "ac");
	checkEraseResult<String<3>>("abc", 2, 3, "ab");
	checkEraseResult<String<3>>("abc", 0, 100, "");
}

TEST(String, erase_clear_operation)
{
	String<3> str{"abc"};
	str.erase();
	EXPECT_THAT(str, Eq(""));
}
