#ifndef FS_WRITEF_CPP
#define FS_WRITEF_CPP

#include "io.hpp"

#include "num_to_str.hpp"
#include "math.hpp"
#include "fpmanip.hpp"

#include "string.hpp"
#include "raw_buf_str.hpp"
#include "raw_str.hpp"

#include "type_traits.hpp"

#include "throw.hpp"

#include "MAP_MACRO.h"

#define __ALT_MODE                         \
if (fmt.altMode) {                         \
   file.write('0');                        \
   char ch = isLower ? mcsl::CASE_BIT : 0; \
   switch (fmt.radix) {                    \
      case  2: ch |= 'B'; break;           \
      case  8: ch |= 'O'; break;           \
      case 10: ch |= 'D'; break;           \
      case 16: ch |= 'X'; break;           \
      default: UNREACHABLE;                \
   }                                       \
   file.write(ch);                         \
   charsPrinted += 2;                      \
}
#define __PRINT_AS_CHAR(char_t)                 \
if constexpr (sizeof(T) == sizeof(char_t)) {    \
   return writef(file, (char_t)num, mode, fmt); \
}
#define __PRINT_AS_CHARS MCSL_MAP(__PRINT_AS_CHAR, MCSL_ALL_CHAR_T)

namespace {
   constexpr mcsl::str_slice __EXP_NOTAT = mcsl::str_slice::make(mcsl::EXP_NOTAT, sizeof(mcsl::EXP_NOTAT) - !mcsl::EXP_NOTAT[sizeof(mcsl::EXP_NOTAT)-1]);
   constexpr mcsl::str_slice __NAN_STR = mcsl::str_slice::make(mcsl::NAN_STR, sizeof(mcsl::NAN_STR) - !mcsl::NAN_STR[sizeof(mcsl::NAN_STR)-1]);
   constexpr mcsl::str_slice __INF_STR = mcsl::str_slice::make(mcsl::INF_STR, sizeof(mcsl::INF_STR) - !mcsl::INF_STR[sizeof(mcsl::INF_STR)-1]);
   constexpr mcsl::str_slice __NEG_INF_STR = mcsl::str_slice::make(mcsl::NEG_INF_STR, sizeof(mcsl::NEG_INF_STR) - !mcsl::NEG_INF_STR[sizeof(mcsl::NEG_INF_STR)-1]);
   constexpr mcsl::str_slice __POS_INF_STR = mcsl::str_slice::make(mcsl::POS_INF_STR, sizeof(mcsl::POS_INF_STR) - !mcsl::POS_INF_STR[sizeof(mcsl::POS_INF_STR)-1]);
   
   //formatted writing of binary data directly into the file
   uint writefBinaryImpl(mcsl::File& file, const mcsl::arr_span<ubyte> data, mcsl::FmtArgs& fmt) {
      fmt.radix = fmt.radix ? fmt.radix : mcsl::DEFAULT_RAW_RADIX;

      //right-justified padding
      if (!fmt.isLeftJust && fmt.minWidth > data.size()) {
         file.write('\0', fmt.minWidth - data.size());
      }

      //write binary data to file
      if (fmt.endianness_b() == mcsl::sys_endian::SYS) {
         file.write(data);
      } else { //ANTI_SYS
         for (uint i = data.size(); i--;) {
            file.write(data[i]);
         }
      }
      

      //left-justified padding
      if (fmt.isLeftJust && fmt.minWidth > data.size()) {
         file.write('\0', fmt.minWidth - data.size());
      }

      //return number of chars printed
      return mcsl::max(data.size(), fmt.minWidth);
   }
   
