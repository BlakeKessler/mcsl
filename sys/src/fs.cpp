#ifndef MCSL_FS_CPP
#define MCSL_FS_CPP

#include "fs.hpp"

#include "alloc.hpp"
#include "cstr.hpp"
#include <cstdio>

mcsl::fs::File::File(const Path fileName, const char* mode):
   File{
      fileName,
      mode,
      {mcsl::malloc<ubyte>(DEFAULT_BUF_SIZE), DEFAULT_BUF_SIZE},
      true
   } {
}
mcsl::fs::File::File(const Path fileName, const char* mode, arr_span<ubyte> buf, bool ownsBuf):
   _file{std::fopen(fileName, mode)},
   _buf{buf.data()},
   _capacity{buf.size()},
   _endIndex{0},
   _ownsBuf{ownsBuf} {
      std::setvbuf(_file, nullptr, _IONBF, 0);
}

mcsl::fs::File::~File() {
   flush();
   std::fclose(_file);
   if (_ownsBuf) {
      mcsl::free(_buf);
   }
}

void mcsl::fs::File::flush() {
   std::fwrite(_buf, 1, _endIndex, _file);
   _endIndex = 0;
}

mcsl::fs::File& mcsl::fs::File::write(const ubyte c) {
   if (_endIndex == _capacity) {
      flush();
   }
   _buf[_endIndex++] = c;
}

mcsl::fs::File& mcsl::fs::File::write(const mcsl::arr_span<ubyte> data) {
   if (data.size() + _endIndex < _capacity) {
      std::memcpy(_buf + _endIndex, data.begin(), data.size());
      _endIndex += data.size();
   } else {
      const uint initCharCount = _capacity - _endIndex;
      std::memcpy(_buf + _endIndex, data.begin(), initCharCount);
      flush();
      const mcsl::arr_span<ubyte> remainder{const_cast<ubyte*>(data.begin() + initCharCount), data.size() - initCharCount};
      debug_assert(_endIndex == 0);
      if (remainder.size() < _capacity) {
         std::memcpy(_buf, remainder.begin(), remainder.size());
         _endIndex = remainder.size();
      } else {
         std::fwrite(remainder.begin(), 1, remainder.size(), _file);
      }
   }
   
   return self;
}

mcsl::arr_span<ubyte> mcsl::fs::File::read(arr_span<ubyte> dest) {
   uint i;
   for (i = 0; i < dest.size(); ++i) {
      const char ch = read();
      if (ch == EOF) {
         break;
      }
      dest[i] = ch;
   }
   return {dest.begin(), i};
}

mcsl::raw_str_span mcsl::fs::File::readln(raw_str_span dest, const char nl) {
   uint i = 0;
   while (i < dest.size()) {
      const char ch = read();
      if (ch == nl) {
         dest[i++] = ch;
         return {dest.begin(), i};
      }
      if (ch == EOF) {
         return {dest.begin(), i};
      }
      dest[i++] = ch;
   }
   __throw(ErrCode::SEGFAULT, "buffer overflow in fs::File::readln");
}

mcsl::string&& mcsl::fs::File::readln(const char nl) {
   string str{};
   while (true) {
      const char ch = read();
      if (ch == nl) {
         str.push_back(ch);
         return std::move(str);
      }
      if (ch == EOF) {
         return std::move(str);
      }
      str.push_back(ch);
   }
}

#endif //MCSL_FS_CPP