#ifndef MCSL_CARRY_CPP
#define MCSL_CARRY_CPP

#include "carry.hpp"
#include "type_traits.hpp"
#include "mcsl_throw.hpp"

#pragma region add
template<bool isError> [[gnu::always_inline, gnu::pure]] inline bool mcsl::carry::ADD(sint lhs, sint rhs, sint* res) {
   if constexpr (isError) {
      sint tmp;
      if (__builtin_sadd_overflow(lhs, rhs, &tmp)) {
         mcsl_throw(ErrCode::INT_OVERFLOW, "integer overflow (%d + %d > %d)", lhs, rhs, TYPEMAX<sint>());
         return true;
      }
      *res = tmp;
      return false;
   } else {
      return __builtin_sadd_overflow(lhs, rhs, res);
   }
}

template<bool isError> [[gnu::always_inline, gnu::pure]] inline bool mcsl::carry::ADD(slong lhs, slong rhs, slong* res) {
   if constexpr (isError) {
      slong tmp;
      if (__builtin_saddl_overflow(lhs, rhs, &tmp)) {
         mcsl_throw(ErrCode::INT_OVERFLOW, "long integer overflow (%ld + %ld > %ld)", lhs, rhs, TYPEMAX<slong>());
         return true;
      }
      *res = tmp;
      return false;
   } else {
      return __builtin_saddl_overflow(lhs, rhs, res);
   }
}

template<bool isError> [[gnu::always_inline, gnu::pure]] inline bool mcsl::carry::ADD(uint lhs, uint rhs, uint* res) {
   if constexpr (isError) {
      uint tmp;
      if (__builtin_uadd_overflow(lhs, rhs, &tmp)) {
         mcsl_throw(ErrCode::INT_OVERFLOW, "unsigned integer overflow (%u + %u > %u)", lhs, rhs, TYPEMAX<uint>());
         return true;
      }
      *res = tmp;
      return false;
   } else {
      return __builtin_uadd_overflow(lhs, rhs, res);
   }
}

template<bool isError> [[gnu::always_inline, gnu::pure]] inline bool mcsl::carry::ADD(ulong lhs, ulong rhs, ulong* res) {
   if constexpr (isError) {
      ulong tmp;
      if (__builtin_uaddl_overflow(lhs, rhs, &tmp)) {
         mcsl_throw(ErrCode::INT_OVERFLOW, "unsigned long integer overflow (%lu + %lu > %lu)", lhs, rhs, TYPEMAX<ulong>());
         return true;
      }
      *res = tmp;
      return false;
   } else {
      return __builtin_uaddl_overflow(lhs, rhs, res);
   }
}
#pragma endregion add

#pragma region sub
template<bool isError> [[gnu::always_inline, gnu::pure]] inline bool mcsl::carry::SUB(sint lhs, sint rhs, sint* res) {
   if constexpr (isError) {
      sint tmp;
      if (__builtin_ssub_overflow(lhs, rhs, &tmp)) {
         mcsl_throw(ErrCode::INT_OVERFLOW, "integer overflow (%d - %d)", lhs, rhs);
         return true;
      }
      *res = tmp;
      return false;
   } else {
      return __builtin_ssub_overflow(lhs, rhs, res);
   }
}

template<bool isError> [[gnu::always_inline, gnu::pure]] inline bool mcsl::carry::SUB(slong lhs, slong rhs, slong* res) {
   if constexpr (isError) {
      slong tmp;
      if (__builtin_ssubl_overflow(lhs, rhs, &tmp)) {
         mcsl_throw(ErrCode::INT_OVERFLOW, "long integer overflow (%ld - %ld)", lhs, rhs);
         return true;
      }
      *res = tmp;
      return false;
   } else {
      return __builtin_ssubl_overflow(lhs, rhs, res);
   }
}

