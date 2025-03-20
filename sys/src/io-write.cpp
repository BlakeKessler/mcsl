#ifndef MCSL_WRITE_CPP
#define MCSL_WRITE_CPP

#include "io.hpp"

#include "MAP_MACRO.h"
#define __write(T, code, fmt) \
mcsl::File& mcsl::write(File& file, const T x) { \
   mcsl::writef(file, x, code, mcsl::FmtArgs fmt);\
   return file;\
}
#define __writeU(T) __write(T, 'u', ())
#define __writeS(T) __write(T, 's', ())
#define __writeF(T) __write(T, 'f', {.precision=mcsl::DEFAULT_FLT_PREC})

MCSL_MAP(__writeU, MCSL_ALL_UINT_T)
MCSL_MAP(__writeS, MCSL_ALL_SINT_T)
MCSL_MAP(__writeF, MCSL_ALL_FLOAT_T)

#undef __writeF
#undef __writeS
#undef __writeU
#undef __write
#include "MAP_MACRO_UNDEF.h"

mcsl::File& mcsl::write(File& file, const void* ptr) {
   mcsl::writef(file, (uptr)ptr, 'u', mcsl::FmtArgs{.radix=mcsl::DEFAULT_RAW_RADIX});
   return file;
}

mcsl::File& mcsl::write(File& file, const char ch) { return file.write(ch); }
mcsl::File& mcsl::write(File& file, str_slice str) { return file.write(str); }

#endif //MCSL_WRITE_CPP