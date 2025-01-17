#ifndef MCSL_ERR_CPP
#define MCSL_ERR_CPP

#include "mcsl_throw.hpp"

//!array of MCSL error message prefix strings
const char* mcsl::ERR_MSG_ARR[] {
   "UNSPECIFIED ERROR: ",
   "Assertion failed: ",
   "Unreachable statement reached: ",
   "Segmentation fault: ",
   "Allocation failure: ",
   "Integer Overflow: ",
   "String to Integer Parsing Failure: "
};

#endif //MCSL_ERR_CPP