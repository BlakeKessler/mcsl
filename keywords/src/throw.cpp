#ifndef MCSL_ERR_CPP
#define MCSL_ERR_CPP

#include "throw.hpp"

#include "str_slice.hpp"

//!array of MCSL error message prefix strings
const mcsl::str_slice mcsl::ERR_MSG_ARR[] {
   FMT("UNSPECIFIED ERROR: "),
   FMT("Assertion failed: "),
   FMT("Debug Assertion failed: "),
   FMT("Assumption violated: "),
   FMT("Unreachable statement reached: "),
   FMT("Segmentation fault: "),
   FMT("Allocation failure: "),
   FMT("Integer Overflow: "),
   FMT("String to Integer Parsing Failure: "),
   FMT("FileSys Error: ")
};

#endif //MCSL_ERR_CPP