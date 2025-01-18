#ifndef MCSL_ASSERT_CPP
#define MCSL_ASSERT_CPP

#include "assert.hpp"
#include "mcsl_throw.hpp"

void mcsl::__assert_fail(const char* msg, const std::source_location loc) {
   mcsl::mcsl_throw(mcsl::ErrCode::ASSERT_FAIL, "%s:%u(%u): %s: `%s`", loc.file_name(), loc.line(), loc.column(), loc.function_name(), msg);
}

#endif //MCSL_ASSERT_CPP