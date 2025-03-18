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
   
   template<float_t T> constexpr T logb(const T x) { return std::logb(x); }
   template<float_t T> constexpr sint ilogb(const T x) { return std::ilogb(x); }
   
   template<float_t T> constexpr pair<T> fextract(const T x);
   template<float_t T> constexpr pair<T, sint> fextracti(const T x);

   // template<float_t T> constexpr T noexp(const T x);
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

template<mcsl::float_t T> constexpr mcsl::pair<T> mcsl::fextract(const T x) {
   T exp = std::logb(x);
   T signif = std::scalbn(x, -exp);
   return {signif, exp};
}
template<mcsl::float_t T> constexpr mcsl::pair<T, sint> mcsl::fextracti(const T x) {
   sint exp = std::ilogb(x);
   T signif = std::scalbn(x, -exp);
   return {signif, exp};
}

// template<mcsl::float_t T> constexpr T mcsl::noexp(const T x) {
//    //!TODO: implement
// }

template<mcsl::float_t T> constexpr mcsl::pair<T,sint> mcsl::sci_notat(const T x, const uint radix) {
   assume(radix > 1);
   if (x == 0) {
      return {0,0};
   }
   if (radix == 2) {
      return mcsl::fextracti(x);
   }

   const sint exp = (sint)mcsl::floor(mcsl::log(radix, mcsl::abs(x)));
   const T signif = x * mcsl::pow((T)radix, -exp);
   return {signif, exp};
}

#pragma endregion inlinesrc

#endif //MCSL_FPMANIP_HPP