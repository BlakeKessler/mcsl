#ifndef MCSL_FREE_CPP
#define MCSL_FREE_CPP

#include "alloc.hpp"

#include <cstdlib>

//!deallocate dynamically allocated memory
void mcsl::free(void* ptr) {
   std::free(ptr);
}

#endif //MCSL_FREE_CPP