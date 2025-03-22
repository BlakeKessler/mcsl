#ifndef MCSL_UNREACHABLE_HPP
#define MCSL_UNREACHABLE_HPP

#include "MCSL.hpp"

#include <source_location>

#ifndef NDEBUG
   #define UNREACHABLE mcsl::__unreachable()
   #define TODO mcsl::__todo()
#else
   #include <utility>
   #define UNREACHABLE std::unreachable()
   #define TODO UNREACHABLE
#endif

namespace mcsl {
   [[noreturn]] void __unreachable(const std::source_location loc = std::source_location::current());
   [[noreturn]] void __todo(const std::source_location loc = std::source_location::current());
};

#endif //MCSL_UNREACHABLE_HPP