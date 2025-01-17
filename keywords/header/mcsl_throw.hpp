#pragma once
#ifndef MCSL_ERR_HPP
#define MCSL_ERR_HPP

#include "MCSL.hpp"
#include <stdexcept>
#include <cstdio>
#ifdef MCSL
   #undef NULL
#endif 

namespace mcsl {
   extern const char* ERR_MSG_ARR[];
   [[noreturn, gnu::format(printf,2,3)]] void mcsl_throw(const ErrCode code, const char* formatStr, auto&&... args);
   [[noreturn, gnu::format(printf,3,4)]] void mcsl_throw(const ErrCode code, const uint lineNum, const char* formatStr, auto&&... args);
}



#pragma region src
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
//!MCSL formatted error thrower
[[noreturn, gnu::format(printf,2,3)]] void mcsl::mcsl_throw(const ErrCode code, const char* formatStr, auto&&... args) {
   std::fprintf(stderr, "\033[31;1;4mMCSL ERROR:\033[0m %s", ERR_MSG_ARR[+code]);
   std::fprintf(stderr, formatStr, std::forward<decltype(args)>(args)...);
   std::fprintf(stderr, "\n");
   std::abort();
   // std::exit(EXIT_FAILURE);
}
//!MCSL formatted error thrower with line num
[[noreturn, gnu::format(printf,3,4)]] void mcsl::mcsl_throw(const ErrCode code, const uint lineNum, const char* formatStr, auto&&... args) {
   std::fprintf(stderr, "\033[31;1;4mMCSL ERROR:\033[0m %s", ERR_MSG_ARR[+code]);
   std::fprintf(stderr, formatStr, std::forward<decltype(args)>(args)...);
   std::fprintf(stderr, " \033[35m(line %u)\033[0m\n", lineNum);
   std::abort();
   // std::exit(EXIT_FAILURE);
}
#pragma GCC diagnostic pop
#pragma endregion src

#endif //MCSL_ERR_HPP