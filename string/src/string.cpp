#ifndef MCSL_STRING_CPP
#define MCSL_STRING_CPP

#include "string.hpp"
#include "alloc.hpp"


//!repeat string
mcsl::string& mcsl::string::operator*=(const uint repeatCount) {
   const uint oldSize = size();
   const uint newSize = repeatCount * oldSize;
   reserve_exact(newSize);
   _buf._size = newSize;
   for (uint pos = oldSize; pos < newSize; pos+=oldSize) {
      memcpy(begin() + pos, begin(), oldSize);
   }
   return self;
}

#endif //MCSL_STRING_CPP