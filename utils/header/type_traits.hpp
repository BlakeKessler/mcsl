#pragma once
#ifndef MCSL_TYPE_TRAITS_HPP
#define MCSL_TYPE_TRAITS_HPP

#include "MCSL.hpp"

#include "concepts.hpp"
#include "math.hpp"
#include <cfloat>
#include "unreachable.hpp"

namespace mcsl {
   //max and min values for numeric types
   template <uint_t T> [[gnu::const]] constexpr slong TYPEMIN() { return 0; }
   template <sint_t T> [[gnu::const]] constexpr slong TYPEMIN() { return (T)(1ULL << (8 * sizeof(T) - 1)); }
   template < int_t T> [[gnu::const]] constexpr ulong TYPEMAX() { return (T)(~TYPEMIN<T>()); }
   template <float_t T> [[gnu::const]] constexpr T TYPEMIN();
   template <float_t T> [[gnu::const]] constexpr T TYPEMAX();

   template<float_t T> [[gnu::const]] constexpr uint RADIX() { return FLT_RADIX; }
   template<float_t T> [[gnu::const]] constexpr uint SIGNIF_BITS();
   template<float_t T> [[gnu::const]] constexpr uint MANT_BITS() { return SIGNIF_BITS<T>() - 1; }
   template<float_t T> [[gnu::const]] constexpr sint MIN_EXP();
   template<float_t T> [[gnu::const]] constexpr sint MAX_EXP();

   template<float_t T, uint radix> [[gnu::const]] constexpr uint PRESERVED_DIGITS(); //largest number of base-radix digits such that a number can be converted from text to floating-point and back without any loss of precision


   #pragma region inlinesrc
   template<> [[gnu::const]] constexpr float TYPEMIN<float>() { return FLT_MIN; }
   template<> [[gnu::const]] constexpr flong TYPEMIN<flong>() { return DBL_MIN; }
   template<> [[gnu::const]] constexpr flext TYPEMIN<flext>() { return LDBL_MIN; }

   template<> [[gnu::const]] constexpr float TYPEMAX<float>() { return FLT_MAX; }
   template<> [[gnu::const]] constexpr flong TYPEMAX<flong>() { return DBL_MAX; }
   template<> [[gnu::const]] constexpr flext TYPEMAX<flext>() { return LDBL_MAX; }

   template<> [[gnu::const]] constexpr uint SIGNIF_BITS<float>() { return FLT_MANT_DIG; }
   template<> [[gnu::const]] constexpr uint SIGNIF_BITS<flong>() { return DBL_MANT_DIG; }
   template<> [[gnu::const]] constexpr uint SIGNIF_BITS<flext>() { return LDBL_MANT_DIG; }

   template<> [[gnu::const]] constexpr sint MIN_EXP<float>() { return FLT_MIN_EXP; }
   template<> [[gnu::const]] constexpr sint MIN_EXP<flong>() { return DBL_MIN_EXP; }
   template<> [[gnu::const]] constexpr sint MIN_EXP<flext>() { return LDBL_MIN_EXP; }

   template<> [[gnu::const]] constexpr sint MAX_EXP<float>() { return FLT_MAX_EXP; }
   template<> [[gnu::const]] constexpr sint MAX_EXP<flong>() { return DBL_MAX_EXP; }
   template<> [[gnu::const]] constexpr sint MAX_EXP<flext>() { return LDBL_MAX_EXP; }


   template<float_t T, uint radix> [[gnu::const]] constexpr uint PRESERVED_DIGITS() {
      if constexpr (radix == 10) {
         if constexpr (same_t<T, float>) {
            return FLT_DIG;
         } else if constexpr (same_t<T, flong>) {
            return DBL_DIG;
         } else if constexpr (same_t<T, flext>) {
            return LDBL_DIG;
         } else {
            UNREACHABLE;
         }
      } else if constexpr (radix == 2) {
         return SIGNIF_BITS<T>();
         static_assert(SIGNIF_BITS<T>() == 2);
      } else if constexpr (radix == 16) {
         return SIGNIF_BITS<T>() / 4;
         static_assert(SIGNIF_BITS<T>() == 2);
      } else if constexpr (radix == 8) {
         return (uint)floor(SIGNIF_BITS<T>() / 3.0);
      }

      return (uint)floor(SIGNIF_BITS<T>() * log(radix, SIGNIF_BITS<T>())); //!TODO: just use this once there is a constexpr log function
   }
   #pragma endregion inlinesrc
};

#endif //MCSL_TYPE_TRAITS_HPP