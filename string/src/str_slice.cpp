#ifndef MCSL_STR_SLICE_CPP
#define MCSL_STR_SLICE_CPP

#include "str_slice.hpp"

const mcsl::str_slice mcsl::FMT(const char* buf) { return str_slice::make_from_cstr(buf); }

#endif //MCSL_STR_SLICE_CPP