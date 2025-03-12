#pragma once
#ifndef MCSL_FPMANIP_HPP
#define MCSL_FPMANIP_HPP

#include "MCSL.hpp"

#include "pair.hpp"
#include "math.hpp"
#include <cmath>

namespace mcsl {
   template<float_t T> inline T makeSigNaN(const ulong payload);
   template<float_t T> inline T makeQuietNaN(const ulong payload);


   template<float_t T> constexpr pair<T> modf(const T x);
   template<float_t T> constexpr pair<T, sint> frexp(const T x); //frexp
   template<float_t T> constexpr pair<T, sint> sci_notat(const T x, const uint radix); //M*r^E = (M*b^((E*log[b](r))%1))*(b^(floor(E*log[b](r))))    (I think, might have made a mistake)
   //!TODO: the rest of the cmath floating point manipulation functions
};



#pragma region inlinesrc

template<mcsl::float_t T> constexpr mcsl::pair<T> mcsl::modf(const T x) {
   if consteval {
      T fracPart = mod(x, 1);
      return {x - fracPart, fracPart};
   } else {
      T intPart;
      T fracPart = std::modf(x, &intPart);
      return {intPart,fracPart};
   }
}

template<mcsl::float_t T> constexpr mcsl::pair<T,sint> mcsl::frexp(const T x) {
   sint exp;
   T fr = std::frexp(x, &exp);
   return {fr,exp};
}
template<mcsl::float_t T> constexpr mcsl::pair<T,sint> mcsl::sci_notat(const T x, const uint radix) {
   assume(radix > 1);
   if (radix == 2) {
      return mcsl::frexp(x);
   }

   auto [frac, exp2] = mcsl::frexp(x);
   const auto [outExp, expRFrac] = mcsl::modf(exp2 * mcsl::log(radix, 2));
   frac *= mcsl::pow(radix, expRFrac);

   debug_assert(mcsl::abs(frac) < radix);

   return {frac, (sint)outExp};
}

#pragma endregion inlinesrc

#endif //MCSL_FPMANIP_HPP