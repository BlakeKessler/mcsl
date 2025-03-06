#pragma once
#ifndef MCSL_RAW_STR_HPP
#define MCSL_RAW_STR_HPP

#include "MCSL.hpp"
#include "str_base.hpp"
#include "assert.hpp"

//!null-terminated string literal
template<uint _size> class [[clang::trivial_abi]] mcsl::raw_str : public str_base<char> {
   private:
      char _buf[_size];

      // static constexpr const raw_str<7> _nameof = "raw_str";
      static constexpr char _nameof[] = "raw_str";
   public:
      //constructors
      // constexpr raw_str(const raw_str& other):_buf{other._buf} {}
      constexpr raw_str():_buf{} {}
      constexpr raw_str(const raw_str& other):_buf{other._buf} {}
      constexpr raw_str(const str_t auto& other): raw_str(other.data(),other.size()) {}
      constexpr raw_str(const char* str, const uint strlen);
      constexpr raw_str(const char (&str)[_size+1]): raw_str(str, _size) {}

      //properties
      [[gnu::pure]] constexpr uint size() const { return _size; }
      [[gnu::pure]] constexpr static const auto& nameof() { return _nameof; }
      // constexpr uint size() const { return _size; }
      // constexpr auto& nameof() const { return _nameof; }

      //member access
      [[gnu::pure]] constexpr char* const* ptr_to_buf() { return reinterpret_cast<char**>(&_buf); }
      [[gnu::pure]] constexpr char* data() { return _buf; }
      [[gnu::pure]] constexpr char* begin() { return _buf; }
      [[gnu::pure]] constexpr const char* const* ptr_to_buf() const { return reinterpret_cast<char**>(&_buf); }
      [[gnu::pure]] constexpr const char* data() const { return _buf; }
      [[gnu::pure]] constexpr const char* begin() const { return _buf; }
};

template<uint _size> constexpr mcsl::raw_str<_size>::raw_str(const char* str, const uint strsize): _buf{} {
   assert(strsize <= _size, __OVERSIZED_COPY_MSG, ErrCode::SEGFAULT);
   memcpy(_buf, str, strsize);
}

#pragma region CTAD
namespace mcsl {
   template<uint len> requires (len > 0) raw_str(const char (&str)[len]) -> raw_str<len-1>;
}
#pragma endregion CTAD

#endif //MCSL_RAW_STR_HPP