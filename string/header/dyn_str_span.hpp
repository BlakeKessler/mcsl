#pragma once
#ifndef MCSL_DYN_STRING_SPAN_HPP
#define MCSL_DYN_STRING_SPAN_HPP

#include "MCSL.hpp"
#include "str_base.hpp"
#include "raw_str.hpp"

//!non-owning potentially-non-null-terminated string
//!not invalidated if the string is reallocated
//!   the object itself - pointers to characters are still invalidated
//!relatively unsafe (even ignoring the invalidation)
//!   nothing stops setting/incrementing _begin or _size beyond the end of the spanned string
class [[clang::trivial_abi]] mcsl::dyn_str_span : public str_base<char> {
   private:
      char* const* _ptrToBuf;
      uint _begin;
      uint _size;

      static constexpr const raw_str _nameof = "dyn_str_span";
   public:
      //constructors
      constexpr dyn_str_span():_ptrToBuf(),_begin(),_size() {}
      constexpr dyn_str_span(char* const* str, const uint strlen);
      constexpr dyn_str_span(str_t auto& other): _ptrToBuf(other.ptr_to_buf()),_begin(0),_size(other.size()) {}
      constexpr dyn_str_span(str_t auto& other, const uint size): _ptrToBuf(other.ptr_to_buf()),_begin(0),_size(size) { assert(other.size() >= size); }
      constexpr dyn_str_span(str_t auto& other, const uint begin, const uint size): _ptrToBuf(other.ptr_to_buf()),_begin(begin),_size(size) { assert(other.size() >= begin + size); }

      //properties
      [[gnu::pure]] constexpr uint size() const { return _size; }
      [[gnu::pure]] constexpr static const auto& nameof() { return _nameof; }
      
      constexpr dyn_str_span& set_begin(const uint i) { _size += _begin; _size -= i; _begin = i; return self; }
      constexpr dyn_str_span& inc_begin(const sint i) { _size -= i; _begin += i; return self; }
      constexpr dyn_str_span& set_size(const uint i) { _size = i; return self; }
      constexpr dyn_str_span& inc_end(const sint i) { _size += i; return self; }

      //member access
      [[gnu::pure]] constexpr char* const* ptr_to_buf() { return _ptrToBuf; }
      [[gnu::pure]] constexpr char* data() { return *_ptrToBuf; }
      [[gnu::pure]] constexpr char* begin() { return *_ptrToBuf + _begin; }
      [[gnu::pure]] constexpr const char* const* ptr_to_buf() const { return _ptrToBuf; }
      [[gnu::pure]] constexpr const char* data() const { return *_ptrToBuf; }
      [[gnu::pure]] constexpr const char* begin() const { return *_ptrToBuf + _begin; }
};

#endif //MCSL_DYN_STRING_SPAN_HPP