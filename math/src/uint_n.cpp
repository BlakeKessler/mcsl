#ifndef MCSL_UINT_N_CPP
#define MCSL_UINT_N_CPP

#include "uint_n.hpp"
#include "carry.hpp"

#include <cstdio>

template<uint _capacity> uint mcsl::uint_n<_capacity>::bit_width() {
   return 8 * _size + std::bit_width(_buf[_size]);
}

template<uint _capacity> mcsl::pair<ulong, ushort> mcsl::uint_n<_capacity>::get_msw() {
   switch (_size) {
      case 0: return mcsl::pair<ulong, ushort>{0,0};
      case 1: return mcsl::pair<ulong, ushort>{_buf[0], 0};
      default:
         mcsl::pair <ulong, ushort> msw;
         ulong* back = end() - 1;
         uoverlong* ptr = (uoverlong*)(back - 1);
         uint bitWidth = std::bit_width(*back);
         msw.first = (ulong)((*ptr) >> bitWidth);
         msw.second = (sizeof(ulong) * 8) - bitWidth;
         return msw;
   }
}

template<uint _capacity> mcsl::uint_n<_capacity>& mcsl::uint_n<_capacity>::operator+=(const ulong x) {
   uint carry = x;
   for (uint i = 0; carry && i < _size; ++i) {
      carry = carry::ADD(_buf[i], carry);
   }
   if (carry) {
      if (_size >= _capacity) {
         mcsl_throw(ErrCode::INT_OVERFLOW, "mcsl::uint_n<%u> overflow", _capacity);
      }
      _buf[_size] = carry;
      ++_size;
   }

   return self;
}

template<uint _capacity> mcsl::uint_n<_capacity>& mcsl::uint_n<_capacity>::operator*=(const ulong x) {
   ulong carry = 0;
   for (uint i = 0; i < _size; ++i) {
      ulong tmp = carry;
      carry = carry::MUL(_buf[i], x);
      _buf[i] += tmp;
   }
   if (carry) {
      if (_size >= _capacity) {
         mcsl_throw(ErrCode::INT_OVERFLOW, "mcsl::uint_n<%u> overflow", _capacity);
      }
      _buf[_size] = carry;
      ++_size;
   }

   return self;
}

template<uint _capacity> void mcsl::uint_n<_capacity>::print() {
   uint i = _size;
   std::printf("%lx", _buf[--i]);
   while (i) {
      std::printf(" %0lx", _buf[--i]);
   }
}

#endif //MCSL_UINT_N_CPP