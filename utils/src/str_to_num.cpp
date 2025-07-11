#ifndef MCSL_STR_TO_NUM_CPP
#define MCSL_STR_TO_NUM_CPP

#include "str_to_num.hpp"
#include "carry.hpp"
#include "uint_n.hpp"
#include "assert.hpp"
#include "type_traits.hpp"

#include "math.hpp"
#include "throw.hpp"

//!TODO: add template parameters for radix, only deduce when radix == 0

[[gnu::pure]] constexpr mcsl::_::u mcsl::_::__str_to_uint_impl(const char* str, const uint strlen, const uint radix, ulong val) {
   uint i = 0;
   while (i < strlen) {
      //parse digit
      uint8 digit = (uint8)digit_to_uint(str[i]);
      //check digit - NOTE: consolidates check for validity using integer underflow
      if (digit >= radix) { [[unlikely]] break; }

      //push digit - NOTE: manually inlines mcsl::MUL (multiplication with carry)
      uoverlong tmp = (uoverlong)val * (uoverlong)radix;
      val = (ulong)tmp;
      if ((ulong)(tmp >> (sizeof(ulong) * 8))) { [[unlikely]] //check for overflow
         __throw(ErrCode::STRTOINT, mcsl::FMT("unsigned integer overflow at index %u"), i);
      }

      val += digit;
      ++i;
   }

   return {
      .val = val,
      .len = i
   };
}

