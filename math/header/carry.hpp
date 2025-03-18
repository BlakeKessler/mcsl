#pragma once
#ifndef MCSL_CARRY_HPP
#define MCSL_CARRY_HPP

#include "MCSL.hpp"

namespace mcsl {
   namespace carry {
      //! +, return carry flag
      template<bool isError = false> [[gnu::always_inline, gnu::pure]] inline bool ADD ( sint lhs,  sint rhs,  sint *res);
      template<bool isError = false> [[gnu::always_inline, gnu::pure]] inline bool ADD (slong lhs, slong rhs, slong *res);
      template<bool isError = false> [[gnu::always_inline, gnu::pure]] inline bool ADD ( uint lhs,  uint rhs,  uint *res);
      template<bool isError = false> [[gnu::always_inline, gnu::pure]] inline bool ADD (ulong lhs, ulong rhs, ulong *res);
      //! +=, return carry flag
      template<bool isError = false> [[gnu::always_inline, gnu::pure]] inline bool ADD ( sint& lhs,  sint rhs) { return ADD<isError>(lhs, rhs, &lhs); }
      template<bool isError = false> [[gnu::always_inline, gnu::pure]] inline bool ADD (slong& lhs, slong rhs) { return ADD<isError>(lhs, rhs, &lhs); }
      template<bool isError = false> [[gnu::always_inline, gnu::pure]] inline bool ADD ( uint& lhs,  uint rhs) { return ADD<isError>(lhs, rhs, &lhs); }
      template<bool isError = false> [[gnu::always_inline, gnu::pure]] inline bool ADD (ulong& lhs, ulong rhs) { return ADD<isError>(lhs, rhs, &lhs); }


      //! -, return carry flag
      template<bool isError = false> [[gnu::always_inline, gnu::pure]] inline bool SUB ( sint lhs,  sint rhs,  sint *res);
      template<bool isError = false> [[gnu::always_inline, gnu::pure]] inline bool SUB (slong lhs, slong rhs, slong *res);
      template<bool isError = false> [[gnu::always_inline, gnu::pure]] inline bool SUB ( uint lhs,  uint rhs,  uint *res);
      template<bool isError = false> [[gnu::always_inline, gnu::pure]] inline bool SUB (ulong lhs, ulong rhs, ulong *res);
      //! -=, return carry flag
      template<bool isError = false> [[gnu::always_inline, gnu::pure]] inline bool SUB ( sint& lhs,  sint rhs) { return SUB<isError>(lhs, rhs, &lhs); }
      template<bool isError = false> [[gnu::always_inline, gnu::pure]] inline bool SUB (slong& lhs, slong rhs) { return SUB<isError>(lhs, rhs, &lhs); }
      template<bool isError = false> [[gnu::always_inline, gnu::pure]] inline bool SUB ( uint& lhs,  uint rhs) { return SUB<isError>(lhs, rhs, &lhs); }
      template<bool isError = false> [[gnu::always_inline, gnu::pure]] inline bool SUB (ulong& lhs, ulong rhs) { return SUB<isError>(lhs, rhs, &lhs); }


      //! *, return carry flag
      template<bool isError = false> [[gnu::always_inline, gnu::pure]] inline sint  MUL ( sint lhs,  sint rhs,  sint *res);
      template<bool isError = false> [[gnu::always_inline, gnu::pure]] inline slong MUL (slong lhs, slong rhs, slong *res);
      template<bool isError = false> [[gnu::always_inline, gnu::pure]] inline uint  MUL ( uint lhs,  uint rhs,  uint *res);
      template<bool isError = false> [[gnu::always_inline, gnu::pure]] inline ulong MUL (ulong lhs, ulong rhs, ulong *res);
      //! *=, return carry flag
      template<bool isError = false> [[gnu::always_inline, gnu::pure]] inline sint  MUL ( sint& lhs,  sint rhs) { return MUL<isError>(lhs, rhs, &lhs); }
      template<bool isError = false> [[gnu::always_inline, gnu::pure]] inline slong MUL (slong& lhs, slong rhs) { return MUL<isError>(lhs, rhs, &lhs); }
      template<bool isError = false> [[gnu::always_inline, gnu::pure]] inline uint  MUL ( uint& lhs,  uint rhs) { return MUL<isError>(lhs, rhs, &lhs); }
      template<bool isError = false> [[gnu::always_inline, gnu::pure]] inline ulong MUL (ulong& lhs, ulong rhs) { return MUL<isError>(lhs, rhs, &lhs); }
   };
}

