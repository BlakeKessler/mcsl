#ifndef FS_PRINTF_CPP
#define FS_PRINTF_CPP

#include "io.hpp"

#include "tuple.hpp"
#include "str_to_num.hpp"

#include "throw.hpp"

namespace {
   using namespace mcsl;
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
   tuple<char, FmtArgs, uint, FmtVarFields> __parseFmtCode(const str_slice str) {
      char mode = 0;
      FmtArgs args{};
      uint i = 0;
      FmtVarFields flags = NONE;
      bool usedDefaultPrec = true;
      do {
         if (i >= str.size()) {
            __throw(ErrCode::FS_ERR, FMT("un-terminated format code"));
         }
         switch (str[i]) {
            case 'f': case 'F':
               args.precision = DEFAULT_FLT_PREC;
               [[fallthrough]];
               
            case FMT_INTRO: [[fallthrough]];

            case 'r': case 'R':
            case 'b': case 'B':
            case 'u': case 'U':
            case 'i': case 'I':
            case 'g': case 'G':
            case 'e': case 'E':
            case 'c': case 'C':
            case 's': case 'S':
               mode = str[i];
               ++i;
               break;


            case '-': args.isLeftJust = true; ++i; break;
            case '+': args.alwaysPrintSign = true; ++i; break;
            case PAD_CHAR: args.padForPosSign = true; ++i; break;
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
               __throw(ErrCode::FS_ERR, FMT("invalid format"));
         }
      } while (!mode);

      //minWidth, precision, radix
      if (mode == -1) {
         //minWidth
         if (i < str.size()) {
            if (str[i] == FMT_VAR_FIELD) {
               flags |= MIN_WIDTH;
               ++i;
            } else {
               while (i < str.size() && is_digit(str[i], 10)) {
                  args.minWidth *= 10;
                  args.minWidth += digit_to_uint(str[i]);
                  ++i;
               }
            }
         }
         
         //precision
         if (i < str.size() && str[i] == FMT_PREC_INTRO) {
            usedDefaultPrec = false;
            ++i;
            if (i < str.size() && str[i] == FMT_VAR_FIELD) {
               flags |= PRECISION;
            } else {
               while (i < str.size() && is_digit(str[i], 10)) {
                  args.precision *= 10;
                  args.precision += digit_to_uint(str[i]);
                  ++i;
               }
            }
         }

         //radix
         if (i < str.size() && str[i] == FMT_RADIX_INTRO) {
            ++i;
            if (i < str.size() && str[i] == FMT_VAR_FIELD) {
               flags |= RADIX;
            }
            while (i < str.size() && is_digit(str[i], 10)) {
               args.radix *= 10;
               args.radix += digit_to_uint(str[i]);
               ++i;
            }
         }

         switch (str[i]) {
            case 'f': case 'F':
               if (usedDefaultPrec) {
                  args.precision = DEFAULT_FLT_PREC;
               }
            case 'r': case 'R':
            case 'b': case 'B':
            case 'u': case 'U':
            case 'i': case 'I':
            case 'g': case 'G':
            case 'e': case 'E':
            case 'c': case 'C':
            case 's': case 'S':
               mode = str[i];
               ++i;
               break;

            case FMT_INTRO: 
               __throw(ErrCode::FS_ERR, FMT("invalid format (%% may not have arguments)"));
            default:
               __throw(ErrCode::FS_ERR, FMT("invalid format"));
         }
      }
      #pragma GCC diagnostic pop

      if (flags & flags) { //warn the user that variable fields are not yet supported
         __throw(ErrCode::FS_ERR, FMT("WARNING: printf does not yet support variable format fields - if are printing variable-length strings, use an str_slice"));
      }

      return {mode, args, i, flags};
   }

   uint __printfImpl(File& file, const str_slice str, uint charsPrinted);
   uint __printfImpl(File& file, const str_slice str, uint charsPrinted, const auto& arg0, const auto&... argv);

   uint __printfImpl(File& file, const str_slice str, uint charsPrinted, const auto& arg0, const auto&... argv) {
      for (uint i = 0; i < str.size(); ++i) {
         if (str[i] == FMT_INTRO) { //found format code
            if (i) {
               write(file, str.slice(i));
               charsPrinted += i;
            }
            ++i;
            assert(str.size() > i, "%% not followed by format code", ErrCode::FS_ERR);
            auto [mode, fmtArgs, codeLen, flags] = __parseFmtCode(str.slice(i, str.size()-i));

            // if (flags & MIN_WIDTH) {} //!TODO: figure out a good way to do this
            // if (flags & PRECISION) {} //!TODO: figure out a good way to do this
            // if (flags & RADIX) {} //!TODO: figure out a good way to do this

            i += codeLen;
            if (mode == FMT_INTRO) { //%%
               write(file, FMT_INTRO);
               ++charsPrinted;
               if (i <= str.size()) { //more to print
                  return __printfImpl(file, str.slice(i, str.size() - i), charsPrinted, arg0, argv...); //tail recursion
               }
            } else { //other format code
               charsPrinted += writef(file, arg0, mode, fmtArgs);
               if (i <= str.size()) { //more to print
                  return __printfImpl(file, str.slice(i, str.size() - i), charsPrinted, argv...); //tail recursion
               }
            }
            //end of format string
            [[unlikely]];
            return charsPrinted;
         }
      }
      //no format codes in format string
      [[unlikely]];
      write(file, str);
      charsPrinted += str.size();
      return charsPrinted;
   }
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wunneeded-internal-declaration"
   uint __printfImpl(File& file, const str_slice str, uint charsPrinted) {
      for (uint i = 0; i < str.size(); ++i) {
         if (str[i] == FMT_INTRO) { //found format code
            [[unlikely]];
            write(file, str.slice(i));
            charsPrinted += i;
            ++i;
            assert(str.size() > i, "%% not followed by format code", ErrCode::FS_ERR);
            if (str[i] == FMT_INTRO) { //%%
               write(file, FMT_INTRO);
               ++charsPrinted;
            } else { //other format code
               __throw(ErrCode::FS_ERR, FMT("printf: more consuming format codes than arguments"));
            }
            ++i;
            if (i < str.size()) { //more to print
               return __printfImpl(file, str.slice(i, str.size() - i), charsPrinted);
            } else { //end of format string
               return charsPrinted;
            }
         }
      }
      //no format codes in format string
      [[likely]];
      write(file, str);
      charsPrinted += str.size();
      return charsPrinted;
   }
   #pragma GCC diagnostic pop
};

uint mcsl::File::printf(const str_slice fmt, const auto&... argv) {
   return __printfImpl(self, fmt, 0, argv...);
}

#endif //FS_PRINTF_CPP