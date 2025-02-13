#pragma once
#ifndef FMANIP_MCSL
#define FMANIP_MCSL

#include "MCSL.hpp"

namespace mcsl {
   template<float_t T> inline T makeSigNaN(const ulong payload);
   template<float_t T> inline T makeQuietNaN(const ulong payload);

   //!TODO: the rest of the cmath floating point manipulation functions
};

#endif