   uint writefRawImpl(mcsl::File& file, const mcsl::arr_span<ubyte> data, bool isLowercase, mcsl::FmtArgs& fmt); //forward declaration
   
   
   template<mcsl::uint_t T> uint writefImpl(mcsl::File& file, T num, char mode, mcsl::FmtArgs& fmt) {
      switch (mode | mcsl::CASE_BIT) {
         case 'c':
            __PRINT_AS_CHARS
         [[fallthrough]];
         case 's':
            mcsl::__throw(mcsl::ErrCode::FS_ERR, mcsl::FMT("invalid format code (%%%c) for type"), mode);
         default:
            mcsl::__throw(mcsl::ErrCode::FS_ERR, mcsl::FMT("invalid format code (%%%c)"), mode);
         case 'e': case 'f': case 'g':
            return mcsl::writef(file, (mcsl::to_float_t<T>)num, mode, fmt);
         case 'i': return mcsl::writef(file, (mcsl::to_sint_t<T>)num, mode, fmt);
         case 'u': 
            fmt.radix = fmt.radix ? fmt.radix : mcsl::DEFAULT_INT_RADIX;
            break;
         case 'r':
            return writefRawImpl(file, {(ubyte*)&num, sizeof(num)}, mode & mcsl::CASE_BIT, fmt);
         case 'b':
            return writefBinaryImpl(file, {(ubyte*)&num, sizeof(num)}, fmt);

      }
      uint charsPrinted = 0;

      //check radix
      switch (fmt.radix) {
         default:
            mcsl::__throw(mcsl::ErrCode::FS_ERR, mcsl::FMT("unsupported radix for printing unsigned integers: %u"), fmt.radix);
         case 2: case 8: case 10: case 16:
            break;
      }

      const bool isLower = mode & mcsl::CASE_BIT;
      
      //calculate digit string
      auto digits = mcsl::uint_to_str(num, fmt.radix, isLower);

      //minWidth with right justification
      if (!fmt.isLeftJust) {
         sint padChars = fmt.minWidth - mcsl::max(fmt.precision, digits.size()) - (fmt.alwaysPrintSign || fmt.padForPosSign) - (2 * fmt.altMode) - charsPrinted;
         if (padChars > 0) { //!TODO: if(fmt.padWithZero) write this padding after the radix specifier
            file.write(fmt.padWithZero ? '0' : mcsl::PAD_CHAR, padChars);
            charsPrinted += padChars;
         }
      }

      //print sign
      bool printSignZero = false;
      if (fmt.alwaysPrintSign) {
         file.write('+');
         ++charsPrinted;
      } else if (fmt.padForPosSign) {
         if (fmt.padWithZero) {
            printSignZero = true;
         } else {
            file.write(mcsl::PAD_CHAR);
            ++charsPrinted;
         }
      }

      //altMode - print radix specifier
      __ALT_MODE;

      //print sign zero
      if (printSignZero) {
         file.write('0');
         ++charsPrinted;
      }
      //print extra precision digits
      if (fmt.precision > digits.size()) {
         file.write('0', fmt.precision - digits.size());
         charsPrinted += fmt.precision - digits.size();
      }

      //print digit string
      file.write(mcsl::str_slice{digits});
      charsPrinted += digits.size();

      //minWidth with left justification
      if (fmt.isLeftJust && fmt.minWidth > charsPrinted) {
         file.write(mcsl::PAD_CHAR, fmt.minWidth - charsPrinted);
         charsPrinted += fmt.minWidth - charsPrinted;
      }

      //return number of printed characters
      return charsPrinted;
   }

