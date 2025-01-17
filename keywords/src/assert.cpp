#ifndef MCSL_ASSERT_CPP
#define MCSL_ASSERT_CPP

#include "assert.hpp"
#include "mcsl_throw.hpp"

void mcsl::__assert_fail(const char* file, const uint line, const char* func, const char* exprStr) {
   mcsl::mcsl_throw(ErrCode::ASSERT_FAIL, "%s:%u: %s: `%s`", file, line, func, exprStr);
   // mcsl::mcsl_throw(ErrCode::ASSERT_FAIL, "%s:%u: %s: Assertion `%s` failed.", file, line, func, exprStr);
}
void mcsl::__unreachable(const char* file, const uint line, const char* func) {
   mcsl::mcsl_throw(ErrCode::UNREACHABLE_REACHED, "%s:%u: %s", file, line, func);
   // mcsl::mcsl_throw(ErrCode::UNREACHABLE_REACHED, "%s:%u: %s: Reached unreachable statement. Aborting.", file, line, func);
}

#endif //MCSL_ASSERT_CPP