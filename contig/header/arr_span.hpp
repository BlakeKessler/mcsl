#pragma once
#ifndef MCSL_ARR_SPAN_HPP
#define MCSL_ARR_SPAN_HPP

#include "MCSL.hpp"
#include "contig_base.hpp"
#include "raw_str.hpp"
#include <memory>
#include <utility>

//!non-owning array
template <typename T> class [[clang::trivial_abi]] mcsl::arr_span : public contig_base<T> {
   private:
      T* _buf;
      uint _size;

      static constexpr const raw_str _nameof = "arr_span";
   public:
      static constexpr const auto& nameof() { return _nameof; }

      constexpr arr_span():_buf{},_size{} {}
      constexpr arr_span(T* begin, T* end):_buf{begin},_size{end-begin} { assert(begin <= end); }
      constexpr arr_span(T* buf, const uint size):_buf{buf},_size{size} {}
      constexpr arr_span(const contig_t auto& other):arr_span{other.begin(), other.size()} {}
      constexpr arr_span(const contig_t auto& other, const uint size):arr_span{other.begin(), size} { assert(size <= other.size()); }
      constexpr arr_span(const contig_t auto& other, const uint begin, const uint size):arr_span{other.begin()+begin, size} { assert((begin+size) <= other.size()); }

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

#pragma region src
//!construct in place
template<typename T> constexpr T* mcsl::arr_span<T>::emplace(const uint i, auto&&... args) {
   if (i >= _size) {
      mcsl_throw(ErrCode::SEGFAULT, "emplace at \033[4m%u\033[24m in %s of size \033[4m%u\033[24m", i,nameof(),_size);
      return nullptr;
   }
   return new (begin() + i) T{args...};
}

#pragma endregion src


#endif //MCSL_ARR_SPAN_HPP