   template<mcsl::sint_t T> uint writefImpl(mcsl::File& file, T num, char mode, mcsl::FmtArgs& fmt) {
      switch (mode | mcsl::CASE_BIT) {
         case 'c':
            __PRINT_AS_CHARS
         [[fallthrough]];
         case 's':
            mcsl::__throw(mcsl::ErrCode::FS_ERR, mcsl::FMT("invalid format code for type (%%%c)"), mode);
         default:
            mcsl::__throw(mcsl::ErrCode::FS_ERR, mcsl::FMT("invalid format code (%%%c)"), mode);
         case 'e': case 'f': case 'g':
            return mcsl::writef(file, (mcsl::to_float_t<T>)num, mode, fmt);
         case 'i': 
            fmt.radix = fmt.radix ? fmt.radix : mcsl::DEFAULT_INT_RADIX;
            break;
         case 'u': return mcsl::writef(file, (mcsl::to_uint_t<T>)num, mode, fmt);
         case 'r':
            return writefRawImpl(file, {(ubyte*)&num, sizeof(num)}, mode & mcsl::CASE_BIT, fmt);
         case 'b':
            return writefBinaryImpl(file, {(ubyte*)&num, sizeof(num)}, fmt);

      }
      uint charsPrinted = 0;

      //check radix
      switch (fmt.radix) {
         default:
            mcsl::__throw(mcsl::ErrCode::FS_ERR, mcsl::FMT("unsupported radix for printing unsigned integers: %u"), fmt.radix);
         case 2: case 8: case 10: case 16:
            break;
      }

      const bool isLower = mode & mcsl::CASE_BIT;
      
      //calculate digit string
      auto digits = mcsl::uint_to_str(mcsl::abs(num), fmt.radix, isLower); //!NOTE: inefficiency from some extra copying, but that should be fixable with more agressive copy elision in the language

      //minWidth with right justification
      if (!fmt.isLeftJust) {
         sint padChars = fmt.minWidth - mcsl::max(fmt.precision, digits.size()) - (num < 0 || fmt.alwaysPrintSign || fmt.padForPosSign) - (2 * fmt.altMode) - charsPrinted;
         if (padChars > 0) { //!TODO: if(fmt.padWithZero) write this padding after the radix specifier
            file.write(fmt.padWithZero ? '0' : mcsl::PAD_CHAR, padChars);
            charsPrinted += padChars;
         }
      }

      //print sign
      bool printSignZero = false;
      if (num < 0) {
         file.write('-');
         ++charsPrinted;
      } else if (fmt.alwaysPrintSign) {
         file.write('+');
         ++charsPrinted;
      } else if (fmt.padForPosSign) {
         if (fmt.padWithZero) {
            printSignZero = true;
         } else {
            file.write(mcsl::PAD_CHAR);
            ++charsPrinted;
         }
      }

      //altMode - print radix specifier
      __ALT_MODE;

      //print sign zero
      if (printSignZero) {
         file.write('0');
         ++charsPrinted;
      }

      //print extra precision digits
      if (fmt.precision > digits.size()) {
         charsPrinted += fmt.precision - digits.size();
         file.write('0', fmt.precision - digits.size());
      }
      //print digit string
      charsPrinted += digits.size();
      file.write(mcsl::str_slice{digits});

      //minWidth with left justification
      if (fmt.isLeftJust && fmt.minWidth > charsPrinted) {
         file.write(mcsl::PAD_CHAR, fmt.minWidth - charsPrinted);
         charsPrinted = fmt.minWidth;
      }

      //return number of printed characters
      return charsPrinted;
   }

