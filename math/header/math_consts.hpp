#pragma once
#ifndef MCSL_MATH_CONSTS_HPP
#define MCSL_MATH_CONSTS_HPP

#include "MCSL.hpp"

#include <numbers>

namespace mcsl{
   namespace nums {
      constexpr double e = std::numbers::e;
      constexpr double log2e = std::numbers::log2e;
      constexpr double log10e = std::numbers::log10e;

      constexpr double ln2 = std::numbers::ln2;
      constexpr double ln10 = std::numbers::ln10;

      constexpr double sqrt2 = std::numbers::sqrt2;
      constexpr double inv_sqrt2 = 1 / sqrt2;
      constexpr double sqrt3 = std::numbers::sqrt3;
      constexpr double inv_sqrt3 = std::numbers::inv_sqrt3;
      
      constexpr double pi = std::numbers::pi;
      constexpr double tau = 2 * pi;
      constexpr double pi_halves  = pi / 2;
      constexpr double pi_thirds  = pi / 3;
      constexpr double pi_fourths = pi / 4;
      constexpr double pi_sixths  = pi / 6;
      constexpr double inv_pi = std::numbers::inv_pi;
      constexpr double inv_sqrt_pi = std::numbers::inv_sqrtpi;

      constexpr double phi = std::numbers::phi;
      constexpr double conj_phi = -1 / phi;
      constexpr double eulmasch = std::numbers::egamma;

      constexpr double lemniscate = 2.622057554292119810464839589891119413682754951431623162816821703/*...*/; //[source](https://oeis.org/A062539)
   };
};

#endif //MCSL_MATH_CONSTS_HPP