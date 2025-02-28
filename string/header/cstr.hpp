#pragma once
#ifndef MCSL_CSTR_HPP
#define MCSL_CSTR_HPP

#include "MCSL.hpp"
#include "str_base.hpp"
#include "dyn_arr.hpp"
#include "raw_str.hpp"

//!null-terminated dynamically-sized cstr
//!NOTE: tiny inefficienies caused by using a dyn_arr instead of re-implementing it with minor modifications
class mcsl::cstr : public str_base<char> {
   private:
      dyn_arr<char> _buf;

      static constexpr const raw_str _nameof = "cstr";
   public:
      //constructors
      constexpr cstr():_buf() {}
      cstr(const uint size);
      cstr(const char* str, const uint strlen);
      cstr(const char* str);
      cstr(cstr&& other);
      cstr(const cstr& other);

      template<str_t strT> requires requires(strT s) { s.release(); }
         cstr(strT&& other): cstr(other.data(),other.size()) { if (this != &other) { other.release(); } }
      template<str_t strT> cstr(const strT& other): cstr(other.data(),other.size()) {}

      //properties
      inline constexpr uint size() const { return _buf.size() - 1; }
      inline constexpr uint capacity() const { return _buf.capacity() - 1; }
      constexpr static const auto& nameof() { return _nameof; }

      inline constexpr char* const* ptr_to_buf() { return _buf.ptr_to_buf(); }
      inline constexpr char* data() { return _buf.data(); }
      constexpr char* begin() { return _buf.begin(); }
      inline constexpr const char* const* ptr_to_buf() const { return _buf.ptr_to_buf(); }
      inline constexpr const char* data() const { return _buf.data(); }
      constexpr const char* begin() const { return _buf.begin(); }
      
      //MODIFIERS
      //!realloc buffer to at least the specified size
      inline bool reserve(const uint newSize) { return _buf.reserve(newSize + 1); }
      inline bool reserve_exact(const uint newSize) { return _buf.reserve_exact(newSize + 1); }
      char* release() { return _buf.release(); }
      char* push_back(const char ch) { _buf.back() = ch; return _buf.push_back('\0'); }
      char pop_back() { const char tmp = back(); _buf.pop_back(); _buf.back() = '\0'; return tmp; }


      cstr& operator+=(const str_t auto& other);
      cstr& operator*=(const uint repeatCount);

      operator char*() { return _buf.begin(); }
      operator const char*() const { return _buf.begin(); }
};

#endif //MCSL_CSTR_HPP