   template<mcsl::float_t T> uint writefImpl(mcsl::File& file, T num, char mode, mcsl::FmtArgs& fmt) {
      switch (mode | mcsl::CASE_BIT) {
         case 'c': case 's': case 'i': case 'u':
            mcsl::__throw(mcsl::ErrCode::FS_ERR, mcsl::FMT("invalid format code for type (%%%c)"), mode);
         default:
            mcsl::__throw(mcsl::ErrCode::FS_ERR, mcsl::FMT("invalid format code (%%%c)"), mode);
         case 'e': case 'f': case 'g':
            fmt.radix = fmt.radix ? fmt.radix : mcsl::DEFAULT_FLOAT_RADIX;
            break;
         case 'r':
            return writefRawImpl(file, {(ubyte*)&num, sizeof(num)}, mode & mcsl::CASE_BIT, fmt);
         case 'b':
            return writefBinaryImpl(file, {(ubyte*)&num, sizeof(num)}, fmt);

      }
      uint charsPrinted = 0;
      const bool isLower = mode & mcsl::CASE_BIT;

      //check radix
      switch (fmt.radix) {
         default:
            mcsl::__throw(mcsl::ErrCode::FS_ERR, mcsl::FMT("unsupported radix for printing floating-point numbers: %u"), fmt.radix);
         case 2: case 8: case 10: case 16:
            break;
      }

      //special cases
      if (mcsl::isNaN(num)) { //NaN
         return mcsl::writef(file, __NAN_STR, isLower ? 's' : 'S', fmt);
      } else if (mcsl::isInf(num)) { //Inf, -Inf
         return mcsl::writef(file, num > 0 ? (fmt.alwaysPrintSign ? __POS_INF_STR : __INF_STR) : __NEG_INF_STR, isLower ? 's' : 'S', fmt);
      }

      //decide between %e and %f if using %g, and calculate expected width (for use in padding)
      T signif;
      sint pow;
      uint expectedCharCount;
      switch (mode | mcsl::CASE_BIT) {
         case 'g':
            {
               auto [tmpSig, tmpPow] = mcsl::sci_notat<T>(num, fmt.radix);
               signif = tmpSig;
               pow = tmpPow; 
            }

            if (!fmt.precision) {
               fmt.precision = 1;
            }

            if ((slong)pow >= -4 && pow < (slong)fmt.precision) {
               fmt.precision = fmt.precision - pow - 1;
               mode = isLower ? 'f' : 'F';
               goto case_f;
            } else {
               fmt.precision = fmt.precision - 1;
               mode = isLower ? 'e' : 'E';
               goto case_e;
            }

         case 'e': //4 + precision + (bool)precision + ceil(log(10,pow))
            {
               auto [tmpSig, tmpPow] = mcsl::sci_notat<T>(num, fmt.radix);
               signif = tmpSig;
               pow = tmpPow; 
            }
            case_e:
            expectedCharCount = 5 + fmt.precision + (bool)fmt.precision + (uint)mcsl::floor(mcsl::log(fmt.radix, mcsl::abs(pow)));
            break;
         case 'f': case_f:
            expectedCharCount = 1 + (uint)mcsl::floor(mcsl::log(fmt.radix, mcsl::round(mcsl::abs(num)))) + fmt.precision + (bool)fmt.precision;
            break;
      }
      expectedCharCount += (num < 0 || fmt.alwaysPrintSign); //sign
      expectedCharCount += fmt.altMode ? 2 : 0; //radix specifier

      //padding (right-justified)
      if (!fmt.isLeftJust && !fmt.padWithZero) {
         sint padChars = fmt.minWidth - expectedCharCount;
         if (padChars > 0) {
            file.write(mcsl::PAD_CHAR, padChars);
            charsPrinted += padChars;
         }
      }

      //sign
      bool extraZero = false;
      if (num < 0) {
         file.write('-');
         ++charsPrinted;
      } else if (fmt.alwaysPrintSign) {
         file.write('+');
         ++charsPrinted;
      } else if (fmt.padForPosSign) {
         if (fmt.padWithZero) {
            extraZero = true;
         } else {
            file.write(mcsl::PAD_CHAR);
            ++charsPrinted;
         }
      }

      //altMode - print radix specifier
      __ALT_MODE;

      if (extraZero) {
         file.write('0');
         ++charsPrinted;
      }
      //padding (right-justified, padWithZero)
      if (!fmt.isLeftJust && fmt.padWithZero) {
         sint padChars = fmt.minWidth - expectedCharCount;
         if (padChars > 0) {
            file.write('0', padChars);
            charsPrinted += padChars;
         }
      }

      switch (mode | mcsl::CASE_BIT) { //!TODO: minWidth, check that the precision and radix can fit in a ulong
         case 'f': {
            num = mcsl::round(num, fmt.precision, fmt.radix);
            auto [whole, frac] = mcsl::modf(num);

            //calculate and print digit string of the whole part
            whole = mcsl::abs(whole);
            mcsl::string wholeDigits;
            wholeDigits.reserve_exact((uint)mcsl::ceil(mcsl::log(fmt.radix, whole)));
            do {
               const ubyte digit = (ubyte)mcsl::mod(whole, fmt.radix);
               whole = mcsl::modf(whole / fmt.radix).first;
               wholeDigits.push_back(mcsl::digit_to_char(digit, isLower));
            } while (whole > 0);

            for (uint i = wholeDigits.size(); i--;) {
               file.write(wholeDigits[i]);
            }

            // file.write(mcsl::str_slice{wholeDigits});
            charsPrinted += wholeDigits.size();
            wholeDigits.free();

            if (fmt.precision > 0) { //fractional part
               file.write('.');
               ulong fracInt = frac * mcsl::pow(fmt.radix, fmt.precision);
               auto fracDigits = mcsl::uint_to_str(fracInt, fmt.radix, isLower);
               if (fmt.precision > fracDigits.size()) {
                  file.write('0', fmt.precision - fracDigits.size());
               }
               file.write(fracDigits.slice());
               
               charsPrinted += fmt.precision + 1;
            }

            //!TODO: all format args besides radix

            break;
         }
         case 'e': {
            //get fields
            // auto [signif, pow] = mcsl::sci_notat<T>(num, fmt.radix);
            signif = mcsl::round(signif, fmt.precision, fmt.radix);
            auto [signifWhole, signifFrac] = mcsl::modf(mcsl::abs(signif));

            //whole part
            file.write(mcsl::digit_to_char((ubyte)signifWhole, isLower));
            ++charsPrinted;
            ulong fracInt = 0;
            if (fmt.precision > 0) { //fractional part
               file.write('.');
               fracInt = signifFrac * mcsl::pow(fmt.radix, fmt.precision);
               auto fracDigits = mcsl::uint_to_str(fracInt, fmt.radix, isLower);
               if (fmt.precision > fracDigits.size()) {
                  file.write('0', fmt.precision - fracDigits.size());
               }
               file.write(fracDigits.slice());
               
               charsPrinted += fmt.precision + 1;
            }
            
            //scientific notation operator
            file.write(__EXP_NOTAT);
            charsPrinted += __EXP_NOTAT.size();

            //exponent //!TODO: when handling padWithZero, don't forget about this
            if ((ubyte)signifWhole == 0 && fracInt == 0) { //0.0
               file.write('+');
               file.write('0');
               charsPrinted += 2;
            } else { //anything else
               auto expDigits = mcsl::sint_to_str(pow, fmt.radix, isLower, true);
               file.write(expDigits.slice());
               charsPrinted += expDigits.size();
            }
         }
         
         break;
      }

      //padding (left-justified)
      if (fmt.isLeftJust) {
         sint padChars = fmt.minWidth - expectedCharCount;
         if (padChars > 0) {
            file.write(mcsl::PAD_CHAR, padChars);
            charsPrinted += padChars;
         }
      }
      
      //return number of chars printed
      return charsPrinted;
   }

