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

#endif //MCSL_HASH_HPP