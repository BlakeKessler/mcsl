#ifndef MCSL_STR_TO_NUM_CPP
#define MCSL_STR_TO_NUM_CPP

#include "str_to_num.hpp"
#include "carry.hpp"
#include "uint_n.hpp"
#include "assert.hpp"

#include <cmath>

//!convert string to unsigned integer
//!legal radices: {0, 2, ... , 36}
//!when radix is 0, base is deduced from contents of string
[[gnu::pure]] constexpr ulong mcsl::str_to_uint(const char* str, const uint strlen, uint radix) {
   assert(str && strlen, __PARSE_NULL_STR_MSG, ErrCode::SEGFAULT);

   uint i = 0;

   //deduce radix and starting index
   if (strlen >= 2 && str[0] == '0') {
      if (radix == 0) {
         switch(str[1]) {
            case 'b': case 'B': radix = 2;  i = 2; break;
            case 'o': case 'O': radix = 8;  i = 2; break;
            case 'd': case 'D': radix = 10; i = 2; break;
            case 'x': case 'X': radix = 16; i = 2; break;
            
            default : radix = 8; i = 1; break;
         }
      } else {
         ++i;
         switch(radix) {
            case  2: i += ((str[1] | CASE_BIT) == 'b'); break;
            case  8: i += ((str[1] | CASE_BIT) == 'o'); break;
            case 10: i += ((str[1] | CASE_BIT) == 'd'); break;
            case 16: i += ((str[1] | CASE_BIT) == 'x'); break;
         }
      }
   } else if (radix == 0) { radix = 10; }

   //check radix
   if (radix < 2 || radix > 36) {
      __throw(ErrCode::STRTOINT, "radix for mcsl::str_to_uint() must be between 2 and 36 (not %u)", radix);
   }

   //parse integer
   ulong val = 0;
   for (; i < strlen; ++i) {
      //parse digit
      uint8 digit = (uint8)digit_to_uint(str[i]);
      //check digit - NOTE: consolidates check for validity using integer underflow
      if (digit >= radix) { [[unlikely]] break; }

      //push digit - NOTE: manually inlines mcsl::MUL (multiplication with carry)
      uoverlong tmp = (uoverlong)val * (uoverlong)radix;
      val = (ulong)tmp;
      if ((ulong)(tmp >> (sizeof(ulong) * 8))) { [[unlikely]] //check for overflow
         __throw(ErrCode::STRTOINT, "unsigned integer overflow at index %u", i);
      }

      val += digit;
   }
   return val;
}

//!convert string to signed integer
//!legal radices: {0, 2, ... , 36}
//!when radix is 0, base is deduced from contents of string
[[gnu::pure]] constexpr slong mcsl::str_to_sint(const char* str, const uint strlen, uint radix) {
   assert(str && strlen, __PARSE_NULL_STR_MSG, ErrCode::SEGFAULT);

   if (str[0] == '-') {
      return -str_to_uint(str+1, strlen-1, radix);
   } else {
      return str_to_uint(str, strlen, radix);
   }
}

//!converts a single character into a base-36 digit
[[gnu::const, gnu::always_inline]] constexpr sint8 mcsl::digit_to_uint(const char ch) {
   switch (ch) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
         return ch & 0x0f;

      case 'a': case 'A':
      case 'b': case 'B':
      case 'c': case 'C':
      case 'd': case 'D':
      case 'e': case 'E':
      case 'f': case 'F':
      case 'g': case 'G':
      case 'h': case 'H':
      case 'i': case 'I':
      case 'j': case 'J':
      case 'k': case 'K':
      case 'l': case 'L':
      case 'm': case 'M':
      case 'n': case 'N':
      case 'o': case 'O':
      case 'p': case 'P':
      case 'q': case 'Q':
      case 'r': case 'R':
      case 's': case 'S':
      case 't': case 'T':
      case 'u': case 'U':
      case 'v': case 'V':
      case 'w': case 'W':
      case 'x': case 'X':
      case 'y': case 'Y':
      case 'z': case 'Z':
         return (ch & 0x1f) + 9;
      
      default: [[unlikely]] return -1;
   }
   // __throw(ErrCode::STRTOINT, "%c (ASCII: %u) is not a valid base-36 digit", ch, (uint)ch);
}

//!returns whether or not an entire string is digits for the specifed base
[[gnu::pure]] constexpr bool mcsl::is_uint(const char* str, const uint strlen, const uint radix) {
   assert(str && strlen, __PARSE_NULL_STR_MSG, ErrCode::SEGFAULT);

   for (uint i = 0; i < strlen; ++i) {
      if (!is_digit(str[i], radix)) {
         return false;
      }
   }
   return true;
}

