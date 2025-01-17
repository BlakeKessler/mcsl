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
   if (!strlen) {
      mcsl_throw(ErrCode::STRTOINT, "mcsl::str_to_uint() cannot convert string of length 0");
   }
   
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
      mcsl_throw(ErrCode::STRTOINT, "radix for mcsl::str_to_uint() must be between 2 and 36 (not %u)", radix);
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
         mcsl_throw(ErrCode::STRTOINT, "unsigned integer overflow at index %u", i);
      }

      val += digit;
   }
   return val;
}

//!convert string to signed integer
//!legal radices: {0, 2, ... , 36}
//!when radix is 0, base is deduced from contents of string
[[gnu::pure]] constexpr slong mcsl::str_to_sint(const char* str, const uint strlen, uint radix) {
   if (!strlen) {
      mcsl_throw(ErrCode::STRTOINT, "mcsl::str_to_uint() cannot convert string of length 0");
   }

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
   // mcsl_throw(ErrCode::STRTOINT, "%c (ASCII: %u) is not a valid base-36 digit", ch, (uint)ch);
}

//!returns whether or not an entire string is digits for the specifed base
[[gnu::pure]] constexpr bool mcsl::is_uint(const char* str, const uint strlen, const uint radix) {
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
[[gnu::pure]] constexpr double mcsl::str_to_real(const char* str, const uint strlen, uint radix) {
   //https://dl.acm.org/doi/pdf/10.1145/93548.93559?download=false
   //https://dl.acm.org/doi/pdf/10.1145/93548.93557?download=false
   //https://www.netlib.org/fp/
   
   
   assert(strlen);

   //deduce sign, radix, and starting index
   bool isNegative = str[0] == '-';
   const char* it = str + isNegative;

   if (strlen >= (2 + isNegative) && *it == '0') {
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

   //find first non-zero digit and radix point/separator
   const char* firstNonZero = nullptr;
   const char* radixPoint = nullptr;
   const char* radixSeparator = nullptr;
   const char* numEnd = nullptr;

   {
      const char* strEnd = str + strlen;
      for (; it < strEnd; ++it) {
         if (*it == '.') {
            if (radixPoint) {
               numEnd = it;
               break;
            }
            radixPoint = it;
            if (!firstNonZero) {
               ++it;
               if (is_digit(*it, radix)) {
                  firstNonZero = it;
               } else {
                  numEnd = it;
                  break;
               }
            }
         } 
         else if ((radix < (10 + 'e' - 'a') && *it == 'e') || *it == 'p') {
            if (radixSeparator) {
               numEnd = it;
               break;
            }

            radixSeparator = it;
            ++it;
            if (it < strEnd) {
               if (!is_digit(*it, radix) && *it != '-' && *it != '+') {
                  numEnd = it;
                  break;
               }
            }
         }
         else {
            if (!is_digit(*it, radix)) {
               numEnd = it;
               break;
            }
            else if (*it != '0') {
               firstNonZero = firstNonZero ? firstNonZero : it;
            }
         }
      }
      numEnd = numEnd ? numEnd : strEnd;
   }

   //check validity
   if (!firstNonZero || (radixSeparator && radixSeparator < radixPoint)) {
      return SigNaN;
   }
   
   //parse float
   uint_n<128> significand = 0;
   // uint_n<(uint)(0.5 + 0.125 * std::log2(TYPEMAX<double>()))> significand;
   uint bitWidth;
   double num = isNegative ? -1.0 : 1.0;

   if (radixSeparator) {
      num *= std::pow(radix, str_to_sint(radixSeparator + 1, numEnd, radix));
      numEnd = radixSeparator;
   }
   for (it = firstNonZero; it < radixPoint; ++it) {
      significand *= radix;
      significand += digit_to_uint(*it);
   }
   if (radixPoint) { bitWidth = significand.bit_width(); ++it; }
   for (; it < numEnd; ++it) {
      significand *= radix;
      significand += digit_to_uint(*it);
   }
   if (!radixPoint) { bitWidth = significand.bit_width(); }
   

   //convert to floating point number and return
   mcsl::pair<ulong, ushort> msw = significand.get_msw();
   sint exp = bitWidth - significand.bit_width() - msw.second;

   return num * std::ldexp(msw.first, exp);
}

#endif //MCSL_STR_TO_NUM_CPP