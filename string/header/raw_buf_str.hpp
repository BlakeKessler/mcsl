#pragma once
#ifndef MCSL_RAW_BUF_STR_HPP
#define MCSL_RAW_BUF_STR_HPP

#include "MCSL.hpp"
#include "str_base.hpp"
#include "raw_str.hpp"

//!owning non-null-terminated string
//!basically only good for data packing
template<uint _capacity, typename size_t>
class [[clang::trivial_abi]] mcsl::raw_buf_str : public str_base<char> {
   private:
      char _buf[_capacity];
      size_t _size;

      static constexpr const raw_str _nameof = "raw_buf_str";
   public:
      //constructors
      constexpr raw_buf_str():_buf{},_size{} {}
      constexpr raw_buf_str(const char* str, const size_t strlen);
      constexpr raw_buf_str(const char* str);
      constexpr raw_buf_str(const str_t auto& other): raw_buf_str(other.data(), other.size()) {}

      //properties
      [[gnu::pure]] constexpr size_t size() const { return _size; }
      [[gnu::pure]] constexpr size_t capacity() const { return _capacity; }
      [[gnu::pure]] constexpr static const auto& nameof() { return _nameof; }

      //member access
      [[gnu::pure]] constexpr char* const* ptr_to_buf() { return &_buf; }
      [[gnu::pure]] constexpr char* data() { return _buf; }
      [[gnu::pure]] constexpr char* begin() { return _buf; }
      [[gnu::pure]] constexpr const char* const* ptr_to_buf() const { return &_buf; }
      [[gnu::pure]] constexpr const char* data() const { return _buf; }
      [[gnu::pure]] constexpr const char* begin() const { return _buf; }
};

template<uint _capacity, typename size_t>
constexpr mcsl::raw_buf_str<_capacity,size_t>::raw_buf_str(const char* str, const size_t strsize): _buf{},_size(strsize) {
   assert(strsize <= _capacity);
   for (uint i = 0; i < strsize; ++i) {
      if (!str[i]) { _size = i; break; }
      _buf[i] = str[i];
   }
}
template<uint _capacity, typename size_t>
constexpr mcsl::raw_buf_str<_capacity,size_t>::raw_buf_str(const char* str): _buf{}, _size(_capacity) {
   for (uint i = 0; i < _capacity; ++i) {
      if (!str[i]) { _size = i; break; }
      _buf[i] = str[i];
   }
}

#endif //MCSL_RAW_BUF_STR_HPP