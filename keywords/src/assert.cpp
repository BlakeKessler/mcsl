#ifndef MCSL_ASSERT_CPP
#define MCSL_ASSERT_CPP

#include "assert.hpp"
#include "throw.hpp"

void mcsl::__assert_fail(const char* msg, const ErrCode code, const std::source_location loc) {
   mcsl::__throw(code, mcsl::FMT("%s:%u(%u): %s: `%s`"), loc.file_name(), loc.line(), loc.column(), loc.function_name(), msg);
}

#endif //MCSL_ASSERT_CPP