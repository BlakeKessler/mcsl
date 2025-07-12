#ifndef MCSL_HASH_HPP
#define MCSL_HASH_HPP

#include "MCSL.hpp"

namespace mcsl {
   namespace hash_algos {
      //rapidhash
      uint64 rapid(const void* key, uint len, uint64 seed = 0);
      uint64 rapidMicro(const void* key, uint len, uint64 seed = 0);
      uint64 rapidNano(const void* key, uint len, uint64 seed = 0);

      uint64 rapid_mix(uint64 lhs, uint64 rhs);

      //MurmurHash
      uint32 murmur32(const void* key, uint len, uint32 seed = 0);
      uint128 murmur128(const void* key, uint len, uint32 seed = 0);
      uint32 MurmurHash3_x86_32(const void* key, uint len, uint32 seed = 0);
      uint128 MurmurHash3_x86_128(const void* key, uint len, uint32 seed = 0);
      uint128 MurmurHash3_x64_128(const void* key, uint len, uint32 seed = 0);

      //ankerl hash
      uint64 ankerl(const void* key, uint len, uint64 seed = 0);

      //abseil hash
      uint64 absl(const void* key, uint len, uint64 seed = 0);

      //fnv hash
      uint32  fnv1_32(const void* key, uint len, uint32 seed = 0);
      uint64  fnv1_64(const void* key, uint len, uint64 seed = 0);
      uint128 fnv1_128(const void* key, uint len, uint128 seed = 0);
      uint32  fnv1a_32(const void* key, uint len, uint32 seed = 0);
      uint64  fnv1a_64(const void* key, uint len, uint64 seed = 0);
      uint128 fnv1a_128(const void* key, uint len, uint128 seed = 0);

      inline uint64 fnv1(const void* key, uint len, uint64 seed = 0) { return fnv1_64(key, len, seed); }
      inline uint64 fnv1a(const void* key, uint len, uint64 seed = 0) { return fnv1a_64(key, len, seed); }
   };
};

//default definition of mcsl::hash
#include <functional>
template<typename T> struct mcsl::hash {
   static uint64 operator()(const T& obj) {
      return std::hash<T>()(obj);
   }
   static uint64 operator()(const T& obj, uint64 seed) {
      return hash_algos::rapid_mix(operator()(obj), seed);
   }
};

//define mcsl::hash for fundamental types
#include <bit>
#include "MAP_MACRO.h"

namespace {
   using namespace mcsl;
   struct __inthash {
      using is_transparent = void;
      static inline uint64 operator()(uint64 n) {
         return operator()(n, RAPIDHASH_RHS_DEFAULT);
      }
      static inline uint64 operator()(uint64 n, uint64 seed) {
         return hash_algos::rapid_mix((uint64)n, seed);
      }
   };
   struct __floathash {
      using is_transparent = void;
      static inline uint64 operator()(float64 n) {
         return operator()(n, RAPIDHASH_RHS_DEFAULT);
      }
      static inline uint64 operator()(float64 n, uint64 seed) {
         return hash_algos::rapid_mix(std::bit_cast<uint64>(n), seed);
      }
   };
};

//ints
template<> struct mcsl::hash<uint64> {
   using is_transparent = void;
   static inline uint64 operator()(uint64 n) {
      return operator()(n, RAPIDHASH_RHS_DEFAULT);
   }
   static inline uint64 operator()(uint64 n, uint64 seed) {
      return hash_algos::rapid_mix((uint64)n, seed);
   }
};
#define __DEF_INT_HASH(T) template<> struct mcsl::hash<T> : public mcsl::hash<uint64> {};
MCSL_MAP(__DEF_INT_HASH, sint64, MCSL_LT_64_INT_T)
#undef __DEF_INT_HASH
template<> struct mcsl::hash<uint128> {
   using is_transparent = void;
   static inline uint64 operator()(uint128 n) {
      return ((uint64)n) ^ ((uint64)(n >> 64));
   }
   static inline uint64 operator()(uint128 n, uint128 seed) {
      return operator()(n ^ seed);
   }
};
template<> struct mcsl::hash<sint128> : public mcsl::hash<uint128> {};
static_assert(sizeof(mcsl::largest_int_t<MCSL_ALL_INT_T>) == sizeof(uint128));

//pointers
template<> struct mcsl::hash<void*> : public mcsl::hash<uptr> {
   static inline uint64 operator()(const void* n) {
      return operator()(n, RAPIDHASH_RHS_DEFAULT);
   }
   static inline uint64 operator()(const void* n, uint64 seed) {
      return hash_algos::rapid_mix((uptr)n, seed);
   }
};
template<typename T> struct mcsl::hash<T*> : public mcsl::hash<void*> {};
//floats
#define __DEF_FLOAT_HASH(T) template<> struct mcsl::hash<T> : public __floathash{};
MCSL_MAP(__DEF_FLOAT_HASH, MCSL_ALL_FLOAT_T)
#undef __DEF_FLOAT_HASH

#include "MAP_MACRO_UNDEF.h"

#endif //MCSL_HASH_HPP