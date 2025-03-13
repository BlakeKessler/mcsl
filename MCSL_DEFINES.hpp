#pragma once
#ifndef MCSL_DEFINES_HPP
#define MCSL_DEFINES_HPP

#define MCSL

#include <cstdint>
#include <stdfloat>
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
using float32  = float;
using float64  = double;
// using float80  = long double;
// using float128 = std::float128_t;

// using half   = float16;
using single = float32;
// using double = float64;
// using quad   = float128;

//!ARCHITECTURE-BASED FLOAT TYPES

// using fshort = float16;
// using float  = float32;
using flong  = float64;
using flext  = long double;
// using flext  = float80;

//!FIXED-WIDTH CHAR TYPES

// using char = char;       //ASCII (NOT UTF-8)
using char8  = char8_t;  //UTF-8
using char16 = char16_t; //UTF-16
using char32 = char32_t; //UTF-32

//!ARCHITECTURE-BASED CHAR TYPES

using wchar = wchar_t;

//enums
namespace mcsl {
   enum class ErrCode {
      UNSPEC = 0,
      ASSERT_FAIL,
      DEBUG_ASSERT_FAIL,
      ASSUMPTION_FAIL,
      UNREACHABLE_REACHED,
      SEGFAULT,
      ALLOC_FAIL,
      INT_OVERFLOW,

      STRTOINT,

      FS_ERR,
   };
   constexpr auto operator+(const ErrCode code) { return std::to_underlying(code); }
}

//simple integer macros
#define sign(num) ((num > 0) - (num < 0))
#define signofdif(a,b) ((a > b) - (a < b))
#define absdif(a,b) ((a>b)?a-b:b-a)

namespace mcsl {
   //numeric constants
   constexpr const float Inf = __builtin_inff();
   constexpr const float NaN = __builtin_nanf("");
   constexpr const float SigNaN = __builtin_nansf("");

   constexpr const uint DEBUG_INT = 0x7ff01954;
   // constexpr const uint DEBUG_INT = 0x7fa00000;
   // constexpr const uint DEBUG_INT = 0xDEADBEEF;
   // constexpr const uint DEBUG_INT = 0xf055f055;

   //container parameters
   constexpr const uint DEFAULT_BUCKET_SIZE = 4;
   constexpr const uint DEFAULT_ARR_LIST_BUF_SIZE = 16;


   //IO parameters
   constexpr uint DEFAULT_INT_RADIX = 10;
   constexpr uint DEFAULT_FLOAT_RADIX = 10;
   constexpr uint DEFAULT_RAW_RADIX = 16;
   constexpr uint8 CASE_BIT = 'a' ^ 'A';
   constexpr char PAD_CHAR = ' ';
   constexpr char EXP_NOTAT[] = "~^";
   // constexpr char EXP_NOTAT[] = "*^";
   constexpr char FMT_INTRO = '%';
};

//bitmask literal
constexpr unsigned long long operator"" _m(const unsigned long long x) {
   return x ? 1LU << (x - 1) : 0;
}



//variadic macro argument count
#include <tuple>
#define __VA_ARG_COUNT__(...) std::tuple_size_v<decltype(std::forward_as_tuple(__VA_ARGS__))>

//select first element of paramater pack
namespace mcsl {
   template<typename T, typename... Ts> T& pack_first(T& first, Ts&... rest) { return first; }
};


//RNG parameters
//sourced from https://nuclear.llnl.gov/CNP/rng/rngman/node4.html

#define MCSL_LCG64_MULT 2862933555777941757ul
#define MCSL_LCG64_INC 3037000493ul
#define MCSL_LCG64_MIN 0ul
#define MCSL_LCG64_MAX 0ul
#define MCSL_LCG64_SEED 1ul


#endif //MCSL_DEFINES_HPP