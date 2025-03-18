#pragma once
#ifndef MCSL_IO_HPP
#define MCSL_IO_HPP

// #if defined(stdout) && !defined(MCSL)
// static_assert(false, "cannot use <cstdio> and \"io.hpp\" together");
// #endif

#include "MCSL.hpp"

#include "fs.hpp"
#include <cstdio>
#undef NULL

namespace mcsl {
   //!standard files
   mcsl::File stdout = mcsl::File::ReopenLibcFile(::stdout);
   mcsl::File stderr = mcsl::File::ReopenLibcFile(::stderr);
   mcsl::File stdin = mcsl::File::ReopenLibcFile(::stdin);

   //!standard formatted IO
   inline uint printf(const str_slice fmt, const auto&... argv) { return stdout.printf(fmt, argv...); }
   inline uint err_printf(const str_slice fmt, const auto&... argv) { return stderr.printf(fmt, argv...); }
   inline uint scanf(const str_slice fmt, auto*... argv) { return stdin.scanf(fmt, argv...); }

   //!standard unformatted output
   inline File& write(const ubyte c) { return stdout.write(c); }
   inline File& write(const ubyte c, uint repCount) { return stdout.write(c, repCount); }
   inline File& write(const arr_span<ubyte> data) { return stdout.write(data); }
   inline File& write(const char c) { return stdout.write(c); }
   inline File& write(const char c, uint repCount) { return stdout.write(c, repCount); }
   inline File& write(const str_slice str) { return stdout.write(str); }
   inline File& writeln(const str_slice str, const char nl = '\n') { stdout.write(str); stdout.write(nl); return stdout; }

   //!standard unformatted input
   inline char read() { return stdin.read(); }
   inline arr_span<ubyte> read(arr_span<ubyte> dest) { return stdin.read(dest); }
   inline str_slice read(str_slice dest) { return stdin.read(dest); }
   inline str_slice readln(str_slice dest, const char nl = '\n') { return stdin.readln(dest, nl); }
   inline string readln(const char nl = '\n') { return stdin.readln(nl); }
}

#endif //MCSL_IO_HPP