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

   inline uint16 byteswap(uint16 n) { return __builtin_bswap16(n); }
   inline uint32 byteswap(uint32 n) { return __builtin_bswap32(n); }
   inline uint64 byteswap(uint64 n) { return __builtin_bswap64(n); }
   // inline uint128 byteswap(uint128 n) { return __builtin_bswap128(n); }
};

#endif //MCSL_HW_HPP