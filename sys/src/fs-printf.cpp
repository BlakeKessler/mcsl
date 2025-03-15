#ifndef FS_PRINTF_CPP
#define FS_PRINTF_CPP

#include "fs.hpp"

#include "tuple.hpp"
#include "str_to_num.hpp"

namespace {
   enum FmtVarFields : ubyte {
      NONE = 0,
      MIN_WIDTH = 1_m,
      PRECISION = 2_m,
      RADIX = 3_m
   };
   FmtVarFields& operator|=(FmtVarFields& lhs, FmtVarFields rhs) {
      lhs = (FmtVarFields)((ubyte)(lhs) | (ubyte)(rhs));
      return lhs;
   }
   bool operator&(FmtVarFields lhs, FmtVarFields rhs) {
      return (ubyte)(lhs) & (ubyte)(rhs);
   }
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wunneeded-internal-declaration"
   mcsl::tuple<char, mcsl::FmtArgs, uint, FmtVarFields> __parseFmtCode(const mcsl::str_slice str) {
      char mode = 0;
      mcsl::FmtArgs args{};
      uint i = 0;
      FmtVarFields flags = NONE;
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
               ++i;
               break;


            case '-': args.isLeftJust = true; ++i; break;
            case '+': args.alwaysPrintSign = true; ++i; break;
            case mcsl::PAD_CHAR: args.padForPosSign = true; ++i; break;
            case '#': args.altMode = true; ++i; break;
            case '0': args.padWithZero = true; ++i; break;

            //handled after this
            case '.':
            case ',':
            case '*':
            case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
               mode = -1;
               break;
            
            default:
               mcsl::__throw(mcsl::ErrCode::FS_ERR, "invalid format");
         }
      } while (!mode);

      //minWidth, precision, radix
      if (mode == -1) {
         //minWidth
         if (i < str.size()) {
            if (str[i] == mcsl::FMT_VAR_FIELD) {
               flags |= MIN_WIDTH;
               ++i;
            } else {
               while (i < str.size() && mcsl::is_digit(str[i], 10)) {
                  args.minWidth *= 10;
                  args.minWidth += mcsl::digit_to_uint(str[i]);
                  ++i;
               }
            }
         }
         
         //precision
         if (i < str.size() && str[i] == mcsl::FMT_PREC_INTRO) {
            ++i;
            if (i < str.size() && str[i] == mcsl::FMT_VAR_FIELD) {
               flags |= PRECISION;
            } else {
               while (i < str.size() && mcsl::is_digit(str[i], 10)) {
                  args.precision *= 10;
                  args.precision += mcsl::digit_to_uint(str[i]);
                  ++i;
               }
            }
         }

         //radix
         if (i < str.size() && str[i] == mcsl::FMT_RADIX_INTRO) {
            ++i;
            if (i < str.size() && str[i] == mcsl::FMT_VAR_FIELD) {
               flags |= RADIX;
            }
            while (i < str.size() && mcsl::is_digit(str[i], 10)) {
               args.radix *= 10;
               args.radix += mcsl::digit_to_uint(str[i]);
               ++i;
            }
         }

         switch (str[i]) {
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
               ++i;
               break;

            case mcsl::FMT_INTRO: 
               mcsl::__throw(mcsl::ErrCode::FS_ERR, "invalid format (%% may not have arguments)");
            default:
               mcsl::__throw(mcsl::ErrCode::FS_ERR, "invalid format");
         }
      }
      #pragma GCC diagnostic pop

      if (flags & flags) { //warn the user that variable fields are not yet supported
         mcsl::__throw(mcsl::ErrCode::FS_ERR, "WARNING: mcsl::printf does not yet support variable format fields - if are printing variable-length strings, use an mcsl::str_slice");
      }

      return {mode, args, i, flags};
   }

   uint __printfImpl(mcsl::File& file, const mcsl::str_slice str, uint charsPrinted);
   uint __printfImpl(mcsl::File& file, const mcsl::str_slice str, uint charsPrinted, const auto& arg0, const auto&... argv);

   uint __printfImpl(mcsl::File& file, const mcsl::str_slice str, uint charsPrinted, const auto& arg0, const auto&... argv) {
      for (uint i = 0; i < str.size(); ++i) {
         if (str[i] == mcsl::FMT_INTRO) { //found format code
            file.write(mcsl::str_slice::make(str, i));
            charsPrinted += i;
            assert(str.size() > (i+1), "%% not followed by format code", mcsl::ErrCode::FS_ERR);
            auto [mode, fmtArgs, codeLen, flags] = __parseFmtCode(mcsl::str_slice::make(str, i+1, str.size()-(i+1)));

            // if (flags & MIN_WIDTH) {} //!TODO: figure out a good way to do this
            // if (flags & PRECISION) {} //!TODO: figure out a good way to do this
            // if (flags & RADIX) {} //!TODO: figure out a good way to do this

            if (mode == mcsl::FMT_INTRO) { //%%
               file.write(mcsl::FMT_INTRO);
               ++charsPrinted;
            } else { //other format code
               charsPrinted += file.writef(arg0, str[i + codeLen], fmtArgs);
            }
            if (i + codeLen < str.size()) { //more to print
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
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wunneeded-internal-declaration"
   uint __printfImpl(mcsl::File& file, const mcsl::str_slice str, uint charsPrinted) {
      for (uint i = 0; i < str.size(); ++i) {
         if (str[i] == mcsl::FMT_INTRO) { //found format code
            [[unlikely]];
            file.write(mcsl::str_slice::make(str, i));
            charsPrinted += i;
            assert(str.size() > (i+1), "%% not followed by format code", mcsl::ErrCode::FS_ERR);
            if (str[i + 1] == mcsl::FMT_INTRO) { //%%
               file.write(mcsl::FMT_INTRO);
               ++charsPrinted;
            } else { //other format code
               mcsl::__throw(mcsl::ErrCode::FS_ERR, "printf: more consuming format codes than arguments");
            }
            if (i+2 < str.size()) { //more to print
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
   #pragma GCC diagnostic pop
};

uint mcsl::File::printf(const mcsl::str_slice fmt, const auto&... argv) {
   return __printfImpl(self, fmt, 0, argv...);
}

#endif //FS_PRINTF_CPP