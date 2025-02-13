#pragma once
#ifndef ALGEBRA_MCSL
#define ALGEBRA_MCSL

#include "MCSL.hpp"

#include "pair.hpp"

namespace mcsl {
   template<float_t T> inline T abs(const T);
   template<float_t T> inline T mod(const T);
   template<float_t T> inline pair<T> remquo(const T dividend, const T divisor);
   template<float_t T> inline T fma(const T mulLHS, const T mulRHS, const T addend);
   template<float_t T> inline T max(const T lhs, const T rhs);
   template<float_t T> inline T min(const T lhs, const T rhs);

   template<float_t T> inline T lerp(const T begin, const T end, const T t);


   template<float_t T, more_precise_t<T> ret_t = T> inline ret_t exp(const T x);
   template<float_t T, more_precise_t<T> ret_t = T> inline ret_t exp_minus_1(const T x);
   template<num_t T, more_precise_t<T> ret_t = T> inline ret_t exp2(const T x);

   template<float_t T> inline T ln(const T x);
   template<float_t T> inline T log2(const T x);
   template<float_t T> inline T log10(const T x);
   template<float_t T> inline T ln_1_plus(const T x);

   template<float_t T> inline T pow(const T base, const T power);
   template<float_t T> inline T sqrt(const T x);
   template<float_t T> inline T cbrt(const T x);
   template<float_t... T> inline auto hypot(const T... x);
   template<float_t... T> inline auto hypot_sq(const T... x);


   template<float_t T> inline T cos(const T x);
   template<float_t T> inline T sin(const T x);
   template<float_t T> inline T tan(const T x);

   template<float_t T> inline T acos(const T x);
   template<float_t T> inline T asin(const T x);
   template<float_t T> inline T atan(const T x);
   template<float_t T> inline T atan2(const T x);

   template<float_t T> inline T cosh(const T x);
   template<float_t T> inline T sinh(const T x);
   template<float_t T> inline T tanh(const T x);

   template<float_t T> inline T acosh(const T x);
   template<float_t T> inline T asinh(const T x);
   template<float_t T> inline T atanh(const T x);
   
   template<float_t T> inline T erf(const T x);
   template<float_t T> inline T erfc(const T x);

   template<num_t T, more_precise_t<T> ret_t> ret_t gamma(const T x);
   template<num_t T, more_precise_t<T> ret_t> ret_t factorial(const T x);
   template<float_t T, more_precise_t<T> ret_t> ret_t ln_gamma(const T x);

   //!TODO: rounding functions
};

#endif //ALGEBRA_MCSL