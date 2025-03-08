#ifndef FS_PRINTF_CPP
#define FS_PRINTF_CPP

#include "fs.hpp"

#include "str_to_num.hpp"
#include "algebra.hpp"

template<> uint mcsl::File::writef<ulong>(const ulong& obj, char mode, FmtArgs fmt) {
   switch (mode | CASE_BIT) {
      case 'c': case 's':
         __throw(ErrCode::FS_ERR, "invalid format code for type `ulong` (%%%c)", mode);
      default:
         __throw(ErrCode::FS_ERR, "invalid format code (%%%c)", mode);
      case 'e': case 'f': case 'g':
         return writef<fext>(obj, mode, fmt);
      case 'i': case 'u': 
         fmt.radix = fmt.radix ? fmt.radix : DEFAULT_INT_RADIX;
         break;
      case 'r':
         fmt.radix = fmt.radix ? fmt.radix : DEFAULT_RAW_RADIX;
         fmt.precision = max(fmt.precision, (uint)(ceil(sizeof(ulong)*log(fmt.radix, TYPEMAX<ubyte>()+1)))); //number of bytes * digits per byte
         break;
   }
   //!TODO: check mode
   uint charsPrinted = 0;

   //check radix
   switch (fmt.radix) {
      default:
         __throw(ErrCode::FS_ERR, "unsupported radix for printing unsigned integers: %u", fmt.radix);
      case 2: case 8: case 10: case 16:
         break;
   }

   const bool isLower = mode & CASE_BIT;
   
   //calculate digit string
   char digits[sizeof(ulong) * 8];
   uint digitCount = 0;
   uint rest = obj;
   do {
      const ubyte digit = rest % fmt.radix;
      rest /= fmt.radix;
      digits[digitCount++] = digit_to_char(digit, isLower);
   } while (rest);

   //minWidth with right justification
   if (!fmt.isLeftJust) {
      sint padChars = fmt.minWidth - max(fmt.precision, digitCount) - fmt.alwaysPrintSign - (2 * fmt.altMode) - charsPrinted;
      if (padChars > 0) {
         charsPrinted += padChars;
         do {
            write(PAD_CHAR);
         } while (--padChars);
      }
   }

   //altMode - print radix specifier
   if (fmt.altMode) {
      write('0');
      char ch = isLower ? 0 : CASE_BIT;
      switch (fmt.radix) {
         case  2: ch |= 'B'; break;
         case  8: ch |= 'O'; break;
         case 10: ch |= 'D'; break;
         case 16: ch |= 'X'; break;
         default: UNREACHABLE;
      }
      write(ch);
      charsPrinted += 2;
   }

   //print sign
   if (fmt.alwaysPrintSign) {
      write('+');
      ++charsPrinted;
   }
   //print extra precision digits
   {
      sint extraZeros = fmt.precision - digitCount;
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
   if (fmt.isLeftJust) {
      sint padChars = fmt.minWidth - charsPrinted;
      if (padChars > 0) {
         charsPrinted = fmt.minWidth;
         do {
            write(PAD_CHAR);
         } while (--padChars);
      }
   }

   //return number of printed characters
   return charsPrinted;
}



template<> uint mcsl::File::writef<char>(const char& obj, char mode, FmtArgs fmt) {
   switch (mode | CASE_BIT) {
      case 'u': case 'r':
         return writef<ubyte>(obj, mode, fmt);
      case 'i':
         return writef<sbyte>(obj, mode, fmt);
      case 'e': case 'f': case 'g':
         return writef<float>(obj, mode, fmt);
      default:
         __throw(ErrCode::FS_ERR, "invalid format code (%%%c)", mode);
      case 'c': case 's':
         break;
   }

   if (!fmt.isLeftJust) { //minWidth with right justification
      sint padChars = fmt.minWidth;
      while (--padChars > 0) {
         write(PAD_CHAR);
      }
   }

   //write char
   write(obj);

   if (fmt.isLeftJust) { //minWidth with left justification
      sint padChars = fmt.minWidth;
      while (--padChars > 0) {
         write(PAD_CHAR);
      }
   }

   //return number of chars printed
   return max(1, fmt.minWidth);
}

#endif //FS_PRINTF_CPP