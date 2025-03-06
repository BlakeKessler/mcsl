#pragma once
#ifndef MCSL_ALGEBRA_HPP
#define MCSL_ALGEBRA_HPP

#include "MCSL.hpp"

#include <cmath>
#include "pair.hpp"

namespace mcsl {
   template<float_t T> constexpr T abs(const T x) { if consteval { return x >= 0 ? x : -x; } else { return std::abs(x); } }
   template< sint_t T> constexpr T abs(const T x) { return x >= 0 ? x : -x; }
   template< uint_t T> constexpr T abs(const T x) { return x; }
   template<  num_t T> constexpr T mod(const T dividend, const T divisor) { return dividend % divisor; }
   template<float_t T> inline pair<T, sint> cremquo(const T dividend, const T divisor) {
      sint tmp;
      T rem = std::remquo(dividend, divisor, &tmp);
      return {rem, tmp};
   }
   template<num_t T> constexpr pair<T> remquo(const T dividend, const T divisor) {
      T quo = trunc(dividend / divisor);
      T rem = dividend - (quo * divisor);
      return {rem, quo};
   }
   template<float_t T> constexpr T fma(const T mulLHS, const T mulRHS, const T addend) { if consteval { return mulLHS * mulRHS + addend; } else { return std::fma(mulLHS, mulRHS, addend); } }
   template<  int_t T> constexpr T fma(const T mulLHS, const T mulRHS, const T addend) { return mulLHS * mulRHS + addend; }
   template<float_t T> constexpr T max(const T lhs, const T rhs) { if consteval {return lhs > rhs ? lhs : rhs; } else { return std::max(lhs, rhs); } }
   template<  int_t T> constexpr T max(const T lhs, const T rhs) { return lhs > rhs ? lhs : rhs; }
   template<float_t T> constexpr T min(const T lhs, const T rhs) { if consteval {return lhs < rhs ? lhs : rhs; } else { return std::min(lhs, rhs); } }
   template<  int_t T> constexpr T min(const T lhs, const T rhs) { return lhs < rhs ? lhs : rhs; }



   template<float_t T> inline T lerp(const T begin, const T end, const T t) { if consteval { return begin + t * (end - begin); } else { return std::lerp(begin, end, t); } }


   template<num_t T> inline T exp(const T x) { return std::exp(x); }
   template<num_t T> inline T exp_minus_1(const T x) { return std::expm1(x); }
   template<num_t T> inline T exp2(const T x) { return std::exp2(x); }

   template<float_t T> inline T ln(const T x) { return std::log(x); }
   template<float_t T> inline T log2(const T x) { return std::log2(x); }
   template<float_t T> inline T log10(const T x) { return std::log10(x); }
   template<float_t T> inline T ln_1_plus(const T x) { return std::log1p(x); }

   template<uint_t pow_t, int_t base_t = pow_t> constexpr base_t pow(const base_t base, const pow_t power);
   template<float_t T> inline T pow(const T base, const T power) { return std::pow(base, power); }
   template<float_t T> inline T sqrt(const T x) { return std::sqrt(x); }
   template<float_t T> inline T cbrt(const T x) { return std::cbrt(x); }
   template<float_t T> inline T hypot(const T x) { return x; }
   template<float_t T, float_t... Ts> inline auto hypot(const T x, const Ts... xs) { return std::hypot(x, hypot(xs...)); }
   template<num_t... T> constexpr auto hypot_sq(const T... xs) { return ((xs * xs) + ...); }


   template<float_t T> inline T cos(const T x) { return std::cos(x); }
   template<float_t T> inline T sin(const T x) { return std::sin(x); }
   template<float_t T> inline T tan(const T x) { return std::tan(x); }

   template<float_t T> inline T acos(const T x) { return std::acos(x); }
   template<float_t T> inline T asin(const T x) { return std::asin(x); }
   template<float_t T> inline T atan(const T x) { return std::atan(x); }
   template<float_t T> inline T atan2(const T x) { return std::atan2(x); }

   template<float_t T> inline T cosh(const T x) { return std::cosh(x); }
   template<float_t T> inline T sinh(const T x) { return std::sinh(x); }
   template<float_t T> inline T tanh(const T x) { return std::tanh(x); }

   template<float_t T> inline T acosh(const T x) { return std::acosh(x); }
   template<float_t T> inline T asinh(const T x) { return std::asinh(x); }
   template<float_t T> inline T atanh(const T x) { return std::atanh(x); }
   
   template<float_t T> inline T erf(const T x) { return std::erf(x); }
   template<float_t T> inline T erfc(const T x) { return std::erfc(x); }

   template<float_t T> T gamma(const T x) { return std::tgamma(x); }
   template<float_t T> T factorial(const T x) { return gamma(x+1); }
   template< uint_t T> constexpr T factorial(const T x);
   template<float_t T> T ln_gamma(const T x) { return std::lgamma(x); }


   template<float_t T> T ceil(const T x) { return std::ceil(x); }
   template<float_t T> T floor(const T x) { return std::floor(x); }
   template<float_t T> T trunc(const T x) { return std::trunc(x); }
   template<float_t T> T round(const T x) { return std::round(x); }
   template<float_t T> T round_fpmode(const T x) { return std::nearbyint(x); }

   template<int_t T> constexpr T ceil(const T x) { return x; }
   template<int_t T> constexpr T floor(const T x) { return x; }
   template<int_t T> constexpr T trunc(const T x) { return x; }
   template<int_t T> constexpr T round(const T x) { return x; }
   template<int_t T> constexpr T round_fpmode(const T x) { return x; }
};



#pragma region inlinesrc

template<mcsl::uint_t pow_t, mcsl::int_t base_t> constexpr base_t mcsl::pow(const base_t base, const pow_t power) {
   if (base == 1 || power == 0) { //pow(0,0) == 1
      return 1;
   }
   if (base == 0 || power == 1) {
      return base;
   }

   base_t res = power;
   while (--power) {
      res *= base;
   }
   return res;
}

#include "dyn_arr.hpp"
template<mcsl::uint_t T> constexpr T mcsl::factorial(const T x) {
   if consteval {
      T factX = 1;
      T i = x;
      while (i) {
         factX *= i--;
      }
      return factX; 
   }
   static dyn_arr<T> memoBuf{1};
   while (memoBuf.size() <= x) {
      memoBuf.push_back(memoBuf.back() * memoBuf.size());
   }
   return memoBuf[x];
}

#pragma endregion inlinesrc

#endif //MCSL_ALGEBRA_HPP