#ifndef MCSL_HALT_CPP
#define MCSL_HALT_CPP

#include "halt.hpp"

#include <cstdlib>
#include <exception>

[[noreturn]] void mcsl::exit(sint status) {
   std::exit(status);
}
[[noreturn]] void mcsl::quick_exit(sint status) {
   std::quick_exit(status);
}

[[noreturn]] void mcsl::terminate() {
   std::terminate();
}
[[noreturn]] void mcsl::abort() {
   std::abort();
}

#endif //MCSL_HALT_CPP