// #include "../src/carry.cpp"
template<> bool mcsl::carry::ADD<false>( sint lhs,  sint rhs,  sint *res);
template<> bool mcsl::carry::ADD<false>(slong lhs, slong rhs, slong *res);
template<> bool mcsl::carry::ADD<false>( uint lhs,  uint rhs,  uint *res);
template<> bool mcsl::carry::ADD<false>(ulong lhs, ulong rhs, ulong *res);
//! +=, return carry flag
template<> bool mcsl::carry::ADD<false>( sint& lhs,  sint rhs);
template<> bool mcsl::carry::ADD<false>(slong& lhs, slong rhs);
template<> bool mcsl::carry::ADD<false>( uint& lhs,  uint rhs);
template<> bool mcsl::carry::ADD<false>(ulong& lhs, ulong rhs);


//! -, return carry flag
template<> bool mcsl::carry::SUB<false>( sint lhs,  sint rhs,  sint *res);
template<> bool mcsl::carry::SUB<false>(slong lhs, slong rhs, slong *res);
template<> bool mcsl::carry::SUB<false>( uint lhs,  uint rhs,  uint *res);
template<> bool mcsl::carry::SUB<false>(ulong lhs, ulong rhs, ulong *res);
//! -=, return carry flag
template<> bool mcsl::carry::SUB<false>( sint& lhs,  sint rhs);
template<> bool mcsl::carry::SUB<false>(slong& lhs, slong rhs);
template<> bool mcsl::carry::SUB<false>( uint& lhs,  uint rhs);
template<> bool mcsl::carry::SUB<false>(ulong& lhs, ulong rhs);


//! *, return carry flag
template<> sint  mcsl::carry::MUL<false>( sint lhs,  sint rhs,  sint *res);
template<> slong mcsl::carry::MUL<false>(slong lhs, slong rhs, slong *res);
template<> uint  mcsl::carry::MUL<false>( uint lhs,  uint rhs,  uint *res);
template<> ulong mcsl::carry::MUL<false>(ulong lhs, ulong rhs, ulong *res);
//! *=, return carry flag
template<> sint  mcsl::carry::MUL<false>( sint& lhs,  sint rhs);
template<> slong mcsl::carry::MUL<false>(slong& lhs, slong rhs);
template<> uint  mcsl::carry::MUL<false>( uint& lhs,  uint rhs);
template<> ulong mcsl::carry::MUL<false>(ulong& lhs, ulong rhs);

template<> bool mcsl::carry::ADD<true>( sint lhs,  sint rhs,  sint *res);
template<> bool mcsl::carry::ADD<true>(slong lhs, slong rhs, slong *res);
template<> bool mcsl::carry::ADD<true>( uint lhs,  uint rhs,  uint *res);
template<> bool mcsl::carry::ADD<true>(ulong lhs, ulong rhs, ulong *res);
//! +=, return carry flag
template<> bool mcsl::carry::ADD<true>( sint& lhs,  sint rhs);
template<> bool mcsl::carry::ADD<true>(slong& lhs, slong rhs);
template<> bool mcsl::carry::ADD<true>( uint& lhs,  uint rhs);
template<> bool mcsl::carry::ADD<true>(ulong& lhs, ulong rhs);


//! -, return carry flag
template<> bool mcsl::carry::SUB<true>( sint lhs,  sint rhs,  sint *res);
template<> bool mcsl::carry::SUB<true>(slong lhs, slong rhs, slong *res);
template<> bool mcsl::carry::SUB<true>( uint lhs,  uint rhs,  uint *res);
template<> bool mcsl::carry::SUB<true>(ulong lhs, ulong rhs, ulong *res);
//! -=, return carry flag
template<> bool mcsl::carry::SUB<true>( sint& lhs,  sint rhs);
template<> bool mcsl::carry::SUB<true>(slong& lhs, slong rhs);
template<> bool mcsl::carry::SUB<true>( uint& lhs,  uint rhs);
template<> bool mcsl::carry::SUB<true>(ulong& lhs, ulong rhs);


//! *, return carry flag
template<> sint  mcsl::carry::MUL<true>( sint lhs,  sint rhs,  sint *res);
template<> slong mcsl::carry::MUL<true>(slong lhs, slong rhs, slong *res);
template<> uint  mcsl::carry::MUL<true>( uint lhs,  uint rhs,  uint *res);
template<> ulong mcsl::carry::MUL<true>(ulong lhs, ulong rhs, ulong *res);
//! *=, return carry flag
template<> sint  mcsl::carry::MUL<true>( sint& lhs,  sint rhs);
template<> slong mcsl::carry::MUL<true>(slong& lhs, slong rhs);
template<> uint  mcsl::carry::MUL<true>( uint& lhs,  uint rhs);
template<> ulong mcsl::carry::MUL<true>(ulong& lhs, ulong rhs);

#endif //MCSL_CARRY_HPP