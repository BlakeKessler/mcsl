#ifndef MCSL_STRING_CPP
#define MCSL_STRING_CPP

#include "string.hpp"
#include "alloc.hpp"


//!concatenate other onto the end of this
mcsl::string& mcsl::string::operator+=(const str_t auto& other) {
   char* addr = end();
   const uint otherSize = other.size();
   reserve(size() + otherSize);
   //copy other
   memcpy(addr, other.begin(), otherSize);
   //return
   return self;
}
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