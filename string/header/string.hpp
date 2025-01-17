#pragma once
#ifndef MCSL_STRING_HPP
#define MCSL_STRING_HPP

#include "MCSL.hpp"
#include "str_base.hpp"
#include "dyn_arr.hpp"
#include "raw_str.hpp"

//!null-terminated dynamically-sized string
//!NOTE: tiny inefficienies caused by using a dyn_arr instead of re-implementing it with minor modifications
class mcsl::string : public str_base<char> {
   private:
      dyn_arr<char> _buf;

      static constexpr const raw_str _nameof = "string";
   public:
      //constructors
      constexpr string():_buf() {}
      string(const uint size);
      string(const char* str, const uint strlen);
      string(const char* str);
      string(string&& other);
      string(const string& other);

      template<str_t strT> requires requires(strT s) { s.release(); }
         string(strT&& other): string(other.data(),other.size()) { if (this != &other) { other.release(); } }
      template<str_t strT> string(const strT& other): string(other.data(),other.size()) {}

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
      inline bool resize(const uint newSize) { return _buf.resize(newSize + 1); }
      inline bool resize_exact(const uint newSize) { return _buf.resize_exact(newSize + 1); }
      char* release() { return _buf.release(); }
      bool push_back(const char ch) { _buf.back() = ch; return _buf.push_back('\0'); }
      char pop_back() { const char tmp = back(); _buf.pop_back(); _buf.back() = '\0'; return tmp; }


      string& operator+=(const str_t auto& other);
      string& operator*=(const uint repeatCount);
};

#endif //MCSL_STRING_HPP