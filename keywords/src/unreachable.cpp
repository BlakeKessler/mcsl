#ifndef MCSL_UNREACHABLE_CPP
#define MCSL_UNREACHABLE_CPP

#include "unreachable.hpp"

#include "throw.hpp"

[[noreturn]] void mcsl::__unreachable(const std::source_location loc) {
   mcsl::__throw(ErrCode::UNREACHABLE_REACHED, mcsl::FMT("%s:%u(%u): %s"), loc.file_name(), loc.line(), loc.column(), loc.function_name());
}

#endif //MCSL_UNREACHABLE_CPP