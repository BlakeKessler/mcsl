#ifndef MCSL_STR_TO_NUM_CPP
#define MCSL_STR_TO_NUM_CPP

#include "str_to_num.hpp"
#include "carry.hpp"
#include "uint_n.hpp"
#include "assert.hpp"

#include <cmath>

//!TODO: add template parameters for radix, only deduce when radix == 0

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
   } else if (str[0] == '+') {
      return str_to_uint(str+1, strlen-1, radix);
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
[[gnu::pure]] constexpr double mcsl::str_to_real(const char* str, const uint strlen, uint radix) {
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

   const uint maxMantDigits = (uint)(LDBL_MANT_DIG / std::log2((float)radix));
   
   //calculate bounds of fields
   const char* const end = str + strlen;
   const char* const mantStart = it;
   const char* mantEnd = end;
   const char* radixPt = nullptr;

	while (it < end) {
      if (!is_digit(*it, radix)) {
         if (*it == '.' && !radixPt) {
            radixPt = it;
         } else {
            mantEnd = it;
            break;
         }
      }

      ++it;
   }

   //bounds
   const char* mantProcessEnd = mantStart + maxMantDigits;
   if (radixPt && radixPt <= mantProcessEnd) {
      ++mantProcessEnd;
   }
   if (mantProcessEnd > mantEnd) {
      mantProcessEnd = mantEnd;
   }

   //mantissa
   long double val;
   {
      ulong tmp = 0;
      const char* mantIt = mantStart;
      if (radixPt < mantProcessEnd) {
         while (mantIt < radixPt) {
            tmp = tmp * radix + digit_to_uint(*mantIt++);
         }
         ++mantIt;
      }
      while (mantIt < mantProcessEnd) {
         tmp = tmp * radix + digit_to_uint(*mantIt++);
      }
      val = tmp;
      val = isNegative ? -val : val;
   }

   //exponent
   sint exp = (radixPt && radixPt < mantProcessEnd) ? radixPt - mantProcessEnd + 1: 0;
   if (it + 2 < end && it[0] == '~' && it[1] == '^') { //Middle-C style
      exp += str_to_sint(it + 2, end, radix);
   }
   else if (it + 1 < end && ((radix < 0xE && (it[0] | CASE_BIT) == 'e') || (it[0] | CASE_BIT) == 'p')) { //C-style
      exp += str_to_sint(it + 1, end, radix);
   }
   val *= std::pow((long double)radix, exp);

   //return
   return (double)val;
}


//!convert string to floating point number
//!legal radices: {0, 10, 16}
//!when radix is 0, base is deduced from contents of string
//!NOTE: slightly imprecise
//!TODO: fix inf, nan, signan
//!TODO: apostrophes?
[[gnu::pure]] constexpr double mcsl::c_float_lit_str_to_real(const char* str, const uint strlen, uint radix) {
   //https://dl.acm.org/doi/pdf/10.1145/93548.93559?download=false
   //https://dl.acm.org/doi/pdf/10.1145/93548.93557?download=false
   //https://www.netlib.org/fp/
   
   assert(str && strlen, __PARSE_NULL_STR_MSG, ErrCode::SEGFAULT);

   //deduce sign, radix, and starting index
   bool isNegative = str[0] == '-';
   const char* it = str + (isNegative || str[0] == '+');

   if (strlen >= (2U + isNegative) && *it == '0') {
      if (radix == 0) {
         if ((*++it | CASE_BIT) == 'x') {
            radix = 16;
            ++it;
         } else {
            radix = 10;
         }
      } else {
         if ((*++it | CASE_BIT) == 'x') {
            assert(radix == 16);
            ++it;
         } else {
            assert(radix == 10);
         }
      }
   } else if (radix == 0) { radix = 10; }

   const uint maxMantDigits = radix == 10 ? 18 : 15;
   debug_assert(sizeof(long double) >= 10);
   
	//calculate bounds of fields
   const char* end = str + strlen;
   const char* mantStart = it;
   const char* mantEnd = end;
   const char* radixPt = nullptr;

	while (it < end) {
      if (!is_digit(*it, radix)) {
         if (*it == '.' && !radixPt) {
            radixPt = it;
         } else {
            mantEnd = it;
            break;
         }
      }

      ++it;
   }

   //calculate value
   long double val = str_to_uint(mantStart, maxMantDigits, radix);
   if (mantStart + maxMantDigits < (radixPt ? radixPt : mantEnd)) {
      val *= std::pow(radix, (radixPt ? radixPt : mantEnd) - mantStart - maxMantDigits);
   }
   if (radixPt && radixPt + 1 < mantStart + maxMantDigits) {
      ulong mantFrac = str_to_uint(radixPt + 1, mantEnd, radix);
      val += mantFrac * std::pow(radix, radixPt + 1 - mantEnd);
   }
   sint exp = 0;
   if (it + 1 < end) {
      if ((radix < 0xE && (*it | CASE_BIT) == 'e') || (*it | CASE_BIT) == 'p') {
         exp += str_to_sint(it + 1, end, 10);
      }
   }
   if (radix == 10) {
      val *= std::pow(10.0, exp);
   } else {
      val *= std::pow(2, exp);
   }

   //return
	return isNegative ? -val : val;
}

#endif //MCSL_STR_TO_NUM_CPP