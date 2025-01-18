#ifndef MCSL_ASSERT_HPP
#define MCSL_ASSERT_HPP

#include "MCSL.hpp"

#include <source_location>

namespace mcsl {
   [[noreturn]] void __assert_fail(const char* msg, const std::source_location loc = std::source_location::current());
};
constexpr void assert(const bool expr, const char* msg, const std::source_location loc = std::source_location::current()) {
   if (!expr) { [[unlikely]]
      mcsl::__assert_fail(msg, loc);
   }
}

#ifndef debug_assert
   #ifndef NDEBUG
      #define debug_assert(expr) assert(expr, #expr)
   #else
      #define debug_assert(expr) void(0)
   #endif
#endif

#endif //MCSL_ASSERT_HPP