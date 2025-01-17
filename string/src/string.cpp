#ifndef MCSL_STRING_CPP
#define MCSL_STRING_CPP

#include "string.hpp"
#include "alloc.hpp"
#include <cstring>


//!concatenate other onto the end of this
mcsl::string& mcsl::string::operator+=(const str_t auto& other) {
   char* addr = end();
   resize(size() + other.size());
   //copy other
   std::memcpy(addr, other.begin(), other.size()*sizeof(char));
   //return
   return self;
}
//!repeat string
mcsl::string& mcsl::string::operator*=(const uint repeatCount) {
   const uint oldSize = size();
   resize_exact(repeatCount * size());
   for (uint pos = oldSize; pos < size(); pos+=oldSize) {
      std::memcpy(begin() + pos, begin(), oldSize*sizeof(char));
   }
   _buf[_buf.size()] = '\0';
   return self;
}


//!construct with buffer size
mcsl::string::string(const uint size):
_buf(size+1) {
   _buf.back() = '\0';
}
//!constructor from c-string and length
mcsl::string::string(const char* str, const uint strlen):
_buf(strlen+1) {
   std::strncpy(_buf.data(),str,strlen*sizeof(str[0]));
   _buf.back() = '\0';
}
//!constructor from null-terminated c-string
mcsl::string::string(const char* str):
string(str,std::strlen(str)) {
   
}
//!move constructor
mcsl::string::string(string&& other):
   _buf(std::move(other._buf)) {
      if (this != &other) { other.release(); }
}
//!copy constructor
//!NOTE: will be removed if possible
mcsl::string::string(const string& other):
   string(other.size()) {
      for (uint i = 0; i < other.size(); ++i) {
         _buf[i] = other[i];
      }
}
#endif //MCSL_STRING_CPP