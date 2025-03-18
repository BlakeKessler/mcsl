#pragma once
#ifndef MCSL_FS_HPP
#define MCSL_FS_HPP

#include "MCSL.hpp"

#include "arr_span.hpp"
#include "str_slice.hpp"
#include "string.hpp"
#include "cstr.hpp"
#include "raw_str.hpp"
#include "type_traits.hpp"
#include "hw.hpp"

class mcsl::Path : public mcsl::cstr { //base type is implementation-defined
   private:
      static constexpr raw_str _nameof = "Path";

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

struct mcsl::FmtArgs {
   uint radix = 0;
   uint minWidth = 0;
   uint precision = 0;
   bool isLeftJust = false;
   bool alwaysPrintSign = false;
   bool padForPosSign = false;
   bool padWithZero = false;
   bool altMode = false;

   sys_endian endianness_b() const;
   sys_endian endianness_r() const;
};

class mcsl::File {
   private:
      FILE* _file;
      ubyte* _buf;
      uint _capacity;
      uint _endIndex;
      bool _ownsBuf;
      //!TODO: control code stack(s)? EX: ANSI color codes
      //!TODO: abstract base class? file_t template?
      //!TODO: maybe make reference-counted
      //!   (since a file being opened by multiple File objects will cause issues)
      //!   if that is done, allocate _buf in-place to avoid additional indirection (since it will never be resized)
      File(FILE*);
      File(sint); //only on systems with a Unix-like/Windows-like file descriptor/handle system
   public:
      static constexpr uint DEFAULT_BUF_SIZE = BUFSIZ;

      File(const Path fileName, const char* mode);
      File(const Path fileName, const char* mode, arr_span<ubyte> buf, bool ownsBuf = false);
      static File ReopenLibcFile(FILE* file) { return File{file}; }

      ~File();
      void close() { File::~File(); }
      
      void flush();

      File& write(const ubyte c);
      File& write(const ubyte c, uint repCount);
      File& write(const arr_span<ubyte> data);
      File& write(const char c) { return write((ubyte)c); }
      File& write(const char c, uint repCount) { return write((ubyte)c, repCount); }
      File& write(const str_slice str) { return write(arr_span<ubyte>((ubyte*)str.begin(), str.size())); }
      File& writeln(const str_slice str, const char nl = '\n') { write(str); write(nl); return self; }

      char read() { return std::getc(_file); }
      arr_span<ubyte> read(arr_span<ubyte> dest);
      str_slice read(str_slice dest) { return {dest.begin(), read(arr_span<ubyte>((ubyte*)dest.begin(), dest.size())).size()}; }
      string readChars(uint charCount);
      dyn_arr<ubyte> readBytes(uint byteCount);
      str_slice readln(str_slice dest, const char nl = '\n');
      string readln(const char nl = '\n');

      uint printf(const str_slice fmt, const auto&... argv);
      uint scanf(const str_slice fmt, auto*... argv);

      operator FILE*() { return _file; }
      operator const FILE*() const { return _file; }
};

#include "MAP_MACRO.h"
#define _fdeclio(T)\
inline File& write(File&, const T); \
uint writef(File&, const T, char, FmtArgs); \
File& read(File&, T*); \
uint readf(File&, T* obj, char, uint);

namespace mcsl {
   MCSL_MAP(_fdeclio, MCSL_ALL_NUM_T)
   MCSL_MAP(_fdeclio, MCSL_ALL_CHAR_T)

   _fdeclio(bool);
   _fdeclio(mcsl::str_slice);
   _fdeclio(void*);

   template<typename T> File& write(File& file, const arr_span<T> buf) {
      if (buf.size()) {
         write(file, buf[0]);
         for (uint i = 1; i < buf.size(); ++i) {
            write(file, ',');
            write(file, ' ');
            write(file, buf[i]);
         }
      }
      return file;
   }
   // template<typename T> File& write(File& file, const arr_span<T> buf) {
   //    file.write(buf.size());
   //    for (uint i = 0; i < buf.size(); ++i) {
   //       write(file, i);
   //    }
   //    return file;
   // }
};

#undef _fdeclio
#include "MAP_MACRO_UNDEF.h"

#pragma region inlinesrc

// #include "../src/fs-printf.cpp"
// #include "../src/fs-write.cpp"

#pragma endregion inlinesrc

// #include "throw.hpp"

#endif //MCSL_FS_HPP