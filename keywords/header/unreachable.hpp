#ifndef MCSL_ASSERT_HPP
#define MCSL_ASSERT_HPP

#include "MCSL.hpp"

#include <source_location>

#ifndef NDEBUG
   #define UNREACHABLE mcsl::__unreachable()
#else
   #include <utility>
   #define UNREACHABLE std::unreachable()
#endif

namespace mcsl {
   [[noreturn]] void __unreachable(const std::source_location loc = std::source_location::current());
};

#endif //MCSL_ASSERT_HPP