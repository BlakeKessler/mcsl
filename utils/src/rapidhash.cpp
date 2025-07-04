/*
 * MCSL implementation of rapidhash V3
 * implementation written by Blake Kessler
 * algorithm released by Nicolas De Cartli under the MIT license
 * 
 * Copyright (C) 2025 Nicolas De Carli
 * MIT License
 * 
 * Additional modifications:
 * Copyright (C) 2025 Blake Kessler
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * original rapidhash source repository: https://github.com/Nicoshev/rapidhash
 */

#ifndef MCSL_RAPIDHASH_CPP
#define MCSL_RAPIDHASH_CPP

#include "hash.hpp"

#include "hw.hpp"
#include <bit>

namespace { //anonymous namespace with MCSL implementations of rapidhash helper functions
   constexpr uint64 RAPID_SECRET[] = {
      0x2d358dccaa6c78a5ull,
      0x8bb84b93962eacc9ull,
      0x4b33a62ed433d4a3ull,
      0x4d5a2da51de1aa47ull,
      0xa0761d6478bd642full,
      0xe7037ed1a0b428dbull,
      0x90ed1765281c388cull,
      0xaaaaaaaaaaaaaaaaull
   };

   //rapid_mum replaced by use of uint128 (more obvious meaning and equal or superior codegen)

   //mix 64-bit integers
   [[gnu::always_inline]] inline constexpr uint64 rapid_mix(uint64 lhs, uint64 rhs) noexcept {
      uint128 prod = (uint128)(lhs) * (uint128)(rhs);
      uint64* ptr = std::bit_cast<uint64*>(&prod);
      return ptr[0] ^ ptr[1];
   }

   //read a little-endian 64-bit integer
   [[gnu::always_inline]] inline constexpr uint64 rapid_read64(const uint8* ptr) noexcept {
      uint64 val = *(std::bit_cast<uint64*>(ptr));
      if constexpr (mcsl::sys_endian::SYS == mcsl::sys_endian::LITTLE) {
         return val;
      } else {
         return mcsl::byteswap(val);
      }
   }
   //read a little-endian 32-bit integer
   [[gnu::always_inline]] inline constexpr uint64 rapid_read32(const uint8* ptr) noexcept {
      uint32 val = *(std::bit_cast<uint32*>(ptr));
      if constexpr (mcsl::sys_endian::SYS == mcsl::sys_endian::LITTLE) {
         return val;
      } else {
         return mcsl::byteswap(val);
      }
   }
};

//MCSL implementation of rapidhash_internal with RAPIDHASH_COMPACT and RAPIDHASH_FAST defined
/*inline constexpr*/ uint64 mcsl::hash_algos::rapid(const void* key, uint len, uint64 seed) {
   const uint8* curr = (const uint8*)key;
   seed ^= rapid_mix(seed ^ RAPID_SECRET[2], RAPID_SECRET[1]);
   uint64 a = 0;
   uint64 b = 0;
   uint i = len;
   if (len <= 16) { [[likely]]; //short keys
      if (len >= 4) {
         seed ^= len;
         if (len >= 8) {
            a = rapid_read64(curr);
            b = rapid_read64(curr + len - 8);
         } else {
            a = rapid_read32(curr);
            b = rapid_read32(curr + len - 4);
         }
      } else if (len > 0) {
         a = (((uint64)curr[0]) << 45) | curr[len - 1];
         b = curr[len >> 1];
      }
   } else { //long keys
      uint64 seed1 = seed;
      uint64 seed2 = seed;
      uint64 seed3 = seed;
      uint64 seed4 = seed;
      uint64 seed5 = seed;
      uint64 seed6 = seed;
      if (i > 112) {
         #define seed0 seed
         #define MIX(n) seed##n = rapid_mix(rapid_read64(curr + (n * 16)) ^ RAPID_SECRET[n], rapid_read64(curr + (n * 16 + 8)) ^ seed )
         do {
            MIX(0); // seed  = rapid_mix(rapid_read64(curr     ) ^ RAPID_SECRET[0], rapid_read64(curr + 8  ) ^ seed );
            MIX(1); // seed1 = rapid_mix(rapid_read64(curr + 16) ^ RAPID_SECRET[1], rapid_read64(curr + 24 ) ^ seed1);
            MIX(2); // seed2 = rapid_mix(rapid_read64(curr + 32) ^ RAPID_SECRET[2], rapid_read64(curr + 40 ) ^ seed2);
            MIX(3); // seed3 = rapid_mix(rapid_read64(curr + 48) ^ RAPID_SECRET[3], rapid_read64(curr + 56 ) ^ seed3);
            MIX(4); // seed4 = rapid_mix(rapid_read64(curr + 64) ^ RAPID_SECRET[4], rapid_read64(curr + 72 ) ^ seed4);
            MIX(5); // seed5 = rapid_mix(rapid_read64(curr + 80) ^ RAPID_SECRET[5], rapid_read64(curr + 88 ) ^ seed5);
            MIX(6); // seed6 = rapid_mix(rapid_read64(curr + 96) ^ RAPID_SECRET[6], rapid_read64(curr + 104) ^ seed6);
            curr += 112;
            i -= 112;
         } while (i > 112);
         #undef MIX
         #undef seed0
         seed  ^= seed1;
         seed2 ^= seed3;
         seed4 ^= seed5;
         seed  ^= seed6;
         seed2 ^= seed4;
         seed  ^= seed2;
      }

      #define ITERATION(n, expr) \
      if (i > (16 * (n + 1))) { \
         seed = rapid_mix(rapid_read64(curr) ^ RAPID_SECRET[2 - ((n >> 1) & 1)], rapid_read64(curr + (16 * n + 8)) ^ seed); \
         expr \
      }
      ITERATION(0, ITERATION(1, ITERATION(2, ITERATION(3, ITERATION(4, ITERATION(5,))))))
      #undef ITERATION
      // if (i > 16) {
      //    seed = rapid_mix(rapid_read64(curr) ^ RAPID_SECRET[2], rapid_read64(curr + 8) ^ seed);
      //    if (i > 32) {
      //       seed = rapid_mix(rapid_read64(curr + 16) ^ RAPID_SECRET[2], rapid_read64(curr + 24) ^ seed);
      //       if (i > 48) {
      //          seed = rapid_mix(rapid_read64(curr + 32) ^ RAPID_SECRET[1], rapid_read64(curr + 40) ^ seed);
      //          if (i > 64) {
      //             seed = rapid_mix(rapid_read64(curr + 48) ^ RAPID_SECRET[1], rapid_read64(curr + 56) ^ seed);
      //             if (i > 80) {
      //                seed = rapid_mix(rapid_read64(curr + 64) ^ RAPID_SECRET[2], rapid_read64(curr + 72) ^ seed);
      //                if (i > 96) {
      //                   seed = rapid_mix(rapid_read64(curr + 80) ^ RAPID_SECRET[1], rapid_read64(curr + 88) ^ seed);
      //                }
      //             }
      //          }
      //       }
      //    }
      // }

      a = rapid_read64(curr + i - 16) ^ i;
      b = rapid_read64(curr + i - 8);
   }
   a ^= RAPID_SECRET[1];
   b ^= seed;
   uint128 prod = (uint128)(a) * (uint128)(b);
   uint64* ptr = std::bit_cast<uint64*>(&prod);
   a = ptr[0];
   b = ptr[1];
   return rapid_mix(a ^ RAPID_SECRET[7], b ^ RAPID_SECRET[1] ^ i);
}

#endif //MCSL_RAPIDHASH_CPP