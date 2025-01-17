#ifndef MCSL_ASSERT_HPP
#define MCSL_ASSERT_HPP

#include "MCSL.hpp"

//!NOTE: maybe rewrite using [std::source_location](https://en.cppreference.com/w/cpp/utility/source_location)

#ifndef assert
   #define assert(expr) (static_cast<bool>(expr) ? void(0) : mcsl::__assert_fail(__FILE__, __LINE__, __func__, #expr))
#endif

#ifndef debug_assert
   #ifndef NDEBUG
      #define debug_assert(expr) assert(expr)
      #define UNREACHABLE mcsl::__unreachable(__FILE__, __LINE__, __func__)
   #else
      #define debug_assert(expr) void(0)
      #define UNREACHABLE std::unreachable()
      #include <utility>
   #endif
#endif

namespace mcsl {
   void __assert_fail(const char* file, const uint line, const char* func, const char* exprStr);
   void __unreachable(const char* file, const uint line, const char* func);
};

#endif //MCSL_ASSERT_HPP