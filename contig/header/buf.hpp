#pragma once
#ifndef MCSL_BUF_HPP
#define MCSL_BUF_HPP

#include "MCSL.hpp"
#include "contig_base.hpp"
#include "alloc.hpp"

#include <memory>
#include "assert.hpp"

template<typename T, uint _capacity> class [[clang::trivial_abi]] mcsl::buf : mcsl::contig_base<T> {
   private:
      T _buf[_capacity];
      uint _size;

      static constexpr const char _nameof[] = "buf";
   public:
      static constexpr const auto& nameof() { return _nameof; }
      
      constexpr buf():_buf{},_size{} {}
      constexpr buf(const contig_t auto& other);
      constexpr buf(castable_to<T> auto&&... initList) requires (sizeof...(initList) <= _capacity);

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
      T* emplace(const uint i, auto&&... args) requires valid_ctor<T, decltype(args)...>;
      T* emplace_back(auto&&... args) requires valid_ctor<T, decltype(args)...>;
};

template<typename T, uint _capacity> constexpr mcsl::buf<T,_capacity>::buf(const contig_t auto& other):
   _buf{},
   _size(other.size()) {
      assert(_size <= _capacity, __OVERSIZED_COPY_MSG, ErrCode::SEGFAULT);
      
      for (uint i = 0; i < _size; ++i) {
         _buf[i] = other[i];
      }
}
template<typename T, uint _capacity> constexpr mcsl::buf<T,_capacity>::buf(castable_to<T> auto&&... initList) requires (sizeof...(initList) <= _capacity):
   _buf{initList...},
   _size(sizeof...(initList)) {

}

template<typename T, uint _capacity> T* mcsl::buf<T,_capacity>::push_back(T&& obj) {
   safe_mode_assert(_size < _capacity);
   _buf[_size] = obj;
   return _buf + (_size++);
}
template<typename T, uint _capacity> T* mcsl::buf<T,_capacity>::push_back(const T& obj) {
   safe_mode_assert(_size < _capacity);
   return new (begin() + (_size++)) T{std::forward<decltype(obj)>(obj)};
}
template<typename T, uint _capacity> bool mcsl::buf<T,_capacity>::pop_back() {
   safe_mode_assert(_size);
   --_size;
   std::destroy_at(self.end());
   return true;
}
template<typename T, uint _capacity> T* mcsl::buf<T,_capacity>::emplace(const uint i, auto&&... args) requires valid_ctor<T, decltype(args)...> {
   safe_mode_assert(i < _size);
   
   return new (begin() + i) T{args...};
}
template<typename T, uint _capacity> T* mcsl::buf<T,_capacity>::emplace_back(auto&&... args) requires valid_ctor<T, decltype(args)...> {
   safe_mode_assert(_size < _capacity);

   return new (begin() + _size++) T{args...};
}


#endif //MCSL_BUF_HPP