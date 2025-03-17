#pragma once
#ifndef MCSL_CONTIG_BASE_HPP
#define MCSL_CONTIG_BASE_HPP

#include "MCSL.hpp"
#include "alloc.hpp"
#include "assert.hpp"

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
   [[gnu::pure]] constexpr operator bool() const { return size(); }

   //element access
   [[gnu::pure]] constexpr T* const* ptr_to_buf(this auto&& obj)   { return obj.ptr_to_buf(); }
   [[gnu::pure]] constexpr T* data(this auto&& obj)   { return obj.data(); }
   [[gnu::pure]] constexpr T* begin(this auto&& obj)   { return obj.begin(); }

   [[gnu::pure]] inline constexpr auto operator+(this auto&& obj, const uint i) -> decltype(auto)   { assume(i < obj.size()); return obj.begin() + i; }
   [[gnu::pure]] inline constexpr auto operator[](this auto&& obj, const uint i) -> decltype(auto)   { return *(obj + i); }
   [[gnu::pure]] inline constexpr auto at(this auto&& obj, const uint i) -> decltype(auto)   { if (i >= obj.size()) { __throw(ErrCode::SEGFAULT, "%s of size %u accessed at index %u", obj.nameof(), obj.size(), i); } if (!obj.data()) { __throw(ErrCode::SEGFAULT, "null %s dereferenced", obj.nameof()); } return obj[i]; }
   [[gnu::pure]] inline constexpr auto end(this auto&& obj) -> decltype(auto)   { return obj.begin() + obj.size(); }
   [[gnu::pure]] inline constexpr auto front(this auto&& obj) -> decltype(auto)   { return obj[0]; }
   [[gnu::pure]] inline constexpr auto back(this auto&& obj) -> decltype(auto)   { return obj[obj.size()-1]; }



   constexpr const arr_span<T> span(this const auto&& obj);
   constexpr const arr_span<T> span(this const auto&& obj, uint size);
   constexpr const arr_span<T> span(this const auto&& obj, uint begin, uint size);
   constexpr arr_span<T> span(this auto&& obj);
   constexpr arr_span<T> span(this auto&& obj, uint begin);
   constexpr arr_span<T> span(this auto&& obj, uint size, uint begin);
};

#endif //MCSL_CONTIG_BASE_HPP