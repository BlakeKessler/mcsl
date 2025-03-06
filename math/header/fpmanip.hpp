#pragma once
#ifndef MCSL_FPMANIP_HPP
#define MCSL_FPMANIP_HPP

#include "MCSL.hpp"

namespace mcsl {
   template<float_t T> inline T makeSigNaN(const ulong payload);
   template<float_t T> inline T makeQuietNaN(const ulong payload);

   //!TODO: the rest of the cmath floating point manipulation functions
};

#endif //MCSL_FPMANIP_HPP