   //formatted human-readable writing of a byte string - as ASCII strings, with spaces between bytes
   uint writefRawImpl(mcsl::File& file, const mcsl::arr_span<ubyte> data, bool isLowercase, mcsl::FmtArgs& fmt) {
      fmt.radix = fmt.radix ? fmt.radix : mcsl::DEFAULT_RAW_RADIX;

      const uint charsPerByte = (uint)(mcsl::ceil(mcsl::log(fmt.radix, mcsl::TYPEMAX<ubyte>()+1)));
      uint charsPrinted = data.size() * (charsPerByte + (fmt.altMode ? 3 : fmt.padForPosSign)) - 1;


      //minWidth with right justification
      if (!fmt.isLeftJust && fmt.minWidth > charsPrinted) {
         file.write(mcsl::PAD_CHAR, fmt.minWidth - charsPrinted);
         charsPrinted = fmt.minWidth;
      }

      //write bytes
      if (data.size()) {
         //make format
         mcsl::FmtArgs byteFmt = fmt;
         byteFmt.minWidth = 0;
         byteFmt.precision = charsPerByte;
         byteFmt.alwaysPrintSign = false;
         byteFmt.padForPosSign = false;
         byteFmt.padWithZero = false;

         //write bytes to the file
         const char mode = isLowercase ? 'u' : 'U';
         if (fmt.endianness_r() == mcsl::sys_endian::SYS) {
            writefImpl<ubyte>(file, data.back(), mode, byteFmt);
            if (data.size() > 1) {
               byteFmt.padForPosSign = fmt.padForPosSign || fmt.altMode; //add spacing between bytes if necessary
               for (uint i = data.size()-1; i--;) {
                  writefImpl<ubyte>(file, data[i], mode, byteFmt);
               }
            }
         } else { //ANTI_SYS
            writefImpl<ubyte>(file, data[0], mode, byteFmt);
            if (data.size() > 1) {
               byteFmt.padForPosSign = fmt.padForPosSign || fmt.altMode; //add spacing between bytes if necessary
               for (uint i = 1; i < data.size(); ++i) {
                  writefImpl<ubyte>(file, data[i], mode, byteFmt);
               }
            }
         }
      }
      
      
      //minWidth with right justification
      if (!fmt.isLeftJust && fmt.minWidth > charsPrinted) {
         file.write(mcsl::PAD_CHAR, fmt.minWidth - charsPrinted);
         charsPrinted = fmt.minWidth;
      }

      //return number of chars printed
      return charsPrinted;
   }
};

#define _writefImpl(T)                                               \
uint mcsl::writef(File& file, const T obj, char mode, FmtArgs fmt) { \
   return writefImpl<T>(file, obj, mode, fmt);                       \
}

MCSL_MAP(_writefImpl, MCSL_ALL_NUM_T)

#undef _writefImpl

uint mcsl::writef(File& file, const void* obj, char mode, FmtArgs fmt) {
   return writef(file, (uptr)obj, mode, fmt);
}

