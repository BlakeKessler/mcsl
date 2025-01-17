//https://treeniks.github.io/x86-64-simplified/instructions/binary-arithmetic-instructions/mul.html
//https://developer.arm.com/documentation/dui0473/m/arm-and-thumb-instructions/umull
//https://developer.arm.com/documentation/ddi0602/2021-12/Base-Instructions/UMULH--Unsigned-Multiply-High-

//https://gcc.gnu.org/onlinedocs/gcc/_005f_005fint128.html

#pragma once
#ifndef MCSL_UINT_N_HPP
#define MCSL_UINT_N_HPP

#include "MCSL.hpp"

#include "pair.hpp"
#include "contig_base.hpp"

template <uint _capacity> struct mcsl::uint_n : mcsl::contig_base<ulong> {
   private:
      uint _size; //number of non-zero elements in _buf
      ulong _buf[_capacity];

      static constexpr const char _nameof[] = "uint_n";
   public:
      uint_n():_size{0},_buf{} {}
      uint_n(const ulong x):_size{1}, _buf{x} {}
      template<uint _otherCapacity> requires (_otherCapacity <= _capacity) uint_n(const uint_n<_otherCapacity>& other):_buf{other._buf},_size{other._size} {}

      uint bit_width();
      mcsl::pair<ulong, ushort> get_msw(); //get most significant word, paired with the number of bits it was shifted by to fit it into a single ulong

      uint_n& operator+=(const ulong x);
      // uint_n& operator-=(const ulong x);
      uint_n& operator*=(const ulong x);
      // uint_n& operator/=(const ulong x);

      ulong* data() { return _buf; }
      ulong* begin() { return _buf; }
      ulong** ptr_to_buf() { return &_buf; }
      uint size() { return _size; }



      void print();
};

#include "../src/uint_n.cpp"

#endif //MCSL_UINT_N_HPP