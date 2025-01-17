#pragma once
#ifndef MCSL_ARRAY_HPP
#define MCSL_ARRAY_HPP

#include "MCSL.hpp"
#include "contig_base.hpp"
#include "alloc.hpp"
#include "raw_str.hpp"

#include <memory>
#include <utility>

//!owning array with automatic scope
template <typename T> class mcsl::array : public contig_base<T> {
   private:
      T* _buf;
      const uint _size;

      static constexpr const raw_str _nameof = "array";
   public:
      static constexpr const auto& nameof() { return _nameof; }

      constexpr array():_buf{},_size{} {}
      array(const uint size);
      array(const T* buf, const uint size);
      array(array&& other);
      array(const array& other);
      array(castable_to<T> auto... initList) requires requires { sizeof...(initList) == _size; }:_buf{std::forward<decltype(initList)>(initList)...} {}
      ~array() { self.free(); }
      void free() const { mcsl::free(_buf); const_cast<T*&>(_buf) = nullptr; const_cast<uint&>(_size) = 0; }

      [[gnu::pure]] constexpr uint size() const { return _size; }
      
      //member access
      [[gnu::pure]] constexpr T* const* ptr_to_buf() { return &_buf; }
      [[gnu::pure]] constexpr T* data() { return _buf; }
      [[gnu::pure]] constexpr T* begin() { return _buf; }
      [[gnu::pure]] constexpr const T* const* ptr_to_buf() const { return &_buf; }
      [[gnu::pure]] constexpr const T* data() const { return _buf; }
      [[gnu::pure]] constexpr const T* begin() const { return _buf; }

      //MODIFIERS
      constexpr T* emplace(const uint i, auto&&... args);
      T* release() { T* temp = _buf; _buf = nullptr; const_cast<uint&>(_size) = 0; return temp; }
};

#pragma region src
// //!default constructor
// template<typename T> constexpr mcsl::array<T>::array():
//    _buf(nullptr),_size(0) {

// }
//!allocate array by size (in elements)
template<typename T> mcsl::array<T>::array(const uint size):
   _buf(mcsl::calloc<T>(size)),_size(size) {

}
//!copy constructor from raw pointer to buffer and size of buffer (in elements)
template<typename T> mcsl::array<T>::array(const T* buf, const uint size):
   _buf(mcsl::malloc<T>(size)),_size(size) {
      for (uint i = 0; i < _size; ++i) {
         _buf[i] = buf[i];
      }
}
//!move constructor
template<typename T> mcsl::array<T>::array(array&& other):
   _buf(other._buf),_size(other._size) {
      if (this != &other) { other.release(); }
}
//!copy constructor
template<typename T> mcsl::array<T>::array(const array& other):
   array{other._size} {
      for (uint i = 0; i < _size; ++i) {
         self[i] = other[i];
      }
}

//!construct in place
template<typename T> constexpr T* mcsl::array<T>::emplace(const uint i, auto&&... args) {
   if (i >= _size) {
      mcsl_throw(ErrCode::SEGFAULT, "emplace at \033[4m%u\033[24m in %s of size \033[4m%u\033[24m", i, self.nameof().begin(), _size);
      return nullptr;
   }
   return new (begin() + i) T{args...};
}

#pragma endregion src

#endif //MCSL_ARRAY_HPP