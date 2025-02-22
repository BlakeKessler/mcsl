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
//!NOTE: slightly imprecise
//!implementation heavily based on a [LibCL implementation](https://github.com/ochafik/LibCL/blob/master/src/main/resources/LibCL/strtod.c)
//!TODO: fix inf, nan, signan
[[gnu::pure]] constexpr double mcsl::str_to_real(const char* str, const uint strlen, uint radix) {
   //https://dl.acm.org/doi/pdf/10.1145/93548.93559?download=false
   //https://dl.acm.org/doi/pdf/10.1145/93548.93557?download=false
   //https://www.netlib.org/fp/
   
   assert(str && strlen, __PARSE_NULL_STR_MSG, ErrCode::SEGFAULT);

   //deduce sign, radix, and starting index
   bool isNegative = str[0] == '-';
   const char* it = str + (isNegative || str[0] == '+');

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

   const uint maxMantDigits = 18; //!TODO: calculate this based on the value of `radix` - these numbers come from long double
   
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

   //calculate exponent and adjust mantissa
   // sint exp = radixPt ? (radixPt - mantEnd - 1) : 0;
   sint mantExpAdj = 0;
   const char* mantProcessEnd = mantStart + maxMantDigits + (bool)radixPt;
   if (mantProcessEnd > mantEnd) {
      mantProcessEnd = mantEnd;
   } else {
      if (!radixPt) {
         mantExpAdj += mantEnd - mantProcessEnd;
      } else if (radixPt > mantProcessEnd) {
         mantExpAdj += radixPt - mantProcessEnd;
      }
   }
   if (radixPt < mantProcessEnd) {
      mantExpAdj += radixPt - mantProcessEnd + 1;
   }
   sint exp = 0;
   if (it + 1 < end) {
      if ((radix < 0xE && (*it | CASE_BIT) == 'e') || (*it | CASE_BIT) == 'p') {
         exp += str_to_sint(it + 1, end, 10);
         // exp += str_to_sint(it + 1, end, radix);
      }
   }
   // //check exponent bounds
   // if (exp > _strtod_maxExponent_) {
   //    return isNegative ? -Inf : Inf;
   // }
   // if (-exp > _strtod_maxExponent_) {
   //    return isNegative ? -0.0 : 0.0;
   // }

	//process mantissa   
   ulong tmp = 0;
   for (it = mantStart; it < mantProcessEnd; ++it) {
      if (*it == '.') {
         continue;
      }
      tmp = radix*tmp + digit_to_uint(*it);
   }
   long double fraction = tmp;
   fraction *= std::pow(radix, mantExpAdj);

   //return
   // fraction *= std::pow((long double)radix, exp);
   if (radix == 10) {
      fraction *= std::pow(10, exp);
   } else {
      fraction *= std::pow(2, exp);
   }
	return isNegative ? -fraction : fraction;
}


#endif //MCSL_STR_TO_NUM_CPP