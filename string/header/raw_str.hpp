#pragma once
#ifndef MCSL_RAW_STR_HPP
#define MCSL_RAW_STR_HPP

#include "MCSL.hpp"
#include "str_base.hpp"
#include "assert.hpp"

//!null-terminated string literal
template<uint _size> class [[clang::trivial_abi]] mcsl::raw_str : public str_base<char> {
   private:
      char _buf[_size + 1];

      // static constexpr const raw_str<7> _nameof = "raw_str";
      static constexpr char _nameof[] = "raw_str";
   public:
      //constructors
      // constexpr raw_str(const raw_str& other):_buf{other._buf} {}
      constexpr raw_str():_buf{} {}
      template<str_t strT> constexpr raw_str(const strT& other): raw_str(other.data(),other.size()) {}
      constexpr raw_str(const char* str, const uint strlen);
      constexpr raw_str(const char* str);

      //properties
      [[gnu::pure]] constexpr uint size() const { return sizeof(_buf) - 1; }
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
   assert(strsize <= _size);
   for (uint i = 0; i < strsize; ++i) {
      if (!str[i]) { break; }
      _buf[i] = str[i];
   }
}
template<uint _size> constexpr mcsl::raw_str<_size>::raw_str(const char* str): _buf{} {
   for (uint i = 0; i < _size; ++i) {
      if (!str[i]) { break; }
      _buf[i] = str[i];
   }
}

#pragma region CTAD
namespace mcsl {
   template<uint len> requires (len > 0) raw_str(const char (&str)[len]) -> raw_str<len-1>;
}
#pragma endregion CTAD

#endif //MCSL_RAW_STR_HPP