template<bool isError> [[gnu::always_inline, gnu::pure]] inline bool mcsl::carry::SUB(uint lhs, uint rhs, uint* res) {
   if constexpr (isError) {
      uint tmp;
      if (__builtin_usub_overflow(lhs, rhs, &tmp)) {
         mcsl_throw(ErrCode::INT_OVERFLOW, "unsigned integer overflow (%u - %u)", lhs, rhs);
         return true;
      }
      *res = tmp;
      return false;
   } else {
      return __builtin_usub_overflow(lhs, rhs, res);
   }
}

template<bool isError> [[gnu::always_inline, gnu::pure]] inline bool mcsl::carry::SUB(ulong lhs, ulong rhs, ulong* res) {
   if constexpr (isError) {
      ulong tmp;
      if (__builtin_usubl_overflow(lhs, rhs, &tmp)) {
         mcsl_throw(ErrCode::INT_OVERFLOW, "unsigned long integer overflow (%lu - %lu)", lhs, rhs);
         return true;
      }
      *res = tmp;
      return false;
   } else {
      return __builtin_usubl_overflow(lhs, rhs, res);
   }
}
#pragma endregion sub

#pragma region mul //!NOTE: rewrite in assembly if possible
template<bool isError> [[gnu::always_inline, gnu::pure]] inline sint mcsl::carry::MUL(sint lhs, sint rhs, sint* res) {
   if constexpr (isError) {
      sint tmp;
      if (__builtin_smul_overflow(lhs, rhs, &tmp)) {
         mcsl_throw(ErrCode::INT_OVERFLOW, "integer overflow (%d * %d)", lhs, rhs);
         return true;
      }
      *res = tmp;
      return 0;
   } else {
      slong tmp = (slong)lhs * (slong)rhs;
      *res = (sint)tmp;
      return (sint)((ulong)(tmp) >> (sizeof(sint) * 8)); //return high bytes
   }
}

template<bool isError> [[gnu::always_inline, gnu::pure]] inline slong mcsl::carry::MUL(slong lhs, slong rhs, slong* res) {
   if constexpr (isError) {
      slong tmp;
      if (__builtin_smull_overflow(lhs, rhs, &tmp)) {
         mcsl_throw(ErrCode::INT_OVERFLOW, "long integer overflow (%ld * %ld)", lhs, rhs);
         return true;
      }
      *res = tmp;
      return 0;
   } else {
      soverlong tmp = (soverlong)lhs * (soverlong)rhs;
      *res = (slong)tmp;
      return (slong)((uoverlong)(tmp) >> (sizeof(slong) * 8)); //return high bytes
   }
}

template<bool isError> [[gnu::always_inline, gnu::pure]] inline uint mcsl::carry::MUL(uint lhs, uint rhs, uint* res) {
   if constexpr (isError) {
      uint tmp;
      if (__builtin_umul_overflow(lhs, rhs, &tmp)) {
         mcsl_throw(ErrCode::INT_OVERFLOW, "unsigned integer overflow (%u * %u)", lhs, rhs);
         return true;
      }
      *res = tmp;
      return 0;
   } else {
      ulong tmp = (ulong)lhs * (ulong)rhs;
      *res = (uint)tmp;
      return (uint)(tmp >> (sizeof(uint) * 8)); //return high bytes
   }
}

template<bool isError> [[gnu::always_inline, gnu::pure]] inline ulong mcsl::carry::MUL(ulong lhs, ulong rhs, ulong* res) {
   if constexpr (isError) {
      ulong tmp;
      if (__builtin_umull_overflow(lhs, rhs, &tmp)) {
         mcsl_throw(ErrCode::INT_OVERFLOW, "unsigned long integer overflow (%lu * %lu)", lhs, rhs);
         return true;
      }
      *res = tmp;
      return 0;
   } else {
      uoverlong tmp = (uoverlong)lhs * (uoverlong)rhs;
      *res = (ulong)tmp;
      return (ulong)(tmp >> (sizeof(ulong) * 8)); //return high bytes
   }
}
#pragma endregion mul

#endif //MCSL_CARRY_CPP