#pragma once
#ifndef MCSL_STATIC_ARR_HPP
#define MCSL_STATIC_ARR_HPP

#include "MCSL.hpp"
#include "contig_base.hpp"
#include "raw_str.hpp"
#include "assert.hpp"

#include <utility>

template <typename T, uint _size> class [[clang::trivial_abi]] mcsl::static_arr : public contig_base<T> {
   private:
      T _buf[_size];

      static constexpr const raw_str _nameof = "static_arr";
   public:
      static constexpr const auto& nameof() { return _nameof; }

      constexpr static_arr():_buf{} {}
      constexpr static_arr(const contig_t auto& other) requires requires{ other.size() >= _size; } { assert(other.size() >= _size); for (uint i = 0; i < _size; ++i) { _buf[i] = other[i]; }}
      constexpr static_arr(T (&other)[_size]):_buf{other} {}
      constexpr static_arr(castable_to<T> auto... initList) requires requires { sizeof...(initList) == _size; }:_buf{std::forward<decltype(initList)>(initList)...} {}

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
};

#pragma region CTAD
namespace mcsl {
   template<typename T, castable_to<T>... Argv_t> static_arr(T, Argv_t...) -> static_arr<T, sizeof...(Argv_t)+1>;
}
#pragma endregion CTAD

#pragma region src

//!construct in place
template<typename T,uint _size> constexpr T* mcsl::static_arr<T,_size>::emplace(const uint i, auto&&... args) {
   if (i >= _size) {
      mcsl_throw(ErrCode::SEGFAULT, "emplace at \033[4m%u\033[24m in %s of size \033[4m%u\033[24m", i, self.nameof(), _size);
      return nullptr;
   }
   return new (begin() + i) T{args...};
}

#pragma endregion src

#endif //MCSL_STATIC_ARR_HPP