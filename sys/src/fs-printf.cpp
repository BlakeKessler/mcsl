#ifndef FS_PRINTF_CPP
#define FS_PRINTF_CPP

#include "fs.hpp"

#include "tuple.hpp"

namespace {
   mcsl::tuple<char, mcsl::FmtArgs, uint> __parseFmtCode(const mcsl::str_slice str) {
      char mode = 0;
      mcsl::FmtArgs args{};
      uint i = 0;
      do {
         if (i >= str.size()) {
            mcsl::__throw(mcsl::ErrCode::FS_ERR, "un-terminated format code");
         }
         switch (str[i]) {
            case mcsl::FMT_INTRO: [[fallthrough]];

            case 'r': case 'R':
            case 'b': case 'B':
            case 'u': case 'U':
            case 'i': case 'I':
            case 'f': case 'F':
            case 'g': case 'G':
            case 'e': case 'E':
            case 'c': case 'C':
            case 's': case 'S':
               mode = str[i];
               break;


            case '-': args.isLeftJust = true; break;
            case '+': args.alwaysPrintSign = true; break;
            case mcsl::PAD_CHAR: args.padForPosSign = true; break;
            case '#': args.altMode = true; break;
            case '0': args.padWithZero = true; break;

            //!TODO: everything in this switch statement below this comment
            case '.':
            case ',':
            case '*':


            case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
         }
      } while (++i, !mode);

      if (mode == mcsl::FMT_INTRO) {
         assert(i == 1, "invalid format code (%%%% may not have arguments)", mcsl::ErrCode::FS_ERR);
         return {mcsl::FMT_INTRO, {}, 1};
      }
      return {mode, args, i};
   }
   uint __printfImpl(mcsl::File& file, const mcsl::str_slice str, uint charsPrinted, const auto& arg0, const auto&... argv) {
      for (uint i = 0; i < str.size(); ++i) {
         if (str[i] == mcsl::FMT_INTRO) { //found format code
            file.write(mcsl::str_slice::make(str, i));
            charsPrinted += i;
            assert(str.size() > (str.size()+1), "%% not followed by format code", mcsl::ErrCode::FS_ERR);
            auto [mode, fmtArgs, codeLen] = __parseFmtCode(mcsl::str_slice::make(str, i+1, str.size()-(i+1)));
            if (str[i + codeLen] == mcsl::FMT_INTRO) { //%%
               file.write(mcsl::FMT_INTRO);
               ++charsPrinted;
            } else { //other format code
               charsPrinted += file.writef(arg0, str[i + codeLen], fmtArgs);
            }
            if (i + codeLen > str.size()) { //more to print
               return __printfImpl(file, mcsl::str_slice::make(str, i + codeLen + 1, str.size() - (i + codeLen + 1)), charsPrinted, argv...); //tail recursion
            } else { //end of format string
               [[unlikely]];
               return charsPrinted;
            }
         }
      }
      //no format codes in format string
      [[unlikely]];
      file.write(str);
      return str.size();
   }
   uint __printfImpl(mcsl::File& file, const mcsl::str_slice str, uint charsPrinted) {
      for (uint i = 0; i < str.size(); ++i) {
         if (str[i] == mcsl::FMT_INTRO) { //found format code
            [[unlikely]];
            file.write(mcsl::str_slice::make(str, i));
            charsPrinted += i;
            assert(str.size() > (str.size()+1), "%% not followed by format code", mcsl::ErrCode::FS_ERR);
            if (str[i + 1] == mcsl::FMT_INTRO) { //%%
               file.write(mcsl::FMT_INTRO);
               ++charsPrinted;
            } else { //other format code
               mcsl::__throw(mcsl::ErrCode::FS_ERR, "printf: more consuming format codes than arguments");
            }
            if (i + 2 > str.size()) { //more to print
               return __printfImpl(file, mcsl::str_slice::make(str, i + 2, str.size() - (i + 2)), charsPrinted);
            } else { //end of format string
               return charsPrinted;
            }
         }
      }
      //no format codes in format string
      [[likely]];
      file.write(str);
      return str.size();
   }
};

uint mcsl::File::printf(const mcsl::str_slice fmt, const auto&... argv) {
   return __printfImpl(self, fmt, 0, argv...);
}

#endif //FS_PRINTF_CPP