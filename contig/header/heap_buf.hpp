#pragma once
#ifndef MCSL_HEAP_BUF_HPP
#define MCSL_HEAP_BUF_HPP

#include "MCSL.hpp"
#include "alloc.hpp"

#include <memory>
#include "assert.hpp"

template<typename T, uint _capacity> class mcsl::heap_buf : mcsl::contig_base<T> {
   private:
      T* _buf;
      uint _size;

      static constexpr const char _nameof[] = "heap_buf";
   public:
      static constexpr const auto& nameof() { return _nameof; }
      
      // constexpr heap_buf():_buf(),_size() {}
      heap_buf():_buf(mcsl::calloc<T>(_capacity)),_size(0) {}
      heap_buf(heap_buf&& other): _buf(other._buf),_size(other._size) { if (this != &other) { other.release(); } }
      heap_buf(const contig_t auto& other);
      heap_buf(castable_to<T> auto&&... initList);

      ~heap_buf() { self.free(); }
      void free() const { mcsl::free(_buf); const_cast<T*&>(_buf) = nullptr; const_cast<uint&>(_size) = 0; }
      T* release() { T* temp = _buf; _buf = nullptr; _size = 0; return temp; }


      [[gnu::pure]] constexpr uint size() const { return _size; }
      [[gnu::pure]] constexpr uint capacity() const { return _capacity; }

      //member access
      [[gnu::pure]] constexpr T* const* ptr_to_buf() { return &_buf; }
      [[gnu::pure]] constexpr T* data() { return _buf; }
      [[gnu::pure]] constexpr T* begin() { return _buf; }
      [[gnu::pure]] constexpr const T* const* ptr_to_buf() const { return &_buf; }
      [[gnu::pure]] constexpr const T* data() const { return _buf; }
      [[gnu::pure]] constexpr const T* begin() const { return _buf; }

      //modifiers
      T* push_back(T&& obj);
      T* push_back(const T& obj);
      bool pop_back();
      T* emplace(const uint i, auto&&... args);
      T* emplace_back(auto&&... args);
};

template<typename T, uint _capacity> mcsl::heap_buf<T,_capacity>::heap_buf(const contig_t auto& other):
   _buf(mcsl::malloc<T>(_capacity)),
   _size(other.size()) {
      assert(_size <= _capacity);

      for (uint i = 0; i < _size; ++i) {
         _buf[i] = other[i];
      }
}
template<typename T, uint _capacity> mcsl::heap_buf<T,_capacity>::heap_buf(castable_to<T> auto&&... initList):
   _buf(initList...),
   // _buf(mcsl::malloc<T>(_capacity)),
   _size(sizeof...(initList)) {
      assert(_size <= _capacity);

      // for (uint i = 0; i < _size; ++i) {
      //    _buf[i] = initList[i];
      // }
}

template<typename T, uint _capacity> T* mcsl::heap_buf<T,_capacity>::push_back(T&& obj) {
   if (_size >= _capacity) {
      return nullptr;
   }
   _buf[_size] = obj;
   return _buf + (_size++);
}
template<typename T, uint _capacity> T* mcsl::heap_buf<T,_capacity>::push_back(const T& obj) {
   if (_size >= _capacity) {
      return nullptr;
   }
   return new (begin() + (_size++)) T{std::forward<decltype(obj)>(obj)};
}
template<typename T, uint _capacity> bool mcsl::heap_buf<T,_capacity>::pop_back() {
   if (!_size) {
      return false;
   }
   --_size;
   std::destroy_at(self.end());
   return true;
}
template<typename T, uint _capacity> T* mcsl::heap_buf<T,_capacity>::emplace(const uint i, auto&&... args) {
   if (i >= _size) {
      mcsl_throw(ErrCode::SEGFAULT, "emplace at \033[4m%u\033[24m in %s of size \033[4m%u\033[24m", i, nameof(), _size);
      return nullptr;
   }
   return new (begin() + i) T{args...};
}
template<typename T, uint _capacity> T* mcsl::heap_buf<T,_capacity>::emplace_back(auto&&... args) {
   if (_size >= _capacity) {
      return nullptr;
   }
   return new (begin() + (_size++)) T{args...};
}


#endif //MCSL_HEAP_BUF_HPP