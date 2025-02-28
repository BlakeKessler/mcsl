#ifndef MCSL_STRING_CPP
#define MCSL_STRING_CPP

#include "string.hpp"
#include "alloc.hpp"
#include <cstring>


//!concatenate other onto the end of this
mcsl::string& mcsl::string::operator+=(const str_t auto& other) {
   char* addr = end();
   reserve(size() + other.size());
   //copy other
   std::memcpy(addr, other.begin(), other.size()*sizeof(char));
   //return
   return self;
}
//!repeat string
mcsl::string& mcsl::string::operator*=(const uint repeatCount) {
   const uint oldSize = size();
   reserve_exact(repeatCount * size());
   for (uint pos = oldSize; pos < size(); pos+=oldSize) {
      std::memcpy(begin() + pos, begin(), oldSize*sizeof(char));
   }
   return self;
}

#endif //MCSL_STRING_CPP