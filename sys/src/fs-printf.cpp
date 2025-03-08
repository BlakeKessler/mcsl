#ifndef FS_PRINTF_CPP
#define FS_PRINTF_CPP

#include "fs.hpp"

#include "str_to_num.hpp"

template<> uint mcsl::File::printf<ulong>(const ulong& obj,
   const char mode,
   uint radix,
   const uint minWidth,
   const uint precision,
   const bool isLeftJust,
   const bool alwaysPrintSign,
   const bool altMode
) {
   //!TODO: check mode
   uint charsPrinted = 0;

   //check radix
   switch (radix) {
      default:
         __throw(ErrCode::FS_ERR, "unsupported radix for printing unsigned integers: %u", radix);
      case 0:
         radix = DEFAULT_INT_RADIX;
      case 2: case 8: case 10: case 16:
         break;
   }

   const bool isUpper = mode & CASE_BIT;
   
   //calculate digit string
   char digits[sizeof(ulong) * 8];
   uint digitCount = 0;
   uint rest = obj;
   do {
      const ubyte digit = rest % radix;
      rest /= radix;
      digits[digitCount++] = digit_to_char(digit, isUpper);
   } while (rest);

   //minWidth with right justification
   if (!isLeftJust) {
      sint padChars = minWidth - max(precision, digitCount) - alwaysPrintSign - (2 * altMode) - charsPrinted;
      if (padChars > 0) {
         charsPrinted += padChars;
         do {
            write(PAD_CHAR);
         } while (--padChars);
      }
   }

   //altMode - print radix specifier
   if (altMode) {
      write('0');
      char ch = isUpper ? 0 : CASE_BIT;
      switch (radix) {
         case  2: ch |= 'b'; break;
         case  8: ch |= 'o'; break;
         case 10: ch |= 'd'; break;
         case 16: ch |= 'x'; break;
         default: UNREACHABLE;
      }
      write(ch);
      charsPrinted += 2;
   }

   //print sign
   if (alwaysPrintSign) {
      write('+');
      ++charsPrinted;
   }
   //print extra precision digits
   {
      sint extraZeros = precision - digitCount;
      if (extraZeros > 0) {
         charsPrinted += extraZeros;
         do {
            write('0');
         } while (--extraZeros);
      }
   }
   //print digit string
   charsPrinted += digitCount;
   while (digitCount--) {
      write(digits[digitCount]);
   }

   //minWidth with left justification
   if (isLeftJust) {
      sint padChars = minWidth - charsPrinted;
      if (padChars > 0) {
         charsPrinted = minWidth;
         do {
            write(PAD_CHAR);
         } while (--padChars);
      }
   }

   //return number of printed characters
   return charsPrinted;
}

#endif //FS_PRINTF_CPP