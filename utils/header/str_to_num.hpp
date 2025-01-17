#pragma once
#ifndef MCSL_STR_TO_NUM_HPP
#define MCSL_STR_TO_NUM_HPP

#include "MCSL.hpp"

namespace mcsl {
   [[gnu::pure ]] constexpr  ulong str_to_uint(const char* str, const uint strlen, uint radix = 0);
   [[gnu::pure ]] constexpr  slong str_to_sint(const char* str, const uint strlen, uint radix = 0);
   [[gnu::pure ]] constexpr double str_to_real(const char* str, const uint strlen, uint radix = 0);

   [[gnu::pure ]] constexpr  ulong str_to_uint(const char* begin, const char* end, uint radix = 0) { { if (begin < end) { return str_to_uint(begin, end-begin, radix); } else { return 0; }} }
   [[gnu::pure ]] constexpr  slong str_to_sint(const char* begin, const char* end, uint radix = 0) { { if (begin < end) { return str_to_sint(begin, end-begin, radix); } else { return 0; }} }
   [[gnu::pure ]] constexpr double str_to_real(const char* begin, const char* end, uint radix = 0) { { if (begin < end) { return str_to_real(begin, end-begin, radix); } else { return NaN; }} }

   [[gnu::const, gnu::always_inline]] constexpr sint8 digit_to_uint(const char ch);
   [[gnu::const]] constexpr inline bool is_digit(const char ch, const uint radix = 10) { return (uint8)digit_to_uint(ch) < radix;}
   [[gnu::pure ]] constexpr bool is_uint(const char* str, const uint strlen, const uint radix = 10);
   [[gnu::pure ]] constexpr bool is_uint(const char* begin, const char* end, const uint radix = 10) { if (begin < end) { return is_uint(begin, end-begin, radix); } else { return false; }}


   constexpr uint8 CASE_BIT = 'a' ^ 'A';
}

#include "../src/str_to_num.cpp"

#endif //MCSL_STR_TO_NUM_HPP