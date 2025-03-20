#pragma once
#ifndef MCSL_DYN_ARR_HPP
#define MCSL_DYN_ARR_HPP

#include "MCSL.hpp"
#include "contig_base.hpp"
#include "alloc.hpp"

#include <bit>
#include <memory>
#include <utility>
#include <initializer_list>

template <typename T> class mcsl::dyn_arr : public contig_base<T> {
   friend class string;
   friend class cstr;

   private:
      uint _capacity;
      uint _size;
      T* _buf;

      // static constexpr const raw_str _nameof = "dyn_arr";
      static constexpr const char _nameof[] = "dyn_arr";
      dyn_arr(const uint size, const uint capacity);
   public:
      static constexpr const auto& nameof() { return _nameof; }

      constexpr dyn_arr();
      dyn_arr(const uint capacity);
      dyn_arr(castable_to<T> auto&&... initList);
      dyn_arr(dyn_arr&& other);
      dyn_arr(const dyn_arr& other);
      template<contig_t<T> Other_t> dyn_arr(const Other_t& other);
      ~dyn_arr() { for (uint i = 0; i < _size; ++i) { std::destroy_at(_buf + i); } self.free(); }
      void free() const { mcsl::free(_buf); const_cast<T*&>(_buf) = nullptr; const_cast<uint&>(_capacity) = 0; const_cast<uint&>(_size) = 0; }

      dyn_arr& operator=(dyn_arr&&) = default;
      dyn_arr& operator=(const dyn_arr&) = default;

      [[gnu::pure]] constexpr uint size() const { return _size; }
      [[gnu::pure]] constexpr uint capacity() const { return _capacity; }

      //member access
      [[gnu::pure]] constexpr T* const* ptr_to_buf() { return &_buf; }
      [[gnu::pure]] constexpr T* data() { return _buf; }
      [[gnu::pure]] constexpr T* begin() { return _buf; }
      [[gnu::pure]] constexpr const T* const* ptr_to_buf() const { return &_buf; }
      [[gnu::pure]] constexpr const T* data() const { return _buf; }
      [[gnu::pure]] constexpr const T* begin() const { return _buf; }


      //MODIFIERS
      //!realloc buffer to at least the specified size
      bool reserve(const uint newSize) { return reserve_exact(std::bit_ceil(newSize)); }
      bool reserve_exact(const uint newSize);
      T* release() { _size = 0; _capacity = 0; T* temp = _buf; _buf = nullptr; return temp; }
      T* push_back(T&& obj);
      T* push_back(const T& obj);
      T pop_back();
      T* emplace(const uint i, auto&&... args) requires valid_ctor<T, decltype(args)...>;
      T* emplace_back(auto&&... args) requires valid_ctor<T, decltype(args)...>;
};

#pragma region src
//!private constructor for implementations
template<typename T> mcsl::dyn_arr<T>::dyn_arr(const uint size, const uint capacity):
   _capacity(std::bit_ceil(capacity)), _size(size),
   _buf(mcsl::calloc<T>(_capacity)) {
      debug_assert(size <= capacity);
}
//!default constructor
template<typename T> constexpr mcsl::dyn_arr<T>::dyn_arr():
   _capacity(0),_size(0),_buf(nullptr) {

}
//!constructor from initial capacity
template<typename T> mcsl::dyn_arr<T>::dyn_arr(const uint capacity):
   _capacity(std::bit_ceil(capacity)), _size(0),
   _buf(mcsl::calloc<T>(_capacity)) {

}
//!constructor from initializer list
template<typename T> mcsl::dyn_arr<T>::dyn_arr(castable_to<T> auto&&... initList):
   dyn_arr(sizeof...(initList), sizeof...(initList)) {
      const T* tmp = std::data(std::initializer_list<T>{initList...});

      for (uint i = 0; i < _size; ++i) {
         _buf[i] = tmp[i];
      }
}
//!move constructor
template<typename T> mcsl::dyn_arr<T>::dyn_arr(dyn_arr&& other):
   _capacity(other._capacity),_size(other._size),_buf(other._buf) {
      if (this != &other) { other.release(); }
}
//!copy constructor
template<typename T> mcsl::dyn_arr<T>::dyn_arr(const dyn_arr& other):
   dyn_arr(other._size, other._capacity) {
      for (uint i = 0; i < _size; ++i) {
         self[i] = other[i];
      }
}
template<typename T> template<mcsl::contig_t<T> Other_t> mcsl::dyn_arr<T>::dyn_arr(const Other_t& other):
   dyn_arr(other.size(), other.size()) {
      for (uint i = 0; i < _size; ++i) {
         self[i] = other[i];
      }
}

//!realloc buffer to the specified size
template<typename T> bool mcsl::dyn_arr<T>::reserve_exact(const uint newSize) {
   _buf = mcsl::realloc<T>(_buf, newSize);
   _capacity = newSize;
   _size = _size > _capacity ? _capacity : _size;
   return true;
}

//!push to the back of the the array
template<typename T> T* mcsl::dyn_arr<T>::push_back(T&& obj) {
   if (_size >= _capacity) {
      reserve(_size ? std::bit_floor(_size) << 1 : 1);
   }
   return new (_buf + _size++) T{std::forward<decltype(obj)>(obj)};
}
template<typename T> T* mcsl::dyn_arr<T>::push_back(const T& obj) {
   if (_size >= _capacity) {
      reserve(_size ? std::bit_floor(_size) << 1 : 1);
   }
   return new (_buf + _size++) T{obj};
}
//!remove last element of array
//!returns the removed element
template<typename T> T mcsl::dyn_arr<T>::pop_back() {
   assume(_size);
   T temp = _buf[--_size];
   std::destroy_at(self.end());
   return temp;
}
//!construct in place
template<typename T> T* mcsl::dyn_arr<T>::emplace(const uint i, auto&&... args) requires valid_ctor<T, decltype(args)...> {
   assume(i < _size);

   return new (begin() + i) T{args...};
}
//!construct in place at back of array
template<typename T> T* mcsl::dyn_arr<T>::emplace_back(auto&&... args) requires valid_ctor<T, decltype(args)...> {
   if (_size >= _capacity) {
      reserve(_size ? std::bit_floor(_size) << 1 : 1);
   }
   return new (begin() + _size++) T{args...};
}

#pragma endregion src


#endif //MCSL_DYN_ARR_HPP