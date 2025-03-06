#pragma once
#ifndef MCSL_STRING_HPP
#define MCSL_STRING_HPP

#include "MCSL.hpp"
#include "str_base.hpp"
#include "dyn_arr.hpp"

//!dynamically-sized string
class mcsl::string : public mcsl::str_base<char> {
   private:
      dyn_arr<char> _buf;
      static constexpr char _nameof[] = "string";
   public:
      string():_buf{} {}
      string(const uint capacity):_buf(capacity) {}
      string(string&& other):_buf(std::move(other._buf)) {}
      string(const string& other):_buf(other._buf) {}
      string(const str_t auto& other): _buf(other) {}
      template<uint len> string(const char (&str)[len+1]): string(str, len) {}

      void free() const { _buf.free(); }

      string& operator=(string&&) = default;
      string& operator=(const string&) = default;

      [[gnu::pure]] constexpr static const auto& nameof() { return _nameof; }

      [[gnu::pure]] constexpr uint size() const { return _buf.size(); }
      [[gnu::pure]] constexpr uint capacity() const { return _buf.capacity(); }

      //member access
      [[gnu::pure]] constexpr char* const* ptr_to_buf() { return _buf.ptr_to_buf(); }
      [[gnu::pure]] constexpr char* data() { return _buf.data(); }
      [[gnu::pure]] constexpr char* begin() { return _buf.begin(); }
      [[gnu::pure]] constexpr const char* const* ptr_to_buf() const { return _buf.ptr_to_buf(); }
      [[gnu::pure]] constexpr const char* data() const { return _buf.data(); }
      [[gnu::pure]] constexpr const char* begin() const { return _buf.begin(); }
      
      //MODIFIERS
      //!realloc buffer to at least the specified size
      bool reserve(const uint newSize) { return _buf.reserve(newSize); }
      bool reserve_exact(const uint newSize) { return _buf.reserve_exact(newSize); }
      char* release() { return _buf.release(); }
      char* push_back(char obj) { return _buf.push_back(obj); }
      char pop_back() { return _buf.pop_back(); }

      string& operator+=(const str_t auto& other);
      string& operator*=(const uint repeatCount);
};

#endif //MCSL_STRING_HPP