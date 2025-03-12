#pragma once
#ifndef MCSL_RAW_BUF_STR_HPP
#define MCSL_RAW_BUF_STR_HPP

#include "MCSL.hpp"
#include "str_base.hpp"
#include "mem.hpp"

//!owning non-null-terminated string
//!static capacity, dynamic size
template<uint _capacity, typename size_t>
class [[clang::trivial_abi]] mcsl::raw_buf_str : public str_base<char> {
   private:
      char _buf[_capacity];
      size_t _size;

      static constexpr const char _nameof[] = "raw_buf_str";
   public:
      //constructors
      constexpr raw_buf_str():_buf{},_size{} {}
      constexpr raw_buf_str(const char* str, const size_t strlen);
      // constexpr raw_buf_str(const raw_buf_str& other):_buf{other._buf},_size{other._size} {}
      constexpr raw_buf_str(const str_t auto& other): raw_buf_str(other.data(), other.size()) {}
      constexpr raw_buf_str(const char str[_capacity+1]): raw_buf_str(str, _capacity) {}
      // template<uint len> requires (len <= _capacity) constexpr raw_buf_str(const char str[len+1]): raw_buf_str(str, len) {}

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

      constexpr char* push_back(char c);
};

template<uint _capacity, typename size_t>
constexpr mcsl::raw_buf_str<_capacity,size_t>::raw_buf_str(const char* str, const size_t strsize): _buf{},_size(strsize) {
   assert(_size <= _capacity, __OVERSIZED_COPY_MSG, ErrCode::SEGFAULT);
   memcpy(_buf, str, strsize);
}

template<uint _capacity, typename size_t>
constexpr char* mcsl::raw_buf_str<_capacity,size_t>::push_back(char c) {
   assume(_size < _capacity);
   char* ptr = _buf + _size++;
   *ptr = c;
   return ptr;
}

#endif //MCSL_RAW_BUF_STR_HPP