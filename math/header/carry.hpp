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




      // //! +, return carry flag
      // [[gnu::always_inline, gnu::pure]] inline bool ADD ( sint lhs,  sint rhs,  sint *res) { return __builtin_sadd_overflow(lhs, rhs, res); }
      // [[gnu::always_inline, gnu::pure]] inline bool ADD (slong lhs, slong rhs, slong *res) { return __builtin_saddl_overflow(lhs, rhs, res); }
      // [[gnu::always_inline, gnu::pure]] inline bool ADD ( uint lhs,  uint rhs,  uint *res) { return __builtin_uadd_overflow(lhs, rhs, res); }
      // [[gnu::always_inline, gnu::pure]] inline bool ADD (ulong lhs, ulong rhs, ulong *res) { return __builtin_uaddl_overflow(lhs, rhs, res); }
      // //! +=, return carry flag
      // [[gnu::always_inline, gnu::pure]] inline bool ADD ( sint& lhs,  sint rhs) { return __builtin_sadd_overflow(lhs, rhs, &lhs); }
      // [[gnu::always_inline, gnu::pure]] inline bool ADD (slong& lhs, slong rhs) { return __builtin_saddl_overflow(lhs, rhs, &lhs); }
      // [[gnu::always_inline, gnu::pure]] inline bool ADD ( uint& lhs,  uint rhs) { return __builtin_uadd_overflow(lhs, rhs, &lhs); }
      // [[gnu::always_inline, gnu::pure]] inline bool ADD (ulong& lhs, ulong rhs) { return __builtin_uaddl_overflow(lhs, rhs, &lhs); }


      // //! -, return carry flag
      // [[gnu::always_inline, gnu::pure]] inline bool SUB ( sint lhs,  sint rhs,  sint *res) { return __builtin_ssub_overflow(lhs, rhs, res); }
      // [[gnu::always_inline, gnu::pure]] inline bool SUB (slong lhs, slong rhs, slong *res) { return __builtin_ssubl_overflow(lhs, rhs, res); }
      // [[gnu::always_inline, gnu::pure]] inline bool SUB ( uint lhs,  uint rhs,  uint *res) { return __builtin_usub_overflow(lhs, rhs, res); }
      // [[gnu::always_inline, gnu::pure]] inline bool SUB (ulong lhs, ulong rhs, ulong *res) { return __builtin_usubl_overflow(lhs, rhs, res); }
      // //! -=, return carry flag
      // [[gnu::always_inline, gnu::pure]] inline bool SUB ( sint& lhs,  sint rhs) { return __builtin_ssub_overflow(lhs, rhs, &lhs); }
      // [[gnu::always_inline, gnu::pure]] inline bool SUB (slong& lhs, slong rhs) { return __builtin_ssubl_overflow(lhs, rhs, &lhs); }
      // [[gnu::always_inline, gnu::pure]] inline bool SUB ( uint& lhs,  uint rhs) { return __builtin_usub_overflow(lhs, rhs, &lhs); }
      // [[gnu::always_inline, gnu::pure]] inline bool SUB (ulong& lhs, ulong rhs) { return __builtin_usubl_overflow(lhs, rhs, &lhs); }


      // //! *, return carry flag
      // [[gnu::always_inline, gnu::pure]] inline bool MUL ( sint lhs,  sint rhs,  sint *res) { return __builtin_smul_overflow(lhs, rhs, res); }
      // [[gnu::always_inline, gnu::pure]] inline bool MUL (slong lhs, slong rhs, slong *res) { return __builtin_smull_overflow(lhs, rhs, res); }
      // [[gnu::always_inline, gnu::pure]] inline bool MUL ( uint lhs,  uint rhs,  uint *res) { return __builtin_umul_overflow(lhs, rhs, res); }
      // [[gnu::always_inline, gnu::pure]] inline bool MUL (ulong lhs, ulong rhs, ulong *res) { return __builtin_umull_overflow(lhs, rhs, res); }
      // //! *=, return carry flag
      // [[gnu::always_inline, gnu::pure]] inline bool MUL ( sint& lhs,  sint rhs) { return __builtin_smul_overflow(lhs, rhs, &lhs); }
      // [[gnu::always_inline, gnu::pure]] inline bool MUL (slong& lhs, slong rhs) { return __builtin_smull_overflow(lhs, rhs, &lhs); }
      // [[gnu::always_inline, gnu::pure]] inline bool MUL ( uint& lhs,  uint rhs) { return __builtin_umul_overflow(lhs, rhs, &lhs); }
      // [[gnu::always_inline, gnu::pure]] inline bool MUL (ulong& lhs, ulong rhs) { return __builtin_umull_overflow(lhs, rhs, &lhs); }
   };
}

#include "../src/carry.cpp"

#endif //MCSL_CARRY_HPP