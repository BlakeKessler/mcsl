#ifndef MCSL_UNREACHABLE_CPP
#define MCSL_UNREACHABLE_CPP

#include "unreachable.hpp"

#include "throw.hpp"

[[noreturn]] void mcsl::__unreachable(const std::source_location loc) {
   mcsl::__throw(ErrCode::UNREACHABLE_REACHED, mcsl::FMT("%s:%u(%u): %s"), FMT(loc.file_name()), loc.line(), loc.column(), FMT(loc.function_name()));
   std::unreachable();
}

[[noreturn]] void mcsl::__todo(const std::source_location loc) {
   mcsl::__throw(ErrCode::TODO_CODE, mcsl::FMT("%s:%u(%u): %s"), FMT(loc.file_name()), loc.line(), loc.column(), FMT(loc.function_name()));
   std::unreachable();
}

#endif //MCSL_UNREACHABLE_CPP