#pragma once
#ifndef MCSL_RAW_CSTR_HPP
#define MCSL_RAW_CSTR_HPP

#include "MCSL.hpp"
#include "str_base.hpp"
#include "assert.hpp"
#include "mem.hpp"

//!null-terminated string literal
template<uint _size> class [[clang::trivial_abi]] mcsl::raw_cstr : public str_base<char> {
   private:
      char _buf[_size + 1];

      // static constexpr const raw_cstr<7> _nameof = "raw_cstr";
      static constexpr char _nameof[] = "raw_cstr";
   public:
      //constructors
      // constexpr raw_cstr(const raw_cstr& other):_buf{other._buf} {}
      constexpr raw_cstr():_buf{} {}
      constexpr raw_cstr(const str_t auto& other): raw_cstr(other.data(),other.size()) {}
      constexpr raw_cstr(const char* str, const uint strlen);
      /*unsafe*/ constexpr raw_cstr(const char* str);

      //properties
      [[gnu::pure]] constexpr uint size() const { return cstrlen(_buf, _size); }
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

template<uint _size> constexpr mcsl::raw_cstr<_size>::raw_cstr(const char* str, const uint strsize): _buf{} {
   assert(strsize <= _size, __OVERSIZED_COPY_MSG, ErrCode::SEGFAULT);
   cstrcpy(_buf, str, strsize);
}
template<uint _size> constexpr mcsl::raw_cstr<_size>::raw_cstr(const char* str): _buf{} {
   cstrcpy(_buf, str, strsize);
}

#pragma region CTAD
namespace mcsl {
   template<uint len> requires (len > 0) raw_cstr(const char (&str)[len]) -> raw_cstr<len-1>;
}
#pragma endregion CTAD

#endif //MCSL_RAW_CSTR_HPP