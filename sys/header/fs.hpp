#pragma once
#ifndef MCSL_FS_HPP
#define MCSL_FS_HPP

#include "MCSL.hpp"

#include "arr_span.hpp"
#include "raw_str_span.hpp"
#include "string.hpp"
#include "cstr.hpp"
#include "raw_str.hpp"
#include "type_traits.hpp"

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

      Path& appendLoc(const raw_str_span);
      Path  withAppendedLoc(const raw_str_span loc) { Path other{self}; other.appendLoc(loc); return other; }
      Path& normalize();
      Path  normalized() { Path other{self}; other.normalize(); return std::move(other); }
};

struct mcsl::FmtArgs {
   uint radix = 0;
   uint minWidth = 0;
   uint precision = 0;
   bool isLeftJust = false;
   bool alwaysPrintSign = false;
   bool altMode = false;
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
      File& write(const raw_str_span str) { return write(arr_span<ubyte>((ubyte*)str.begin(), str.size())); }
      File& writeln(const raw_str_span str, const char nl = '\n') { write(str); write(nl); return self; }

      char read() { return std::getc(_file); }
      arr_span<ubyte> read(arr_span<ubyte> dest);
      raw_str_span read(raw_str_span dest) { return {dest.begin(), read(arr_span<ubyte>((ubyte*)dest.begin(), dest.size())).size()}; }
      raw_str_span readln(raw_str_span dest, const char nl = '\n');
      string readln(const char nl = '\n');

      template<typename T> File& write(const T& obj); //serialize object - to be implemented by users for their types
      template<typename T> T read(); //deserialize object - to be implemented by users for their types
      template<typename T, typename Buffer_t> T read(Buffer_t buf); //deserialize object - to be implemented by users for their types

      uint printf(const raw_str_span fmt, const auto&... argv);
      uint scanf(const raw_str_span fmt, auto*... argv);

      template<typename T> uint writef(const T& obj, char mode, FmtArgs fmt = FmtArgs()); //used by printf, to be implemented by users for their types
      template<typename T> uint readf(T* obj,
         char mode,
         uint maxWidth = mcsl::TYPEMAX<uint>()
      ); //used by scanf, to be implemented by users for their types

      operator FILE*() { return _file; }
      operator const FILE*() const { return _file; }
};

#include <type_traits>
#define _fdeclio(T)\
template<> mcsl::File& mcsl::File::write<T>(std::add_lvalue_reference_t<std::add_const_t<T>>); \
template<> uint mcsl::File::writef<T>(std::add_lvalue_reference_t<std::add_const_t<T>>, char, FmtArgs); \
template<> uint mcsl::File::readf<T>(T* obj, char, uint)

_fdeclio(uint8);
_fdeclio(uint16);
_fdeclio(uint32);
_fdeclio(uint64);
_fdeclio(uint128);

_fdeclio(float);
_fdeclio(flong);
_fdeclio(flext);
// _fdeclio(float8);
// _fdeclio(float16);
// _fdeclio(float32);
// _fdeclio(float64);
// _fdeclio(float128);

_fdeclio(bool);
_fdeclio(char);
_fdeclio(char8);
_fdeclio(char16);
_fdeclio(char32);

_fdeclio(mcsl::raw_str_span);

_fdeclio(void*); //!TODO: CTAD for printing all pointers as void pointers

#undef _fdeclio

#endif //MCSL_FS_HPP