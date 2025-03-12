#pragma once
#ifndef MCSL_MATH_CONSTS_HPP
#define MCSL_MATH_CONSTS_HPP

#include "MCSL.hpp"

#include <numbers>

namespace mcsl{
   namespace nums {
      constexpr flong e = std::numbers::e;
      constexpr flong log2e = std::numbers::log2e;
      constexpr flong log10e = std::numbers::log10e;

      constexpr flong ln2 = std::numbers::ln2;
      constexpr flong ln10 = std::numbers::ln10;

      constexpr flong sqrt2 = std::numbers::sqrt2;
      constexpr flong inv_sqrt2 = 1 / sqrt2;
      constexpr flong sqrt3 = std::numbers::sqrt3;
      constexpr flong inv_sqrt3 = std::numbers::inv_sqrt3;
      
      constexpr flong pi = std::numbers::pi;
      constexpr flong tau = 2 * pi;
      constexpr flong pi_halves  = pi / 2;
      constexpr flong pi_thirds  = pi / 3;
      constexpr flong pi_fourths = pi / 4;
      constexpr flong pi_sixths  = pi / 6;
      constexpr flong inv_pi = std::numbers::inv_pi;
      constexpr flong inv_sqrt_pi = std::numbers::inv_sqrtpi;

      constexpr flong phi = std::numbers::phi;
      constexpr flong conj_phi = -1 / phi;
      constexpr flong eulmasch = std::numbers::egamma;

      constexpr flong lemniscate = 2.622057554292119810464839589891119413682754951431623162816821703/*...*/; //[source](https://oeis.org/A062539)
   };
};

#endif //MCSL_MATH_CONSTS_HPP