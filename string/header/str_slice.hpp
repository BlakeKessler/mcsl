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
      constexpr str_slice(char* begin, char* end):_buf(begin),_size(end-begin) { assert(begin <= end, __END_BEFORE_BEGIN_MSG, ErrCode::SEGFAULT); }
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
   return make(buf, std::strlen(buf));
}

//slicing
#include "str_slice.hpp"
template<typename char_t> constexpr const mcsl::str_slice mcsl::str_base<char_t>::slice(this const auto&& obj) {
   return {obj.begin(), obj.size()};
}
template<typename char_t> constexpr const mcsl::str_slice mcsl::str_base<char_t>::slice(this const auto&& obj, uint size) {
   assume(size <= obj.size());
   return {obj.begin(), size};
}
template<typename char_t> constexpr const mcsl::str_slice mcsl::str_base<char_t>::slice(this const auto&& obj, uint begin, uint size) {
   assume(begin + size <= obj.size());
   return {obj.begin() + begin, size};
}
template<typename char_t> constexpr mcsl::str_slice mcsl::str_base<char_t>::slice(this auto&& obj) {
   return str_slice::make(obj.begin(), obj.size());
}
template<typename char_t> constexpr mcsl::str_slice mcsl::str_base<char_t>::slice(this auto&& obj, uint size) {
   assume(size <= obj.size());
   return str_slice::make(obj.begin(), size);
}
template<typename char_t> constexpr mcsl::str_slice mcsl::str_base<char_t>::slice(this auto&& obj, uint begin, uint size) {
   assume(begin + size <= obj.size());
   return str_slice::make(obj.begin() + begin, size);
}


// #include "throw.hpp"


#pragma endregion inlinesrc

#endif //MCSL_STR_SLICE_HPP