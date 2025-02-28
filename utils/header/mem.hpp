#pragma once
#ifndef MCSL_MEM_HPP
#define MCSL_MEM_HPP

#include "MCSL.hpp"

#include <cstring>

namespace mcsl {
   constexpr sint memcmp(const ubyte* lhs, const ubyte* rhs, const uint len) {
      if consteval {
         const ubyte* lhsEnd = lhs + len;
         while (lhs != lhsEnd) {
            if (*lhs != *rhs) {
               return *lhs - *rhs;
            }
            ++lhs;
            ++rhs;
         }
         return 0;
      } else {
         return std::memcmp(lhs, rhs, len);
      }
   }
   constexpr sint strcmp(const char* lhs, const char* rhs, const uint len) {
      if consteval {
         const char* lhsEnd = lhs + len;
         while (lhs != lhsEnd) {
            if (*lhs != *rhs || !*lhs || !*rhs) {
               return *lhs - *rhs;
            }
            ++lhs;
            ++rhs;
         }
         return 0;
      } else {
         return std::strncmp(lhs, rhs, len);
      }
   }
};

#endif //MCSL_MEM_HPP