#pragma once
#ifndef MCSL_NUM_TO_STR_HPP
#define MCSL_NUM_TO_STR_HPP

#include "MCSL.hpp"

#include "raw_buf_str.hpp"
#include "assert.hpp"

namespace mcsl {
   [[gnu::pure, gnu::always_inline]] constexpr char digit_to_char(const ubyte digit, const bool isLowercase);
   template<uint_t T> [[gnu::pure]] constexpr raw_buf_str<8*sizeof(T)> uint_to_str(T num, const uint radix, const bool isLowercase);
};



#pragma region inlinesrc

[[gnu::pure, gnu::always_inline]] constexpr char mcsl::digit_to_char(const ubyte digit, const bool isLowercase) {
   assume(digit < 36);
   char c = '0' | digit;
   if (digit >= 0xA) {
      c += isLowercase ? ('a' - '0' - 0xA) : ('A' - '0' - 0xA);
   }
   return c;
}

template<mcsl::uint_t T> [[gnu::pure]] constexpr mcsl::raw_buf_str<8*sizeof(T)> mcsl::uint_to_str(T num, const uint radix, const bool isLowercase) {
   //calculate digit string
   raw_buf_str<8*sizeof(T)> digits;
   do {
      const ubyte digit = num % radix;
      num /= radix;
      digits.push_back(mcsl::digit_to_char(digit, isLowercase));
   } while (num);

   //reverse digit string
   uint fwd = 0;
   uint bwd = digits.size() - 1;
   while (fwd < bwd) {
      const char tmp = digits[fwd];
      digits[fwd] = digits[bwd];
      digits[bwd] = tmp;
      ++fwd;
      --bwd;
   }

   return digits;
}

#pragma endregion inlinesrc

#endif //MCSL_NUM_TO_STR_HPP