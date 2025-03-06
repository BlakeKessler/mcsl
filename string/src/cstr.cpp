#ifndef MCSL_CSTR_CPP
#define MCSL_CSTR_CPP

#include "cstr.hpp"
#include "alloc.hpp"


//!concatenate other onto the end of this
mcsl::cstr& mcsl::cstr::operator+=(const str_t auto& other) {
   const uint len = size();
   char* addr = begin() + len;
   const uint otherSize = other.size();
   reserve(len + otherSize);
   //copy other
   memcpy(addr, other.begin(), otherSize);
   //return
   return self;
}
//!repeat cstr
mcsl::cstr& mcsl::cstr::operator*=(const uint repeatCount) {
   const uint oldSize = size();
   const uint newSize = repeatCount * oldSize;
   reserve_exact(newSize);
   _buf._size = newSize + 1;
   _buf.back() = '\0';
   for (uint pos = oldSize; pos < newSize; pos+=oldSize) {
      memcpy(begin() + pos, begin(), oldSize);
   }
   return self;
}


//!construct with buffer size
mcsl::cstr::cstr(const uint size):
_buf(size+1) {
   _buf.back() = '\0';
}
//!constructor from c-cstr and length
mcsl::cstr::cstr(const char* str, const uint strlen):
_buf(strlen+1) {
   _buf._size = cstrcpy(_buf.data(),str,strlen) + 1;
   _buf.back() = '\0';
}
//!constructor from null-terminated c-cstr
//!TODO: rework to not effectively calculate the length twice
mcsl::cstr::cstr(const char* str):
cstr(str,std::strlen(str)) {
   
}
#endif //MCSL_CSTR_CPP