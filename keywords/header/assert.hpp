#ifndef MCSL_ASSERT_HPP
#define MCSL_ASSERT_HPP

#include "MCSL.hpp"

#include "unreachable.hpp"
#include <source_location>

namespace mcsl {
   [[noreturn]] void __assert_fail(const char* msg, const ErrCode code = ErrCode::UNSPEC, const std::source_location loc = std::source_location::current());

   constexpr void __assert(const bool expr, const char* msg = "", const mcsl::ErrCode code = mcsl::ErrCode::UNSPEC, const std::source_location loc = std::source_location::current()) {
      if (!expr) { [[unlikely]]
         mcsl::__assert_fail(msg, code, loc);
      }
   }
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wunused-parameter"
   constexpr void __assert(const bool expr, const char* msg, const mcsl::ErrCode code, const char* dummy) { if (!expr) { UNREACHABLE; } }
   constexpr void __assert(const bool expr, const char* msg, const char* dummy1, const char* dummy2 = "") { if (!expr) { UNREACHABLE; } }
   #pragma GCC diagnostic pop
};

#define assert(...)\
   if constexpr (__VA_ARG_COUNT__(__VA_ARGS__) == 1) { mcsl::__assert(__VA_ARGS__ __VA_OPT__(,) #__VA_ARGS__); }\
   else { mcsl::__assert(__VA_ARGS__); }\
   void(0)

#ifndef NDEBUG
   #define debug_assert(expr) assert(expr, #expr, mcsl::ErrCode::DEBUG_ASSERT_FAIL)
#else
   #define debug_assert(expr) void(0)
#endif

#if defined(SAFE_MODE) || !defined(NDEBUG)
   #define safe_mode_assert(expr) assert(expr, #expr, mcsl::ErrCode::SAFE_ASSERT_FAIL)
#else
   #define safe_mode_assert(expr) void(0)
#endif


#endif //MCSL_ASSERT_HPP