#ifndef MCSL_CSTR_CPP
#define MCSL_CSTR_CPP

#include "cstr.hpp"
#include "alloc.hpp"
#include <cstring>


//!concatenate other onto the end of this
mcsl::cstr& mcsl::cstr::operator+=(const str_t auto& other) {
   char* addr = end();
   reserve(size() + other.size());
   //copy other
   std::memcpy(addr, other.begin(), other.size()*sizeof(char));
   //return
   return self;
}
//!repeat cstr
mcsl::cstr& mcsl::cstr::operator*=(const uint repeatCount) {
   const uint oldSize = size();
   reserve_exact(repeatCount * size());
   for (uint pos = oldSize; pos < size(); pos+=oldSize) {
      std::memcpy(begin() + pos, begin(), oldSize*sizeof(char));
   }
   _buf[_buf.size()] = '\0';
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
   std::strncpy(_buf.data(),str,strlen*sizeof(str[0]));
   _buf.back() = '\0';
}
//!constructor from null-terminated c-cstr
mcsl::cstr::cstr(const char* str):
cstr(str,std::strlen(str)) {
   
}
//!move constructor
mcsl::cstr::cstr(cstr&& other):
   _buf(std::move(other._buf)) {
      if (this != &other) { other.release(); }
}
//!copy constructor
//!NOTE: will be removed if possible
mcsl::cstr::cstr(const cstr& other):
   cstr(other.size()) {
      for (uint i = 0; i < other.size(); ++i) {
         _buf[i] = other[i];
      }
}
#endif //MCSL_CSTR_CPP