#pragma once
#ifndef MCSL_FLOAT_FIELDS_HPP
#define MCSL_FLOAT_FIELDS_HPP

#include "MCSL.hpp"

#include "type_traits.hpp"

template<mcsl::float_t T> struct mcsl::Float {
   public:
      using int_type = to_uint_t<T>;

      static constexpr uint RADIX = FLT_RADIX;
      static constexpr sint EXP_BIAS;

      static constexpr int_type SIGN_BITS = 1;
      static constexpr int_type MANT_BITS;
      static constexpr int_type EXP_BITS;

      static constexpr int_type SIGN_POS = MANT_BITS + EXP_BITS;
      static constexpr int_type MANT_POS = 0;
      static constexpr int_type EXP_POS = MANT_BITS;
      
      static constexpr int_type SIGN_MASK = ((1 << SIGN_BITS) - 1) << SIGN_POS;
      static constexpr int_type MANT_MASK = ((1 << MANT_BITS) - 1) << MANT_POS;
      static constexpr int_type EXP_MASK = ((1 << EXP_BITS) - 1) << EXP_POS;
   private:
      T _float;
   public:
      constexpr Float(const T f = {}):_float{f} {}

      constexpr int_type bits() const { return std::bit_cast<int_type>(_float); }

      constexpr int_type rawSignBit() const { return bits() & SIGN_MASK; } //result of masking away all but the sign bit
      constexpr int_type rawMant() const { return bits() & MANT_MASK; } //result of masking away all but the mantissa
      constexpr int_type rawExp() const { return bits() & EXP_MASK; } //result of masking away all but the exponent

      constexpr int_type mant() const { return rawMant() >> MANT_POS; } //mantissa bits, shifted so that the first bit of the mantissa is in the first bit of the int_type (without the implicit 1)
      constexpr int_type biasedExp() const { return rawExp() >> EXP_POS; } //biased exponent

      constexpr int_type signBit() const { return rawSignBit() ? 1 : 0; } //0 or 1
      constexpr int_type significand() const { return mant() | (1 << (MANT_BITS + MANT_POS)); } //mantissa bits, shifted so that the first bit of the mantissa is in the first bit of the int_type, and with the implicit 1 bit bitwise OR'ed in
      constexpr int_type exp() const { return biasedExp() - EXP_BIAS; } //unbiased exponent

      constexpr int_type significandExp() const { return biasedExp() - (EXP_BIAS + MANT_BITS)} //unbiased exponent to convert significand() back to the original value

      constexpr operator T() const { return _float; }
};

#endif //MCSL_FLOAT_FIELDS_HPP