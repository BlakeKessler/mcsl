#pragma once
#ifndef MCSL_TYPE_TRAITS_HPP
#define MCSL_TYPE_TRAITS_HPP

#include "MCSL.hpp"
#include "concepts.hpp"

#include <cfloat>

namespace mcsl {
   //max and min values for numeric types
   template <uint_t T> [[gnu::const]] constexpr slong TYPEMIN() { return 0; }
   template <sint_t T> [[gnu::const]] constexpr slong TYPEMIN() { return (T)(1ULL << (8 * sizeof(T) - 1)); }
   template < int_t T> [[gnu::const]] constexpr ulong TYPEMAX() { return (T)(~TYPEMIN<T>()); }
   
   template <float_t T> [[gnu::const]] constexpr T TYPEMIN();
   template <float_t T> [[gnu::const]] constexpr T TYPEMAX();
   template<> [[gnu::const]] constexpr float TYPEMIN<float>() { return FLT_MIN; }
   template<> [[gnu::const]] constexpr float TYPEMAX<float>() { return FLT_MAX; }
   template<> [[gnu::const]] constexpr double TYPEMIN<double>() { return DBL_MIN; }
   template<> [[gnu::const]] constexpr double TYPEMAX<double>() { return DBL_MAX; }


};

#endif //MCSL_TYPE_TRAITS_HPP