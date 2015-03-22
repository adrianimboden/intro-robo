#include <gmock/gmock.h>
#include "TestAssert.h"

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