uint mcsl::writef(File& file, const char ch, char mode, FmtArgs fmt) { //!TODO: consider locale
   switch (mode | CASE_BIT) {
      case 'u': case 'r':
         return writef(file, (ubyte)ch, mode, fmt);
      case 'i':
         return writef(file, (sbyte)ch, mode, fmt);
      case 'e': case 'f': case 'g':
         return writef(file, (float)ch, mode, fmt);
      default:
         __throw(ErrCode::FS_ERR, mcsl::FMT("invalid format code (%%%c)"), mode);
      case 'c': case 's':
         break;
   }

   //minWidth with right justification
   if (!fmt.isLeftJust && fmt.minWidth > 1) {
      file.write(PAD_CHAR, fmt.minWidth - 1);
   }

   //write char
   file.write(ch);

   //minWidth with left justification
   if (fmt.isLeftJust && fmt.minWidth > 1) {
      file.write(PAD_CHAR, fmt.minWidth - 1);
   }

   //return number of chars printed
   return max(1, fmt.minWidth);
}

//!TODO: implement these
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
uint mcsl::writef(File& file, const wchar ch, char mode, FmtArgs fmt) {
   __throw(ErrCode::FS_ERR, FMT("printing `wchar`s is not yet supported"));
}
uint mcsl::writef(File& file, const char8 ch, char mode, FmtArgs fmt) {
   __throw(ErrCode::FS_ERR, FMT("printing `char8`s is not yet supported"));
}
uint mcsl::writef(File& file, const char16 ch, char mode, FmtArgs fmt) {
   __throw(ErrCode::FS_ERR, FMT("printing `char16`s is not yet supported"));
}
uint mcsl::writef(File& file, const char32 ch, char mode, FmtArgs fmt) {
   __throw(ErrCode::FS_ERR, FMT("printing `char32`s is not yet supported"));
}
#pragma GCC diagnostic pop

uint mcsl::writef(File& file, const bool obj, char mode, FmtArgs fmt) {
   raw_buf_str<8> str;
   const bool isLower = mode & CASE_BIT;
   switch (mode | CASE_BIT) {
      case 'u': case 'i': case 'r':
         return writef(file, (ubyte)obj, mode, fmt);
      case 'e': case 'f': case 'g':
         return writef(file, (to_float_t<bool>)obj, mode, fmt);
      case 's':
         if (fmt.altMode) {
            if (obj) { str = raw_str{isLower ? "yes" : "YES"}; }
            else     { str = raw_str{isLower ?  "no" : "NO" }; }
         } else {
            if (obj) { str = raw_str{isLower ?  "true" : "TRUE" }; }
            else     { str = raw_str{isLower ? "false" : "FALSE"}; }
         }
         break;

      case 'c':
         if (fmt.altMode) {
            if (obj) { str = raw_str{isLower ? "y" : "Y"}; }
            else     { str = raw_str{isLower ? "n" : "N"}; }
         } else {
            if (obj) { str = raw_str{isLower ? "t" : "T"}; }
            else     { str = raw_str{isLower ? "f" : "F"}; }
         }
         break;
      
      default:
         mcsl::__throw(ErrCode::FS_ERR, mcsl::FMT("invalid format code (%%%c)"), mode);
   }

   //right-justified padding
   if (!fmt.isLeftJust && fmt.minWidth > str.size()) {
      file.write(PAD_CHAR, fmt.minWidth - str.size());
   }

   //bool string
   file.write(str_slice{str});

   //left-justified padding
   if (fmt.isLeftJust && fmt.minWidth > str.size()) {
      file.write(PAD_CHAR, fmt.minWidth - str.size());
   }

   //return number of chars printed
   return max(str.size(), fmt.minWidth);
}

uint mcsl::writef(File& file, const str_slice obj, char mode, FmtArgs fmt) {
   if ((mode | CASE_BIT) != 's') {
      mcsl::__throw(mcsl::ErrCode::FS_ERR, mcsl::FMT("invalid format code for type (%%%c)"), mode);
   }

   const str_slice str = obj.slice(fmt.precision ? min(obj.size(), fmt.precision) : obj.size());

   if (!fmt.isLeftJust && fmt.minWidth > str.size()) {
      file.write(PAD_CHAR, fmt.minWidth - str.size());
   }

   file.write(str);

   if (fmt.isLeftJust && fmt.minWidth > str.size()) {
      file.write(PAD_CHAR, fmt.minWidth - str.size());
   }

   return max(str.size(), fmt.minWidth);
}

#undef __PRINT_AS_CHARS
#undef __PRINT_AS_CHAR
#undef __ALT_MODE

#include "MAP_MACRO_UNDEF.h"

#endif //FS_WRITEF_CPP