#pragma once
#ifndef MCSL_STR_BASE_HPP
#define MCSL_STR_BASE_HPP

#include "MCSL.hpp"
#include "contig_base.hpp"
#include "char_type.hpp"
#include "dyn_arr.hpp"
#include "math.hpp"
#include "mem.hpp"

//!IMPLEMENTATION GUIDE:
//!   consider null-termination
//!      if possible, owning string classes should maintain null-termination
//!      if the implementation doesn't ensure null-termination, DOCUMENT THAT   
//!   implement contig_base
//!   implement name
//!   write constructors
template<typename char_t>
struct mcsl::str_base : public contig_base<char_t> {
   // static constexpr const raw_str _nameof = "str_base";
   static constexpr const char _nameof[] = "str_base";
   constexpr static const auto& nameof() { return _nameof; }

   constexpr const str_slice slice(this const auto&& obj);
   constexpr const str_slice slice(this const auto&& obj, uint size);
   constexpr const str_slice slice(this const auto&& obj, uint begin, uint end);
   constexpr str_slice slice(this auto&& obj);
   constexpr str_slice slice(this auto&& obj, uint begin);
   constexpr str_slice slice(this auto&& obj, uint begin, uint end);
   
   //strlen
   constexpr uint strlen(this auto&& obj) {
      for (uint i = 0; i < obj.size(); ++i) {
         if (!obj[i]) {
            return i;
         }
      }
      return obj.size();
   }

   //repeat declaration of pointer arithmatic operator because C++ templates are weird
   [[gnu::pure]] inline constexpr auto operator+(this auto&& obj, const uint i) -> decltype(auto)   { assume(i < obj.size()); return obj.begin() + i; }
   
   //operations
   template<str_t strT> strT copy(this const auto& obj);

   // str_t& operator+=(const str_base& other);
   template<str_t strT> auto operator+(this const auto& obj, const strT& other);
   
   // str_t& operator*=(const uint i);
   template<str_t strT> strT operator*(this auto&& obj, const uint i);
   
   template<str_t strT> strT&& alter(this auto&& obj, char (*const transformer)(const char));
   template<str_t strT> strT altered(this const auto& obj, char (*const transformer)(const char));
   
   inline auto&& to_upper(this auto&& obj) { return obj.alter(mcsl::to_upper); }
   inline auto&& to_lower(this auto&& obj) { return obj.alter(mcsl::to_lower); }
   inline auto as_upper(this const auto& obj) { return obj.altered(mcsl::to_upper); }
   inline auto as_lower(this const auto& obj) { return obj.altered(mcsl::to_lower); }

   uint operator&(this const auto& obj, const auto& other); //strspn
   uint operator^(this const auto& obj, const auto& other); //strcspn
   uint operator/(this const auto& obj, const auto& other); //largest n : other*n ⊇ self

   template<str_t strT> dyn_arr<strT> tokenize(this const auto& obj, const str_base& delimChars);
   template<str_t strT> dyn_arr<strT> tokenize(this const auto& obj, const container_base<str_base>& delimStrs);

   //comparison
   template<str_t strT> inline constexpr bool operator==(this const auto& s, const strT& other) { if (s.size() != other.size()) { return false; } return !(s <=> other); }
   template<str_t strT> inline constexpr bool operator!=(this const auto& s, const strT& other) { if (s.size() != other.size()) { return true;  } return   s <=> other;  }

   template<str_t strT> inline constexpr sint operator<=>(this const auto& s, const strT& other) { return s.strcmp(other); }


   template<uint len> inline constexpr bool operator==(this const auto& s, const char_t other[len+1]) { return s == str_slice(other); }
   template<uint len> inline constexpr bool operator!=(this const auto& s, const char_t other[len+1]) { return s != str_slice(other); }

   template<uint len> inline constexpr sint operator<=>(this const auto& s, const char_t other[len+1]) { return s <=> str_slice(other); }


   inline constexpr bool operator==(this const auto& s, const char c) { return s.size() == 1 && s[0] == c; }
   inline constexpr bool operator!=(this const auto& s, const char c) { return s.size() != 1 || s[0] != c; }

   inline constexpr sint operator<=>(this const auto& s, const char c) { if (s.size() == 1) { return s[0] - c; } else { return (s[0] != c) ? (s[0] - c) : s[1]; } }

   //!TODO: make sure that cstr types use strlen when `size` is called
   //!TODO: make sure that non-cstr types don't act like cstr types
   template<str_t strT> constexpr sint strcmp(this const auto& lhs, const strT& rhs) {
      const uint lhsLen = lhs.size();
      const uint rhsLen = rhs.size();
      const uint minLen = mcsl::min(lhsLen, rhsLen);

      const sint dif = mcsl::memcmp(lhs.begin(), rhs.begin(), minLen);
      if (dif) { return dif; }
      else if (lhsLen > rhsLen) { return  lhs[minLen]; }
      else if (lhsLen < rhsLen) { return -rhs[minLen]; }
      else { return 0; }
   }
   template<str_t strT> constexpr sint substrcmp(this const auto& s, const strT& other) {
      const uint len = mcsl::min(s.size(), other.size());
      return mcsl::memcmp(s.begin(), other.begin(), len);
   }
};




//hashing
template<mcsl::str_t str_t> struct std::hash<str_t> {
   using is_transparent = void;

   uword __impl(const char* str, const uint size) const noexcept { //FNV HASH
      uword hash = 0;
      for (uint i = size; i;) {
         --i;
         hash = (hash << 8) + (uint8)(str[i]);
         // hash = (hash << 8) + (byte)(str[i]);
      }
      return hash;
   }

   template<mcsl::str_t other_t> inline uword operator()(const other_t& str) const noexcept { return __impl(str.begin(), str.size()); }
   inline uword operator()(const std::string_view str) const noexcept { return __impl(str.begin(), str.size()); }
   inline uword operator()(const std::string& str) const noexcept { return __impl(str.begin(), str.size()); }
};
//equality checking
template<mcsl::str_t str_t> struct std::equal_to<str_t> {
   using is_transparent = void;

   template<mcsl::str_t other_t> inline bool operator()(const str_t& lhs, const other_t& rhs) const noexcept { return lhs == rhs; }
};

#endif //MCSL_STR_BASE_HPP