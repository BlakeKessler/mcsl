#pragma once
#ifndef MCSL_DEFINES_HPP
#define MCSL_DEFINES_HPP

#define MCSL

#include <cstdint>
// #include <stdfloat>
#include <utility>

//self
#define self (*this)

//!FIXED WIDTH INTEGER TYPES
using uint8   = uint8_t;
using uint16  = uint16_t;
using uint32  = uint32_t;
using uint64  = uint64_t;
using uint128 = __uint128_t;

using sint8   = int8_t;
using sint16  = int16_t;
using sint32  = int32_t;
using sint64  = int64_t;
using sint128 = __int128_t;

//!ARCHITECTURE-BASED INTEGER TYPES

using ubyte     = uint8;
using ushort    = uint16;
using uint      = uint32;
using ulong     = uint64;
using uoverlong = uint128; //must be large enough to hold the full result of long multiplication

using uptr  = uintptr_t;
using uword = uint64;

using sbyte     = sint8;
using sshort    = sint16;
using sint      = sint32;
using slong     = sint64;
using soverlong = sint128; //must be large enough to hold the full result of long multiplication

using sptr  = intptr_t;
using sword = slong;

//!FIXED-WIDTH FLOATING-POINT TYPES
// using float16  = std::float16_t;
// using float32  = std::float32_t;
// using float64  = std::float64_t;
// using float128 = std::float128_t;

// using half   = float16;
// using single = float32;
// using double = float64;
// using quad   = float128;

// using fshort = float16;
// using float  = float32;
// using flong  = float64;

//enums
namespace mcsl {
   enum class ErrCode {
      UNSPEC = 0,
      ASSERT_FAIL,
      UNREACHABLE_REACHED,
      SEGFAULT,
      ALLOC_FAIL,
      INT_OVERFLOW,

      STRTOINT,
   };
   constexpr auto operator+(const ErrCode code) { return std::to_underlying(code); }
}

//simple integer macros
#define sign(num) ((num > 0) - (num < 0))
#define signofdif(a,b) ((a > b) - (a < b))
#define absdif(a,b) ((a>b)?a-b:b-a)

//numeric constants
constexpr const float Inf = __builtin_inff();
constexpr const float NaN = __builtin_nanf("");
constexpr const float SigNaN = __builtin_nansf("");

constexpr const uint DEBUG_INT = 0x7ff01954;
// constexpr const uint DEBUG_INT = 0x7fa00000;
// constexpr const uint DEBUG_INT = 0xDEADBEEF;
// constexpr const uint DEBUG_INT = 0xf055f055;

// //comparison macros
// #define cmpfunc(T) bool cmp(const T& a, const T& b)

//container parameters
constexpr const uint DEFAULT_BUCKET_SIZE = 4;





//bitmask literal
constexpr unsigned long long operator"" _m(const unsigned long long x) {
   return x ? 1LU << (x - 1) : 0;
}

#endif //MCSL_DEFINES_HPP