//!convert string to floating point number
//!legal radices: {0, 2, 8, 10, 16}
//!when radix is 0, base is deduced from contents of string
//!NOTE: might have minor imprecision when scientific notation is used
//!implementation heavily based on a [LibCL implementation](https://github.com/ochafik/LibCL/blob/master/src/main/resources/LibCL/strtod.c)
[[gnu::pure]] constexpr double mcsl::str_to_real(const char* str, const uint strlen, uint radix) {
   //https://dl.acm.org/doi/pdf/10.1145/93548.93559?download=false
   //https://dl.acm.org/doi/pdf/10.1145/93548.93557?download=false
   //https://www.netlib.org/fp/
   
   assert(str && strlen, __PARSE_NULL_STR_MSG, ErrCode::SEGFAULT);

   //deduce sign, radix, and starting index
   bool isNegative = str[0] == '-';
   const char* it = str + isNegative;

   if (strlen >= (2U + isNegative) && *it == '0') {
      if (radix == 0) {
         switch(*++it) {
            case 'b': case 'B': radix = 2;  ++it; break;
            case 'o': case 'O': radix = 8;  ++it; break;
            case 'd': case 'D': radix = 10; ++it; break;
            case 'x': case 'X': radix = 16; ++it; break;
            
            default : radix = 10; break;
         }
      } else {
         ++it;
         switch(radix) {
            case  2: it += ((*it | CASE_BIT) == 'b'); break;
            case  8: it += ((*it | CASE_BIT) == 'o'); break;
            case 10: it += ((*it | CASE_BIT) == 'd'); break;
            case 16: it += ((*it | CASE_BIT) == 'x'); break;
         }
      }
   } else if (radix == 0) { radix = 10; }

   const uint maxMantDigits = 18; //!TODO: calculate this based on the value of `radix`
   const int _strtod_maxExponent_ = 511; //!TODO: calculate this based on the value of `radix`
   double powers[9];
   powers[0] = radix;
   for (uint i = 0; i < 8; ++i) {
      powers[i+1] = powers[i] * powers[i];
   }
   
   bool expSign = false;
	int fracExp = 0;		/* Exponent that derives from the fractional
	 * part.  Under normal circumstatnces, it is
	 * the negative of the number of digits in F.
	 * However, if I is very long, the last digits
	 * of I get dropped (otherwise a long I with a
	 * large negative exponent could cause an
	 * unnecessary overflow on I alone).  In this
	 * case, fracExp is incremented one for each
	 * dropped digit. */
	uint mantSize = 0;		//Number of digits in mantissa.
	sint digBeforeRadixPt = -1;			//Number of mantissa digits BEFORE decimal point

	/*
	 * Count the number of digits in the mantissa (including the decimal
	 * point), and also locate the decimal point.
	 */

	while (true) {
		const char c = *it;
		if (!is_digit(c, radix)) {
			if ((c != '.') || (digBeforeRadixPt >= 0)) {
				break;
			}
			digBeforeRadixPt = mantSize;
		}
		++it;
      ++mantSize;
	}

	/*
	 * Now suck up the digits in the mantissa.  Use two integers to
	 * collect 9 digits each (this is faster than using floating-point).
	 * If the mantissa has more than 18 digits, ignore the extras, since
	 * they can't affect the value anyway.
	 */

	const char* pExp = it;
	it -= mantSize;
	if (digBeforeRadixPt < 0) {
		digBeforeRadixPt = mantSize;
	} else {
		--mantSize;			/* One of the digits was the point. */
	}
   if (mantSize == 0) {
      return isNegative ? -0.0 : 0.0;
	}

	mantSize = (mantSize > maxMantDigits) ? maxMantDigits : mantSize;
   fracExp = digBeforeRadixPt - mantSize;
   
   ulong tmp = 0;
   for ( ; mantSize > 0; --mantSize) {
      if (*it == '.') {
         ++it;
      }
      tmp = radix*tmp + digit_to_uint(*it);
      ++it;
   }
   double fraction = tmp;

	/*
	 * Skim off the exponent.
	 */

	it = pExp;
   sint exp = 0;
	if ((radix < 0xE && *it == 'e') || *it == 'p') {
		++it;
		if (*it == '-') {
			expSign = true;
			++it;
		} else {
			if (*it == '+') {
				++it;
			}
			expSign = false;
		}
		while (is_digit(*it, radix)) {
			exp = exp*radix + digit_to_uint(*it);
			++it;
		}
	}
	if (expSign) {
		exp = fracExp - exp;
	} else {
		exp = fracExp + exp;
	}

	/*
	 * Generate a floating-point number that represents the exponent.
	 * Do this by processing the exponent one bit at a time to combine
	 * many powers of 2 of 10. Then combine the exponent with the
	 * fraction.
	 */

	if (exp < 0) {
		expSign = true;
		exp = -exp;
	} else {
		expSign = false;
	}
	exp = (exp > _strtod_maxExponent_) ? _strtod_maxExponent_ : exp;

	double dblExp = 1.0;
   {
      double d = radix;
      while (exp) {
         if (exp & 1) {
            dblExp *= d;
         }
         exp >>= 1;
         d *= d;
      }
   }

	if (expSign) {
		fraction /= dblExp;
	} else {
		fraction *= dblExp;
	}

	return isNegative ? -fraction : fraction;
}

#endif //MCSL_STR_TO_NUM_CPP