//!convert string to unsigned integer
//!legal radices: {0, 2, ... , 36}
//!when radix is 0, base is deduced from contents of string
[[gnu::pure]] constexpr mcsl::_::u mcsl::str_to_uint(const char* str, const uint strlen, uint radix) {
   assert(str && strlen, __PARSE_NULL_STR_MSG, ErrCode::SEGFAULT);

   uint i = 0;

   //deduce radix and starting index
   if (strlen >= 2 && str[0] == '0') {
      if (radix == 0) {
         switch(str[1]) {
            case 'b': case 'B': radix =  2; i = 2; break;
            case 'o': case 'O': radix =  8; i = 2; break;
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
   if (radix < 2 || radix > 36) { [[unlikely]];
      __throw(ErrCode::STRTOINT, mcsl::FMT("radix for mcsl::str_to_uint() must be between 2 and 36 (not %u)"), radix);
   }

   //parse integer
   auto res = _::__str_to_uint_impl(str + i, strlen - i, radix, 0);

   return {
      .val = res.val,
      .len = i + res.len
   };
}

//!convert string to signed integer
//!legal radices: {0, 2, ... , 36}
//!when radix is 0, base is deduced from contents of string
[[gnu::pure]] constexpr mcsl::_::s mcsl::str_to_sint(const char* str, const uint strlen, uint radix) {
   assert(str && strlen, __PARSE_NULL_STR_MSG, ErrCode::SEGFAULT);

   if (str[0] == '-') {
      auto tmp = str_to_uint(str+1, strlen-1, radix);
      return {
         .val = -(slong)tmp.val,
         .len = tmp.len + 1
      };
   } else if (str[0] == '+') {
      auto tmp = str_to_uint(str+1, strlen-1, radix);
      return {
         .val = (slong)tmp.val,
         .len = tmp.len + 1
      };
   } else {
      auto tmp = str_to_uint(str, strlen, radix);
      return {
         .val = (slong)tmp.val,
         .len = tmp.len
      };
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
   // __throw(ErrCode::STRTOINT, mcsl::FMT("%c (ASCII: %u) is not a valid base-36 digit"), ch, (uint)ch);
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
//!TODO: nan, inf
[[gnu::pure]] constexpr mcsl::_::f mcsl::str_to_real(const char* str, const uint strlen, uint radix) {
   using namespace _;
   assert(str && strlen, __PARSE_NULL_STR_MSG, ErrCode::SEGFAULT);

   //deduce sign, radix, and starting index
   bool isNegative = str[0] == '-';
   const char* it = str + (isNegative || str[0] == '+');

   if (strlen >= (2U + isNegative) && *it == '0') {
      if (radix == 0) {
         switch(*++it) {
            case 'b': case 'B': radix =  2; ++it; break;
            case 'o': case 'O': radix =  8; ++it; break;
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
   
   const char* const end = str + strlen;

   uint overPrecDigits = 0;

   //before radix point
   u wholePt = __str_to_uint_impl(it, min(end - it, maxMantDigits), radix, 0);
   it += wholePt.len;
   while (it < end && is_digit(*it, radix)) { [[unlikely]];
      ++overPrecDigits;
      ++it;
   }
   //after radix point
   u fracPt;
   bool hasRadixPt = it < end && *it == '.';
   if (hasRadixPt) { [[likely]];
      ++it;
      fracPt = __str_to_uint_impl(it, min(end - it, maxMantDigits - wholePt.len), radix, wholePt.val);
      it += fracPt.len;
      while (it < end && is_digit(*it, radix)) { [[unlikely]];
         ++it;
      }
   } else {
      fracPt = {.val = wholePt.val, .len = 0};
   }

   //exponent
   flext val = fracPt.val;
   sint exp = overPrecDigits - fracPt.len;
   if (it + 2 < end && it[0] == EXP_NOTAT[0] && it[1] == EXP_NOTAT[1]) { //Middle-C style
      it += 2;
      auto tmp = str_to_sint(it, end, radix);
      exp += tmp.val;
      it += tmp.len;
   }
   else if (it + 1 < end && radix == 10 && (it[0] | CASE_BIT) == 'e') { //decimal float
      ++it;
      auto tmp = str_to_sint(it, end, radix);
      exp += tmp.val;
      it += tmp.len;
   }
   else if (it + 1 < end && (it[0] | CASE_BIT) == 'p') { //IEEE hex float
      ++it;
      auto tmp = str_to_sint(it, end, 10);
      val = ldexp(val, tmp.val);
      it += tmp.len;
   }
   val *= pow((flext)radix, exp);

   //return value
   return f{
      .val = (flong)val,
      .len = (uint)(it - str)
   };
}


//!convert string to floating point number
//!legal radices: {0, 10, 16}
//!when radix is 0, base is deduced from contents of string
//!NOTE: slightly imprecise
//!TODO: fix inf, nan, signan
//!TODO: apostrophes?
[[gnu::pure]] constexpr mcsl::_::f mcsl::c_float_lit_str_to_real(const char* str, const uint strlen, uint radix) {
   //https://dl.acm.org/doi/pdf/10.1145/93548.93559?download=false
   //https://dl.acm.org/doi/pdf/10.1145/93548.93557?download=false
   //https://www.netlib.org/fp/
   
   using namespace _;

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
   static_assert(sizeof(flext) >= 10);
   
   const char* const end = str + strlen;

   uint overPrecDigits = 0;

   //before radix point
   u wholePt = __str_to_uint_impl(it, min(end - it, maxMantDigits), radix, 0);
   it += wholePt.len;
   while (it < end && is_digit(*it, radix)) { [[unlikely]];
      ++overPrecDigits;
      ++it;
   }
   //after radix point
   u fracPt;
   bool hasRadixPt = it < end && *it == '.';
   if (hasRadixPt) { [[likely]];
      ++it;
      fracPt = __str_to_uint_impl(it, min(end - it, maxMantDigits - wholePt.len), radix, wholePt.val);
      it += fracPt.len;
      while (it < end && is_digit(*it, radix)) { [[unlikely]];
         ++it;
      }
   } else {
      fracPt = {.val = wholePt.val, .len = 0};
   }

   //exponent
   flext val = fracPt.val;
   sint exp = overPrecDigits - fracPt.len;
   if (it + 1 < end && radix == 10 && (it[0] | CASE_BIT) == 'e') { //decimal float
      ++it;
      auto tmp = str_to_sint(it, end, radix);
      exp += tmp.val;
      it += tmp.len;
   }
   else if (it + 1 < end && radix == 16 && (it[0] | CASE_BIT) == 'p') { //IEEE hex float
      ++it;
      auto tmp = str_to_sint(it, end, 10);
      val = ldexp(val, tmp.val);
      it += tmp.len;
   }
   val *= pow((flext)radix, exp);
   
   //return value
   return f{
      .val = (flong)val,
      .len = (uint)(it - str)
   };
}

//!convert string to number
//!legal radices: {0, 2, 8, 10, 16}
//!when radix is 0, base is deduced from contents of string
//!TODO: nan, inf
[[gnu::pure]] constexpr mcsl::_::n mcsl::str_to_num(const char* str, const uint strlen, uint radix) {
   using namespace _;
   assert(str && strlen, __PARSE_NULL_STR_MSG, ErrCode::SEGFAULT);

   bool isSigned = false;
   bool isReal = false;

   //deduce sign, radix, and starting index
   bool isNegative = str[0] == '-';
   isSigned = isNegative || str[0] == '+';
   const char* it = str + isSigned;

   if (strlen >= (2U + isNegative) && *it == '0') {
      if (radix == 0) {
         switch(*++it) {
            case 'b': case 'B': radix =  2; ++it; break;
            case 'o': case 'O': radix =  8; ++it; break;
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
   
   const char* const end = str + strlen;

   uint overPrecDigits = 0;

   //before radix point
   u wholePt = __str_to_uint_impl(it, min(end - it, maxMantDigits), radix, 0);
   it += wholePt.len;
   if (it < end && is_digit(*it, radix)) { [[unlikely]];
      //!TODO: support ints that would not fit in floats
      isReal = true;
      do {
         ++overPrecDigits;
         ++it;
      } while (it < end && is_digit(*it, radix));
   }
   //after radix point
   u fracPt;
   bool hasRadixPt = it < end && *it == '.';
   if (hasRadixPt) { [[likely]];
      isReal = true;
      ++it;
      fracPt = __str_to_uint_impl(it, min(end - it, maxMantDigits - wholePt.len), radix, wholePt.val);
      it += fracPt.len;
      while (it < end && is_digit(*it, radix)) { [[unlikely]];
         ++it;
      }
   } else {
      fracPt = {.val = wholePt.val, .len = 0};
   }

   //exponent
   flext val;
   sint exp;
   if (it + 2 < end && it[0] == EXP_NOTAT[0] && it[1] == EXP_NOTAT[1]) { //Middle-C style
      val = fracPt.val;
      exp = overPrecDigits - fracPt.len;

      it += 2;
      auto tmp = str_to_sint(it, end, radix);
      exp += tmp.val;
      it += tmp.len;
      isReal = true;
   }
   else if (it + 1 < end && radix == 10 && (it[0] | CASE_BIT) == 'e') { //decimal float
      val = fracPt.val;
      exp = overPrecDigits - fracPt.len;

      ++it;
      auto tmp = str_to_sint(it, end, radix);
      exp += tmp.val;
      it += tmp.len;
      isReal = true;
   }
   else if (it + 1 < end && (it[0] | CASE_BIT) == 'p') { //IEEE hex float
      val = fracPt.val;
      exp = overPrecDigits - fracPt.len;

      ++it;
      auto tmp = str_to_sint(it, end, 10);
      val = ldexp(val, tmp.val);
      it += tmp.len;
      isReal = true;
   } else if (isReal) {
      val = fracPt.val;
      exp = overPrecDigits - fracPt.len;
   }

   //return an integer if appropriatee
   if (!isReal) {
      if (isSigned) { //sint
         return n{(slong)(isNegative ? -wholePt.val : wholePt.val), (uint)(it - str)};
      } else { //uint
         return n{(ulong)wholePt.val, (uint)(it - str)};
      }
   }

   //finish calculating exponent
   val *= pow((flext)radix, exp);

   //return float
   return n{(flong)val, (uint)(it - str)};
}

#endif //MCSL_STR_TO_NUM_CPP