#pragma once

#include <FixedSizeString.h>
#include <CharBit.h>
#include <Optional.h>

#include <type_traits>

namespace detail
{

constexpr size_t calcMaxStringSizeByUnsignedNumberType(size_t bits)
{
	return
	(bits == 8) ? 3 :  //256
	(bits == 16) ? 5 : //65536
	(bits == 32) ? 10 : //4294967296
	(bits == 64) ? 20 : //18446744073709551616
	0;
}

constexpr size_t calcMaxStringSizeByNumberType(size_t bits, bool isSigned)
{
	return isSigned ? calcMaxStringSizeByUnsignedNumberType(bits) + 1 : calcMaxStringSizeByUnsignedNumberType(bits);
}

template <typename ContFn>
auto preventOverflowIfNecessary(int8_t val, ContFn fnCont) -> String<calcMaxStringSizeByNumberType(sizeof(int8_t)*CHAR_BIT, true)>
{
	if (val == (int8_t)(0x80))
	{ /* special case 0x80/-128: prevent overflow in fnCont. */
		return "-128";
	}
	else
	{
		return fnCont();
	}
}

template <typename ContFn>
auto preventOverflowIfNecessary(int16_t val, ContFn fnCont) -> String<calcMaxStringSizeByNumberType(sizeof(int16_t)*CHAR_BIT, true)>
{
	if (val == (int16_t)(0x8000))
	{ /* special case 0x8000/-32768: prevent overflow in fnCont. */
		return "-32768";
	}
	else
	{
		return fnCont();
	}
}

template <typename ContFn>
auto preventOverflowIfNecessary(int32_t val, ContFn fnCont) -> String<calcMaxStringSizeByNumberType(sizeof(int32_t)*CHAR_BIT, true)>
{
	if (val == (int32_t)(0x80000000))
	{  /* special case 0x80000000/-2147483648: prevent overflow in fnCont. */
		return "-2147483648";
	}
	else
	{
		return fnCont();
	}
}

template <typename ContFn>
auto preventOverflowIfNecessary(int64_t val, ContFn fnCont) -> String<calcMaxStringSizeByNumberType(sizeof(int64_t)*CHAR_BIT, true)>
{
	if (val == (int64_t)(0x8000000000000000))
	{  /* special case 0x80000000/-9223372036854775808: prevent overflow in fnCont. */
		return "-9223372036854775808";
	}
	else
	{
		return fnCont();
	}
}

template <typename ContFn>
auto preventOverflowIfNecessary(uint8_t /*val*/, ContFn fnCont) -> String<calcMaxStringSizeByNumberType(sizeof(uint8_t)*CHAR_BIT, true)>
{
	return fnCont();
}

template <typename ContFn>
auto preventOverflowIfNecessary(uint16_t /*val*/, ContFn fnCont) -> String<calcMaxStringSizeByNumberType(sizeof(uint16_t)*CHAR_BIT, true)>
{
	return fnCont();
}

template <typename ContFn>
auto preventOverflowIfNecessary(uint32_t /*val*/, ContFn fnCont) -> String<calcMaxStringSizeByNumberType(sizeof(uint32_t)*CHAR_BIT, true)>
{
	return fnCont();
}

template <typename ContFn>
auto preventOverflowIfNecessary(uint64_t /*val*/, ContFn fnCont) -> String<calcMaxStringSizeByNumberType(sizeof(uint64_t)*CHAR_BIT, true)>
{
	return fnCont();
}

template <bool IsSignedType, typename TVal>
auto numberToStringImpl(TVal val) -> String<calcMaxStringSizeByNumberType(sizeof(TVal)*CHAR_BIT, true)>
{
	std::array<char, calcMaxStringSizeByNumberType(sizeof(TVal)*CHAR_BIT, true) + 1> buffer;
	size_t dstSize = buffer.size();

	char *ptr = buffer.begin();
	unsigned char i=0, j;
	unsigned char tmp;
	unsigned char sign = IsSignedType && (unsigned char)(val < 0);

	return preventOverflowIfNecessary(val, [&]
	{
		dstSize--; /* for zero uint8_t */
		if (sign)
		{
			val = -val;
		}
		if (val == 0 && dstSize > 0)
		{
			ptr[i++] = '0';
			dstSize--;
		}
		while (val > 0 && dstSize > 0)
		{
			ptr[i++] = (unsigned char)((val % 10) + '0');
			dstSize--;
			val /= 10;
		}
		if (sign && dstSize > 0)
		{
			ptr[i++] = '-';
			dstSize--;
		}
		for(j=0; j<(i/2); j++)
		{ /* swap buffer */
			tmp = ptr[j];
			ptr[j] = ptr[(i-j)-1];
			ptr[(i-j)-1] = tmp;
		}
		ptr[i] = '\0';

		return ptr;
	});
}

template <typename TVal>
optional<TVal> stringToNumber(const unsigned char* pStr)
{
	/* scans a decimal number, and stops at any non-number. Number can have any preceding zeros or spaces. */
	constexpr auto MaxNoOfDigits = calcMaxStringSizeByNumberType(sizeof(TVal) * CHAR_BIT, false) + 1;
	uint8_t noOfDigits = MaxNoOfDigits;
	const unsigned char *p = pStr;

	while(*p==' ')
	{ /* skip leading spaces */
		p++;
	}

	bool isNeg = false;
	if (std::is_signed<TVal>::value)
	{
		if (*p=='-')
		{
			isNeg = true;
			p++; /* skip minus */
		}
	}

	TVal val = 0;
	while(*p>='0' && *p<='9' && noOfDigits > 0)
	{
		val = (TVal)((val)*10 + *p-'0');
		noOfDigits--;
		p++;
	} /* while */
	if (noOfDigits==0)
	{
		return {};
	}
	if (noOfDigits == MaxNoOfDigits)
	{ /* no digits at all? */
		return {};
	}

	if (isNeg)
	{
		return -val;
	} else
	{
		return val;
	}
}


}


template <typename TVal>
auto numberToString(TVal val) -> decltype(detail::numberToStringImpl<std::is_signed<TVal>::value, TVal>(val))
{
	return detail::numberToStringImpl<std::is_signed<TVal>::value, TVal>(val);
}

template <typename TVal>
optional<TVal> stringToNumber(const unsigned char* pStr)
{
	return detail::stringToNumber<TVal>(pStr);
}

template <typename TVal>
optional<TVal> stringToNumber(const char* pStr)
{
	return detail::stringToNumber<TVal>(reinterpret_cast<const unsigned char*>(pStr));
}

template <typename TVal, size_t MaxSize>
optional<TVal> stringToNumber(const String<MaxSize>& str)
{
	return stringToNumber<TVal>(str.begin());
}
