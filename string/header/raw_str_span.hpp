#pragma once
#ifndef MCSL_RAW_STRING_SPAN_HPP
#define MCSL_RAW_STRING_SPAN_HPP

#include "MCSL.hpp"
#include "str_base.hpp"
#include "raw_str.hpp"

//!non-owning potentially-non-null-terminated string
//!invalidated if the string is reallocated
//!relatively unsafe (even ignoring the invalidation)
//!   nothing stops setting/incrementing _buf or _size beyond the end of the spanned string
class [[clang::trivial_abi]] mcsl::raw_str_span : public str_base<char> {
   private:
      char* _buf;
      uint _size;

      static constexpr const raw_str _nameof = "raw_str_span";
   public:
      //constructors
      constexpr raw_str_span(): _buf(),_size() {}
      constexpr raw_str_span(char* str, const uint size):_buf(str),_size(size) {}
      constexpr raw_str_span(char* begin, char* end):_buf(begin),_size(end-begin) {}
      template<mcsl::str_t Other_t> constexpr raw_str_span(Other_t& other): raw_str_span(other.begin(),other.size()) {}
      template<mcsl::str_t Other_t> constexpr raw_str_span(Other_t& other, const uint size): raw_str_span(other.begin(),size) { assert(other.size() >= size); }
      template<mcsl::str_t Other_t> constexpr raw_str_span(Other_t& other, const uint begin, const uint size): raw_str_span(other.begin() + begin, size) { assert(other.size() >= begin + size); }

      //properties
      [[gnu::pure]] constexpr uint size() const { return _size; }
      [[gnu::pure]] constexpr static const auto& nameof() { return _nameof; }
      
      constexpr raw_str_span& inc_begin(const sint i) { _size -= i; _buf += i; return self; }
      constexpr raw_str_span& set_size(const uint i) { _size = i; return self; }
      constexpr raw_str_span& inc_end(const sint i) { _size += i; return self; }

      //member access
      [[gnu::pure]] constexpr char* const* ptr_to_buf() { return &_buf; }
      [[gnu::pure]] constexpr char* data() { return _buf; }
      [[gnu::pure]] constexpr char* begin() { return _buf; }
      [[gnu::pure]] constexpr const char* const* ptr_to_buf() const { return &_buf; }
      [[gnu::pure]] constexpr const char* data() const { return _buf; }
      [[gnu::pure]] constexpr const char* begin() const { return _buf; }
};

#endif //MCSL_RAW_STRING_SPAN_HPP