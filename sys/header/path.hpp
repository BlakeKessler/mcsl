#ifndef MCSL_PATH_HPP
#define MCSL_PATH_HPP

#include "MCSL.hpp"

#include "cstr.hpp"
#include "raw_str.hpp"
#include "str_slice.hpp"

class mcsl::Path : public mcsl::cstr { //base type is implementation-defined
   private:
      static constexpr char _nameof[] = "Path";

   public:
      //values and types are implementation-defined
      static constexpr char DIR_DELIM = '/';
      static constexpr char DIR_ID = '.';
      static constexpr raw_str PARENT_DIR = "..";
      
      using cstr::cstr;

      static Path pwd(); //print working directory
      static Path usrHome(); //user home directory
      static Path tmp(); //temporary file directory
      static Path newTmpDir(); //available path for a new temporary directory

      Path& appendLoc(const str_slice);
      Path  withAppendedLoc(const str_slice loc) { Path other{self}; other.appendLoc(loc); return other; }
      Path& normalize();
      Path  normalized() { Path other{self}; other.normalize(); return std::move(other); }
};

#endif //MCSL_PATH_HPP
