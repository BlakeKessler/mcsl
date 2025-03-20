#pragma once
#ifndef MCSL_MEM_HPP
#define MCSL_MEM_HPP

#include "MCSL.hpp"

#include <cstring>
#undef NULL

namespace mcsl {
   constexpr void memcpy(ubyte* dest, const ubyte* src, const uint len) {
      if consteval {
         for (uint i = 0; i < len; ++i) {
            dest[i] = src[i];
         }
      } else {
         std::memcpy(dest, src, len);
      }
   }
   constexpr void memcpy(char* dest, const char* src, const uint len) {
      if consteval {
         for (uint i = 0; i < len; ++i) {
            dest[i] = src[i];
         }
      } else {
         std::memcpy(dest, src, len);
      }
   }
   constexpr uint cstrcpy(char* dest, const char* src, const uint capacity) {
      for (uint i = 0; i < capacity; ++i) {
         if (!src[i]) {
            return i;
         }
         dest[i] = src[i];
      }
      return capacity;
   }
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
   constexpr sint memcmp(const char* lhs, const char* rhs, const uint len) {
      if consteval {
         const char* lhsEnd = lhs + len;
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
   constexpr sint cstrcmp(const char* lhs, const char* rhs, const uint len) {
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

   constexpr uint cstrlen(const char* str, const uint capacity) {
      for (uint i = 0; i < capacity; ++i) {
         if (!str[i]) {
            return i;
         }
      }
      return capacity;
   }
};

#endif //MCSL_MEM_HPP