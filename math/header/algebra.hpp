#pragma once
#ifndef MCSL_ALGEBRA_HPP
#define MCSL_ALGEBRA_HPP

#include "MCSL.hpp"

#include <utility>
#include <cmath>
#include "pair.hpp"

#include "MAP_MACRO.h"
#define _DECLTYPE(x) decltype(x)
#define _TYPES(...) MCSL_MAP_LIST(_DECLTYPE, __VA_ARGS__)
#define _MPT(...) most_precise_t<_TYPES(__VA_ARGS__)>

#define _N const num_t auto
#define _F const float_t auto
#define _I const int_t auto
#define _U const uint_t auto
#define _S const sint_t auto

#define _STDF2(mcslfunc, stdfunc) inline auto mcslfunc(_N x) -> to_float_t<decltype(x)> { return std::stdfunc(x); }
#define _STDF(func) _STDF2(func, func)

namespace mcsl {
   constexpr auto ceil(_I x) -> decltype(x) { return x; }
   constexpr auto floor(_I x) -> decltype(x) { return x; }
   constexpr auto trunc(_I x) -> decltype(x) { return x; }
   constexpr auto round(_I x) -> decltype(x) { return x; }
   constexpr auto round_fpmode(_I x) -> decltype(x) { return x; }

   _STDF(ceil)
   _STDF(floor)
   _STDF(trunc)
   _STDF(round)
   _STDF2(round_fpmode, nearbyint)

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
      //!TODO: make `quo` an integer type
      T quo = mcsl::trunc(dividend / divisor);
      T rem = dividend - (quo * divisor);
      return {rem, quo};
   }
   constexpr auto fma(_N mulLHS, _N mulRHS, _N addend) -> _MPT(mulLHS, mulRHS, addend);
   
   //!TODO: single declarations using _N?
   constexpr auto max(_N lhs, _N rhs) -> _MPT(lhs, rhs);
   constexpr auto min(_N lhs, _N rhs) -> _MPT(lhs, rhs);


   constexpr auto lerp(_N begin, _N end, _F t) -> _MPT(begin, end, t);


   _STDF(exp)
   _STDF2(exp_minus_1, expm1)
   _STDF(exp2)

   inline auto log(_N base, _N val) -> to_float_t<_MPT(base, val)> { return std::log2(val) / std::log2(base); }
   _STDF2(ln, log)
   _STDF(log2)
   _STDF(log10)
   _STDF2(ln_1_plus, log1p)

   template<uint_t pow_t, int_t base_t = pow_t> constexpr base_t pow(const base_t base, const pow_t power);
   inline auto pow(_N base, _N power) -> to_float_t<_MPT(base, power)> { return std::pow(base, power); }
   _STDF(sqrt)
   _STDF(cbrt)
   template<num_t... Ts> inline auto hypot(const Ts... xs) -> to_float_t<most_precise_t<Ts...>>;
   template<num_t... Ts> constexpr auto hypot_sq(const Ts... xs) -> most_precise_t<Ts...>;

   _STDF(cos)
   _STDF(sin)
   _STDF(tan)

   _STDF(acos)
   _STDF(asin)
   _STDF(atan)
   _STDF(atan2)

   _STDF(cosh)
   _STDF(sinh)
   _STDF(tanh)

   _STDF(acosh)
   _STDF(asinh)
   _STDF(atanh)
   
   _STDF(erf)
   _STDF(erfc)

   _STDF2(gamma, tgamma)
   inline auto factorial(_F x) -> decltype(x) { return gamma(x+1); } //no _S version because x! is undefined for all negative integers
   constexpr auto factorial(_U x) -> decltype(x);
   _STDF2(ln_gamma, lgamma)


   
};

#pragma region inlinesrc

template<mcsl::uint_t pow_t, mcsl::int_t base_t> constexpr base_t mcsl::pow(const base_t base, const pow_t power) {
   if (base == 1 || power == 0) { //pow(0,0) == 1
      return 1;
   }
   if (base == 0 || power == 1) {
      return base;
   }

   //!TODO: use this if consteval, use std::pow otherwise (numbers large enough to be imprecise would cause overflow)
   base_t res = power;
   while (--power) {
      res *= base;
   }
   return res;
}

constexpr auto mcsl::fma(_N mulLHS, _N mulRHS, _N addend) -> _MPT(mulLHS, mulRHS, addend) {
   if consteval {
      return mulLHS * mulRHS + addend;
   }
   else {
      if constexpr(any_float_t<_TYPES(mulLHS, mulRHS, addend)>) {
         return std::fma(mulLHS, mulRHS, addend);
      } else {
         return mulLHS * mulRHS + addend;
      }
   }
}

constexpr auto mcsl::max(_N lhs, _N rhs) -> _MPT(lhs, rhs) {
   return std::cmp_greater(lhs, rhs) ? lhs : rhs;
}
constexpr auto mcsl::min(_N lhs, _N rhs) -> _MPT(lhs, rhs) {
   return std::cmp_less(lhs, rhs) ? lhs : rhs;
}

constexpr auto mcsl::lerp(_N begin, _N end, _F t) -> _MPT(begin, end, t) {
   if consteval {
      return begin + t * (end - begin);
   } else {
      return std::lerp(begin, end, t);
   }
}

template<mcsl::num_t... Ts> inline auto mcsl::hypot(const Ts... xs) -> to_float_t<most_precise_t<Ts...>> {
   to_float_t<most_precise_t<Ts...>> h = 0;
   auto calc = [&](const decltype(h) val) { h = std::hypot(h, val); };
   (calc(xs) , ...);
   return h;
}
template<mcsl::num_t... Ts> constexpr auto mcsl::hypot_sq(const Ts... xs) -> most_precise_t<Ts...> {
   return ((xs * xs) + ...);
}

namespace { //factorial implementation helper functions and table
   template<mcsl::uint_t T = uword> consteval T __FACT_INV_ZERO() {
      T fact = 1;
      T n = 1;
      while (fact) {
         fact *= ++n;
      }
      return n;
   }
   struct __BUF { //to avoid depending on anything else (so everything else can depend on this file)
      uword buf[__FACT_INV_ZERO()];
      constexpr __BUF() {
         buf[0] = 1;
         for (uint i = 1; i < __FACT_INV_ZERO(); ++i) {
            buf[i] = i * buf[i-1];
         }
      }
      static constexpr uword size() { return __FACT_INV_ZERO(); }
      constexpr uword& operator[](const uword i) { return buf[i]; }
      constexpr uword operator[](const uword i) const { return buf[i]; }
   };
   static constexpr __BUF __FACT_BUF{};
   static_assert(__FACT_BUF[__FACT_BUF.size()-1] * __FACT_BUF.size() == 0);
};
constexpr auto mcsl::factorial(_U x) -> decltype(x) {
   if (x >= __FACT_BUF.size()) {
      return 0;
   }
   return (decltype(x))__FACT_BUF[x];
}

#pragma endregion inlinesrc

#undef _STDF
#undef _STDF2

#undef _S
#undef _U
#undef _I
#undef _F
#undef _N

#undef _MPT
#undef _TYPES
#undef _DECLTYPE
#include "MAP_MACRO_UNDEF.h"

#endif //MCSL_ALGEBRA_HPP