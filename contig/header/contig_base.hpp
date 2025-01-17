#pragma once
#ifndef MCSL_CONTIG_BASE_HPP
#define MCSL_CONTIG_BASE_HPP

#include "MCSL.hpp"
#include "alloc.hpp"

#include <typeinfo>

//!IMPLEMENTATION GUIDE:
//!    create member variables and template params (buffer,size)
//!    write constructors
//!    implement data() and begin()
//!    implement ptr_to_buf()
//!    implement size()
//!    implement nameof()
template<typename T> struct mcsl::contig_base {
   static constexpr const bool is_contig = true;
   static constexpr const char _nameof[] = "contig_base";
   static constexpr const auto& nameof() { return _nameof; }

   //properties
   [[gnu::pure]] constexpr uint size(this auto&& obj) { return obj.size(); }

   //element access
   [[gnu::pure]] constexpr T* const* ptr_to_buf(this auto&& obj)   { return obj.ptr_to_buf(); }
   [[gnu::pure]] constexpr T* data(this auto&& obj)   { return obj.data(); }
   [[gnu::pure]] constexpr T* begin(this auto&& obj)   { return obj.begin(); }

   [[gnu::pure]] inline constexpr auto end(this auto&& obj) -> decltype(auto)   { return obj.begin() + obj.size(); }
   [[gnu::pure]] inline constexpr auto operator[](this auto&& obj, const uint i) -> decltype(auto)   { return obj.begin()[i]; }
   [[gnu::pure]] inline constexpr auto at(this auto&& obj, const uint i) -> decltype(auto)   { if (i >= obj.size()) { mcsl_throw(ErrCode::SEGFAULT, "%s of size %u accessed at index %u", obj.nameof(), obj.size(), i); } return obj[i]; }
   [[gnu::pure]] inline constexpr auto front(this auto&& obj) -> decltype(auto)   { return obj.begin()[0]; }
   [[gnu::pure]] inline constexpr auto back(this auto&& obj) -> decltype(auto)   { return obj.begin()[obj.size()-1]; }

   //slicing
   [[gnu::pure]] inline decltype(auto) slice(this const auto&& obj, const uint stop) { return obj.slice(0, stop, 1); }
   [[gnu::pure]] inline decltype(auto) slice(this const auto&& obj, const uint start, const uint stop) { return obj.slice(start, stop, 1); }
   [[gnu::pure]] decltype(auto) slice(this const auto&& obj, const uint start, const uint stop, const uint step);

   [[gnu::pure]] inline operator bool() const { return static_cast<bool>(size()); }
};

#endif //MCSL_CONTIG_BASE_HPP