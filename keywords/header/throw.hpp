#pragma once
#ifndef MCSL_ERR_HPP
#define MCSL_ERR_HPP

#include "MCSL.hpp"
#include <stdexcept>
#include "io.hpp"
#include "str_slice.hpp"

namespace mcsl {
   extern const mcsl::str_slice ERR_MSG_ARR[];
   void __throw(const ErrCode code, const mcsl::str_slice formatStr, auto&&... args);
   void __throw(const ErrCode code, const uint lineNum, const mcsl::str_slice formatStr, auto&&... args);
}



#pragma region src
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
//!MCSL formatted error thrower
void mcsl::__throw(const ErrCode code, const mcsl::str_slice formatStr, auto&&... args) {
   mcsl::err_printf(FMT("\033[31;1;4mMCSL ERROR:\033[0m %s"), ERR_MSG_ARR[+code]);
   mcsl::err_printf(formatStr, std::forward<decltype(args)>(args)...);
   mcsl::err_printf(FMT("\n"));
   std::abort();
   // std::exit(EXIT_FAILURE);
}
//!MCSL formatted error thrower with line num
void mcsl::__throw(const ErrCode code, const uint lineNum, const mcsl::str_slice formatStr, auto&&... args) {
   mcsl::err_printf(FMT("\033[31;1;4mMCSL ERROR:\033[0m %s"), ERR_MSG_ARR[+code]);
   mcsl::err_printf(formatStr, std::forward<decltype(args)>(args)...);
   mcsl::err_printf(FMT(" \033[35m(line %u)\033[0m\n"), lineNum);
   std::abort();
   // std::exit(EXIT_FAILURE);
}
#pragma GCC diagnostic pop
#pragma endregion src

#endif //MCSL_ERR_HPP