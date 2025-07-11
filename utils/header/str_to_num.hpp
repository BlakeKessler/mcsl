#pragma once
#ifndef MCSL_STR_TO_NUM_HPP
#define MCSL_STR_TO_NUM_HPP

#include "MCSL.hpp"

#include "num.hpp"
#include "pair.hpp"
#include "assert.hpp"

namespace mcsl {
   namespace _ {
      #define val_t(T) struct T { T##long val; uint len; operator T##long() const { return val; } operator pair<T##long, uint>() const { return {val, len}; }}
      val_t(u);
      val_t(s);
      val_t(f);
      val_t(n);
      #undef val_t

      [[gnu::pure]] constexpr u __str_to_uint_impl(const char* str, const uint strlen, const uint radix, ulong val);
      [[gnu::pure]] constexpr u str_to_uint_known_radix(const char* str, const uint strlen, const uint radix) { return __str_to_uint_impl(str, strlen, radix, 0); }
   };
   template<uint radix> [[gnu::pure]] constexpr _::u str_to_uint(const char* str, const uint strlen);
   [[gnu::pure]] constexpr _::u str_to_uint(const char* str, const uint strlen, uint radix = 0);
   [[gnu::pure]] constexpr _::s str_to_sint(const char* str, const uint strlen, uint radix = 0);
   [[gnu::pure]] constexpr _::f str_to_real(const char* str, const char* end, uint radix = 0);

   [[gnu::pure]] constexpr _::f c_float_lit_str_to_real(const char* str, const char* end, uint radix = 0);
   [[gnu::pure]] constexpr _::n str_to_num(const char* str, const char* end, uint radix = 0);

   [[gnu::pure]] constexpr _::f str_to_real(const char* str, const uint strlen, uint radix = 0) { return str_to_real(str, str + strlen, radix); }
   [[gnu::pure]] constexpr _::f c_float_lit_str_to_real(const char* str, const uint strlen, uint radix = 0) { return c_float_lit_str_to_real(str, str + strlen, radix); }
   [[gnu::pure]] constexpr _::n str_to_num(const char* str, const uint strlen, uint radix = 0) { return str_to_num(str, str + strlen, radix); }

   [[gnu::pure]] constexpr _::u str_to_uint(const char* str, const char* end, uint radix = 0) { assume(str && end && str < end); return str_to_uint(str, end-str, radix); }
   [[gnu::pure]] constexpr _::s str_to_sint(const char* str, const char* end, uint radix = 0) { assume(str && end && str < end); return str_to_sint(str, end-str, radix); }


   [[gnu::const, gnu::always_inline]] constexpr sint8 digit_to_uint(const char ch);
   [[gnu::const]] constexpr inline bool is_digit(const char ch, const uint radix = 10) { return (uint8)digit_to_uint(ch) < radix; }
   [[gnu::pure]] constexpr bool is_uint(const char* str, const uint strlen, const uint radix = 10);
   [[gnu::pure]] constexpr bool is_uint(const char* str, const char* end, const uint radix = 10) { if (str < end) { return is_uint(str, end-str, radix); } else { return false; }}
}

#include "../src/str_to_num.cpp"

#endif //MCSL_STR_TO_NUM_HPP