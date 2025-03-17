#ifndef MCSL_FS_WRITE_CPP
#define MCSL_FS_WRITE_CPP

#include "fs.hpp"

#define __write(T, code, fmt) \
inline mcsl::File& mcsl::write(File& file, const T x) { \
   mcsl::writef(file, x, code, mcsl::FmtArgs fmt);\
   return file;\
}

#define __writeInts(size) \
__write(uint##size, 'u', ()) __write(sint##size, 's', ())

__writeInts(8)
__writeInts(16)
__writeInts(32)
__writeInts(64)
__writeInts(128)

#undef __writeInts

__write(float, 'f', {.precision=mcsl::DEFAULT_FLT_PREC})
__write(flong, 'f', {.precision=mcsl::DEFAULT_FLT_PREC})
__write(flext, 'f', {.precision=mcsl::DEFAULT_FLT_PREC})

__write(bool, 's', ())

#undef __write

inline mcsl::File& mcsl::write(File& file, const void* ptr) {
   mcsl::writef(file, (uptr)ptr, 'u', mcsl::FmtArgs{.radix=mcsl::DEFAULT_RAW_RADIX});
   return file;
}

inline mcsl::File& mcsl::write(File& file, const char ch) { return file.write(ch); }
inline mcsl::File& mcsl::write(File& file, str_slice str) { return file.write(str); }

#endif //MCSL_FS_WRITE_CPP