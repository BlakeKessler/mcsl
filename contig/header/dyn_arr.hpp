#pragma once
#ifndef MCSL_DYN_ARR_HPP
#define MCSL_DYN_ARR_HPP

#include "MCSL.hpp"
#include "contig_base.hpp"
#include "alloc.hpp"

#include <bit>
#include <memory>
#include <cstring>
#include <utility>

template <typename T> class mcsl::dyn_arr : public contig_base<T> {
   private:
      uint _bufSize;
      uint _size;
      T* _buf;

      // static constexpr const raw_str _nameof = "dyn_arr";
      static constexpr const char _nameof[] = "dyn_arr";
   public:
      static constexpr const auto& nameof() { return _nameof; }

      constexpr dyn_arr();
      dyn_arr(const uint size);
      dyn_arr(const uint size, const uint bufSize);
      dyn_arr(castable_to<T> auto&... initList) requires requires { sizeof...(initList) == _size; }:_buf{std::forward<decltype(initList)>(initList)...} {}
      dyn_arr(dyn_arr&& other);
      dyn_arr(const dyn_arr& other);
      ~dyn_arr() { self.free(); }
      void free() const { mcsl::free(_buf); const_cast<T*&>(_buf) = nullptr; const_cast<uint&>(_bufSize) = 0; const_cast<uint&>(_size) = 0; }

      [[gnu::pure]] constexpr uint size() const { return _size; }
      [[gnu::pure]] constexpr uint capacity() const { return _bufSize; }

      //member access
      [[gnu::pure]] constexpr T* const* ptr_to_buf() { return &_buf; }
      [[gnu::pure]] constexpr T* data() { return _buf; }
      [[gnu::pure]] constexpr T* begin() { return _buf; }
      [[gnu::pure]] constexpr const T* const* ptr_to_buf() const { return &_buf; }
      [[gnu::pure]] constexpr const T* data() const { return _buf; }
      [[gnu::pure]] constexpr const T* begin() const { return _buf; }


      //MODIFIERS
      //!realloc buffer to at least the specified size
      bool resize(const uint newSize) { return resize_exact(std::bit_ceil(newSize)); }
      bool resize_exact(const uint newSize);
      T* release() { T* temp = _buf; _buf = nullptr; _size = 0; _bufSize = 0; return temp; }
      bool push_back(T&& obj);
      bool push_back(const T& obj);
      T pop_back();
      T* emplace(const uint i, auto&&... args);
      T* emplace_back(auto&&... args);
};

#pragma region src
//!default constructor
template<typename T> constexpr mcsl::dyn_arr<T>::dyn_arr():
   _bufSize(0),_size(0),_buf(nullptr) {

}
//!constructor from array size
template<typename T> mcsl::dyn_arr<T>::dyn_arr(const uint size):
   _bufSize(std::bit_ceil(size)), _size(size),
   _buf(mcsl::calloc<T>(_bufSize)) {

}
//!constructor from array size and buffer size
template<typename T> mcsl::dyn_arr<T>::dyn_arr(const uint size, const uint bufSize):
   _bufSize(bufSize), _size(size) {
      if (_bufSize < _size) {
         mcsl::mcsl_throw(ErrCode::SEGFAULT, "cannot construct %s with array size greater than buffer size (\033[4m%u\033[24m < \033[4m%u\033[24m)", _nameof,_bufSize,_size);
         _buf = nullptr;
      }
      else {
         _buf = mcsl::calloc<T>(_bufSize);
      }
}
//!move constructor
template<typename T> mcsl::dyn_arr<T>::dyn_arr(dyn_arr&& other):
   _bufSize(other._bufSize),_size(other._size),_buf(other._buf) {
      if (this != &other) { other.release(); }
}
//!copy constructor
template<typename T> mcsl::dyn_arr<T>::dyn_arr(const dyn_arr& other):
   dyn_arr{other._size, other._bufSize} {
      for (uint i = 0; i < _size; ++i) {
         self[i] = other[i];
      }
}

//!realloc buffer to the specified size
template<typename T> bool mcsl::dyn_arr<T>::resize_exact(const uint newSize) {
   _buf = mcsl::realloc<T>(_buf, newSize);
   _bufSize = newSize;
   return true;
}

//!push to the back of the the array
//!returns if a reallocation was required
template<typename T> bool mcsl::dyn_arr<T>::push_back(T&& obj) {
   bool realloced = false;
   if (_size >= _bufSize) {
      realloced = resize(_size ? std::bit_floor(_size) << 1 : 1);
   }
   new (&_buf[_size++]) T{std::forward<decltype(obj)>(obj)};
   return realloced;
}
template<typename T> bool mcsl::dyn_arr<T>::push_back(const T& obj) {
   bool realloced = false;
   if (_size >= _bufSize) {
      realloced = resize(_size ? std::bit_floor(_size) << 1 : 1);
   }
   _buf[_size++] = obj;
   return realloced;
}
//!remove last element of array
//!returns the removed element
//!zeroes out the index in the array
template<typename T> T mcsl::dyn_arr<T>::pop_back() {
   T temp = _buf[--_size];
   // std::memset(_buf + _size, 0, sizeof(T));
   std::destroy_at(self.end());
   return temp;
}
//!construct in place
template<typename T> T* mcsl::dyn_arr<T>::emplace(const uint i, auto&&... args) {
   if (i >= _size) {
      mcsl_throw(ErrCode::SEGFAULT, "emplace at \033[4m%u\033[24m in %s of size \033[4m%u\033[24m", i, self.nameof(), _size);
      return nullptr;
   }
   return new (begin() + i) T{args...};
}
//!construct in place at back of array
template<typename T> T* mcsl::dyn_arr<T>::emplace_back(auto&&... args) {
   if (_size >= _bufSize) {
      resize(_size ? std::bit_floor(_size) << 1 : 1);
   }
   return emplace(_size++, std::forward<decltype(args)>(args)...);
}

#pragma endregion src


#endif //MCSL_DYN_ARR_HPP