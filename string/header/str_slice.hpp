#pragma once
#ifndef MCSL_STR_SLICE_HPP
#define MCSL_STR_SLICE_HPP

#include "MCSL.hpp"
#include "str_base.hpp"

//!non-owning potentially-non-null-terminated string
//!invalidated if the string is reallocated
//!relatively unsafe (even ignoring the invalidation)
//!   nothing stops setting/incrementing _buf or _size beyond the end of the spanned string
class [[clang::trivial_abi]] mcsl::str_slice : public str_base<char> {
   private:
      char* _buf;
      uint _size;

      static constexpr const char _nameof[] = "str_slice";
   public:
      //constructors
      constexpr str_slice(): _buf(),_size() {}
      constexpr str_slice(char* str, const uint size):_buf(str),_size(size) {}
      constexpr str_slice(char* begin, char* end):_buf(begin),_size(end-begin) { assert(begin <= end, __END_BEFORE_BEGIN_MSG, Errno::SEGFAULT); }
      constexpr str_slice(str_t auto& other): str_slice(other, other.size()) {}
      constexpr str_slice(str_t auto& other, const uint size): str_slice(other.begin(),size) { assert(other.size() >= size, __OVERSIZED_SPAN_MSG); }
      constexpr str_slice(str_t auto& other, const uint begin, const uint size): str_slice(other.begin() + begin, size) { assert(other.size() >= (begin + size), __OVERSIZED_SPAN_MSG); }

      static constexpr const str_slice make(const char* str, const uint size);
      static constexpr const str_slice make(const char* begin, const char* end);
      static constexpr const str_slice make(const str_t auto& other);
      static constexpr const str_slice make(const str_t auto& other, const uint size);
      static constexpr const str_slice make(const str_t auto& other, const uint begin, const uint size);
      static constexpr const str_slice make_from_cstr(const char*);

      //properties
      [[gnu::pure]] constexpr uint size() const { return _size; }
      [[gnu::pure]] constexpr static const auto& nameof() { return _nameof; }
      
      constexpr str_slice& inc_begin(const sint i) { _size -= i; _buf += i; return self; }
      constexpr str_slice& set_size(const uint i) { _size = i; return self; }
      constexpr str_slice& inc_end(const sint i) { _size += i; return self; }

      //member access
      [[gnu::pure]] constexpr char* const* ptr_to_buf() { return &_buf; }
      [[gnu::pure]] constexpr char* data() { return _buf; }
      [[gnu::pure]] constexpr char* begin() { return _buf; }
      [[gnu::pure]] constexpr const char* const* ptr_to_buf() const { return &_buf; }
      [[gnu::pure]] constexpr const char* data() const { return _buf; }
      [[gnu::pure]] constexpr const char* begin() const { return _buf; }

      //hashing
      constexpr uint64 hash() const { return hash_algos::rapid(_buf, _size); }
      constexpr uint64 hash(uint64 seed) const { return hash_algos::rapid_mix(hash(), seed); }
};



#pragma region inlinesrc

constexpr const mcsl::str_slice mcsl::str_slice::make(const char* str, const uint size) {
   return str_slice{const_cast<char*>(str), size};
}
constexpr const mcsl::str_slice mcsl::str_slice::make(const char* begin, const char* end) {
   return str_slice{const_cast<char*>(begin), const_cast<char*>(end)};
}
constexpr const mcsl::str_slice mcsl::str_slice::make(const str_t auto& other) {
   return make(other.begin(), other.size());
}
constexpr const mcsl::str_slice mcsl::str_slice::make(const str_t auto& other, const uint size) {
   assert(other.size() >= size, __OVERSIZED_SPAN_MSG);
   return make(other.begin(), size);
}
constexpr const mcsl::str_slice mcsl::str_slice::make(const str_t auto& other, const uint begin, const uint size) {
   assert(other.size() >= (begin + size), __OVERSIZED_SPAN_MSG);
   return make(other.begin() + begin, size);
}
constexpr const mcsl::str_slice mcsl::str_slice::make_from_cstr(const char* buf) {
   uint len = 0;
   while (buf[len] != '\0') {
      ++len;
   }
   return make(buf, len);
}

//slicing
#include "str_slice.hpp"
template<typename char_t> constexpr const mcsl::str_slice mcsl::str_base<char_t>::slice(this const auto& obj) {
   return {(char_t*)obj.begin(), obj.size()};
}
template<typename char_t> constexpr const mcsl::str_slice mcsl::str_base<char_t>::slice(this const auto& obj, uint size) {
   assume(size <= obj.size());
   return {(char_t*)obj.begin(), size};
}
template<typename char_t> constexpr const mcsl::str_slice mcsl::str_base<char_t>::slice(this const auto& obj, uint begin, uint end) {
   assume(begin <= end);
   assume(end <= obj.size());
   return {(char_t*)obj.begin() + begin, end - begin};
}
template<typename char_t> constexpr mcsl::str_slice mcsl::str_base<char_t>::slice(this auto& obj) {
   return str_slice::make(obj.begin(), obj.size());
}
template<typename char_t> constexpr mcsl::str_slice mcsl::str_base<char_t>::slice(this auto& obj, uint size) {
   assume(size <= obj.size());
   return str_slice::make(obj.begin(), size);
}
template<typename char_t> constexpr mcsl::str_slice mcsl::str_base<char_t>::slice(this auto& obj, uint begin, uint end) {
   assume(begin <= end);
   assume(end <= obj.size());
   return str_slice::make(obj.begin() + begin, end - begin);
}


template<typename char_t> constexpr mcsl::str_base<char_t>::operator const str_slice(this const auto& obj) {
   return obj.slice();
}
// template<typename char_t> constexpr mcsl::str_base<char_t>::operator str_slice(this auto& obj) {
//    return obj.slice();
// }


// #include "throw.hpp"


#pragma endregion inlinesrc

//default string hashing implementation
namespace {
   using namespace mcsl;
   struct __strhash {
      using is_transparent = void;
      using T = str_slice;

      inline uint64 operator()(const T str) const noexcept { return str.hash(); }
      inline uint64 operator()(const std::string_view str) const noexcept { return T::make(str.begin(), str.size()).hash(); }
      inline uint64 operator()(const std::string& str) const noexcept { return T::make(str.data(), str.size()).hash(); }

      inline uint64 operator()(const T str, uint64 seed) const noexcept { return str.hash(seed); }
      inline uint64 operator()(const std::string_view str, uint64 seed) const noexcept { return T::make(str.begin(), str.size()).hash(seed); }
      inline uint64 operator()(const std::string& str, uint64 seed) const noexcept { return T::make(str.data(), str.size()).hash(seed); }
   };
};
template<mcsl::str_t str_t> struct mcsl::hash<str_t> : public __strhash {};
template <mcsl::str_t str_t> struct std::hash<str_t> : public mcsl::hash<str_t> {};
//equality checking
template<mcsl::str_t str_t> struct std::equal_to<str_t> {
   using is_transparent = void;

   template<mcsl::str_t other_t> inline bool operator()(const str_t& lhs, const other_t& rhs) const noexcept { return lhs == rhs; }
};

template<typename char_t> constexpr uint64 mcsl::str_base<char_t>::hash(this const auto& obj) { return ((str_slice)obj).hash(); }
template<typename char_t> constexpr uint64 mcsl::str_base<char_t>::hash(this const auto& obj, uint64 seed) { return ((str_slice)obj).hash(seed); }

#endif //MCSL_STR_SLICE_HPP