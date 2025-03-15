#pragma once
#ifndef MCSL_HW_HPP
#define MCSL_HW_HPP

#include "MCSL.hpp"

#include <stdbit.h>

namespace mcsl {
   enum class sys_endian : ubyte {
      LITTLE,
      BIG,

      #if __STDC_ENDIAN_NATIVE__ == __STDC_ENDIAN_BIG__
      SYS = BIG,
      ANTI_SYS = LITTLE,
      #else
      SYS = LITTLE,
      ANTI_SYS = BIG,
      #endif
   };
};

#endif //MCSL_HW_HPP