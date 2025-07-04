#ifndef MCSL_HASH_CPP
#define MCSL_HASH_CPP

#include "hash.hpp"

#pragma region fnv
#define DEF_FNV(bits) \
uint##bits mcsl::hash_algos::fnv1_##bits(const void* key, uint len, uint##bits seed) {  \
   const uint8* const buf = (const uint8*)key;                                          \
   uint##bits hash = (FNV_OFFSET_##bits * FNV_PRIME_##bits) ^ seed;                     \
   while (len--) {                                                                      \
      hash *= FNV_PRIME_##bits;                                                         \
      hash ^= buf[len];                                                                 \
   }                                                                                    \
   return hash;                                                                         \
}                                                                                       \
uint##bits mcsl::hash_algos::fnv1a_##bits(const void* key, uint len, uint##bits seed) { \
   const uint8* const buf = (const uint8*)key;                                          \
   uint##bits hash = (FNV_OFFSET_##bits * FNV_PRIME_##bits) ^ seed;                     \
   while (len--) {                                                                      \
      hash ^= buf[len];                                                                 \
      hash *= FNV_PRIME_##bits;                                                         \
   }                                                                                    \
   return hash;                                                                         \
}
DEF_FNV(32)
DEF_FNV(64)
DEF_FNV(128)
#undef DEF_FNV
#pragma endregion fnv

#endif //MCSL_HASH_CPP