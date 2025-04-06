#ifndef MCSL_UNREACHABLE_CPP
#define MCSL_UNREACHABLE_CPP

#include "unreachable.hpp"

#include "throw.hpp"
#include "halt.hpp"
#include "io.hpp"

[[noreturn]] void mcsl::__unreachable(const std::source_location loc) {
   static bool hasReached = false;
   if (!hasReached) { //to prevent accidental infinite recursion if something goes wrong with the IO code
      hasReached = true;
      mcsl::flush();
   }
   mcsl::__throw(ErrCode::UNREACHABLE_REACHED, mcsl::FMT("%s:%u(%u): %s"), FMT(loc.file_name()), loc.line(), loc.column(), FMT(loc.function_name()));
   std::unreachable();
}

[[noreturn]] void mcsl::__todo(const std::source_location loc) {
   mcsl::flush();
   mcsl::err_printf(mcsl::FMT("\n\033[1;4;36mMCSL TODO:\033[22;24;39m %s:%u(%u): %s\n"), FMT(loc.file_name()), loc.line(), loc.column(), FMT(loc.function_name()));
   mcsl::exit(EXIT_FAILURE);
}

#endif //MCSL_UNREACHABLE_CPP