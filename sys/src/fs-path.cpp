#ifndef FS_PATH_CPP
#define FS_PATH_CPP

#include "fs.hpp"

#include "dyn_str_span.hpp"
#include "str_slice.hpp"

mcsl::Path& mcsl::Path::appendLoc(const str_slice loc) {
   //directory delimiter
   if (size() > 0 && back() != DIR_DELIM) {
      self.push_back(DIR_DELIM);
   }
   //path
   self += loc;

   //return
   return self;
}

mcsl::Path& mcsl::Path::normalize() { //!TODO: test this
   dyn_str_span rest = self;
   auto currLocLen = [&]() {
      for (uint i = 0; i < rest.size(); ++i) {
         if (rest[i] == DIR_DELIM) {
            return i;
         }
      }
      return rest.size();
   };
   auto prevLocLen = [&]() {
      if (rest.begin() - 2 > begin()) {
         return 0u;
      }
      const char* it = rest.begin() - 2;
      const char* b = begin();
      while (it >= b) {
         if (it[0] == DIR_DELIM) {
            return (uint)(rest.begin() - it + 1);
         }
         --it;
      }
      return (uint)(rest.begin() - it);
   };
   while (rest.size()) {
      if (rest.size() >= (PARENT_DIR.size() + 1) && rest.substrcmp(PARENT_DIR) == 0 && rest[PARENT_DIR.size()] == DIR_DELIM) { // ../
         //!TODO: .. (as opposed to ../)
         const uint len = prevLocLen();
         if (len == 0) { // ../
            //!TODO: /../
            rest.inc_begin(PARENT_DIR.size() + 1);
            continue;
         }
         if (len == PARENT_DIR.size() + 1) {
            auto tmp = rest;
            tmp.inc_begin(-len);
            if (tmp.substrcmp(PARENT_DIR) == 0 && tmp[PARENT_DIR.size()] == DIR_DELIM) { // ../../
               rest.inc_begin(PARENT_DIR.size() + 1);
               continue;
            }
         }
         //remove current dir and the ../
         std::memmove(rest.begin() - len, rest.begin() + (PARENT_DIR.size()+1), rest.size() - PARENT_DIR.size()); //rest.size() - PARENT_DIR.size() instead of -(PARENT_DIR.size()+1) to also catch the null terminator
         rest = dyn_str_span{self, (uint)(rest.begin() - begin() - len), rest.size() - (PARENT_DIR.size() + 1)}; //adjust rest
         continue;

      } else if (rest.size() >= 2 && rest[0] == DIR_ID && rest[1] == DIR_DELIM) { // ./
         std::memmove(rest.begin(), rest.begin() + 2, rest.size() - 1); //remove the ./ (rest.size()-1 instead of -2 to also catch the null terminator)
         rest.inc_end(-2); //adjust rest
         continue;

      } else { //skip current location
         rest.inc_begin(currLocLen());
         if (rest.size()) {
            rest.inc_begin(1);
         }
         continue;
      }
   }

   return self;
}

#endif //FS_PATH_CPP