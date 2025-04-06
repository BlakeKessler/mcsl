#pragma once
#ifndef MCSL_ARR_SPAN_HPP
#define MCSL_ARR_SPAN_HPP

#include "MCSL.hpp"
#include "contig_base.hpp"
#include <memory>
#include <utility>

//!non-owning array
template <typename T> class [[clang::trivial_abi]] mcsl::arr_span : public contig_base<T> {
   private:
      T* _buf;
      uint _size;

      static constexpr const char _nameof[] = "arr_span";
   public:
      static constexpr const auto& nameof() { return _nameof; }

      constexpr arr_span():_buf{},_size{} {}
      constexpr arr_span(T* buf, const uint size):_buf{buf},_size{size} { assume(!_size || _buf); }
      constexpr arr_span(T* begin, T* end):arr_span{begin, end-begin} { assert(begin <= end, __END_BEFORE_BEGIN_MSG, ErrCode::SEGFAULT); }
      constexpr arr_span(const contig_t<T> auto& other):arr_span{other.begin(), other.size()} {}
      constexpr arr_span(const contig_t<T> auto& other, const uint size):arr_span{other.begin(), size} { assert(size <= other.size(), __OVERSIZED_SPAN_MSG, ErrCode::SEGFAULT); }
      constexpr arr_span(const contig_t<T> auto& other, const uint begin, const uint size):arr_span{other.begin()+begin, size} { assert((begin+size) <= other.size(), __OVERSIZED_SPAN_MSG, ErrCode::SEGFAULT); }

      static constexpr const arr_span make(const T* str, const uint size);
      static constexpr const arr_span make(const T* begin, const T* end);
      static constexpr const arr_span make(const contig_t<T> auto& other);
      static constexpr const arr_span make(const contig_t<T> auto& other, const uint size);
      static constexpr const arr_span make(const contig_t<T> auto& other, const uint begin, const uint size);

      [[gnu::pure]] constexpr uint size() const { return _size; }

      //member access
      [[gnu::pure]] constexpr T* const* ptr_to_buf() { return &_buf; }
      [[gnu::pure]] constexpr T* data() { return _buf; }
      [[gnu::pure]] constexpr T* begin() { return _buf; }
      [[gnu::pure]] constexpr const T* const* ptr_to_buf() const { return &_buf; }
      [[gnu::pure]] constexpr const T* data() const { return _buf; }
      [[gnu::pure]] constexpr const T* begin() const { return _buf; }

      //MODIFIERS
      constexpr T* emplace(const uint i, auto&&... args) requires valid_ctor<T, decltype(args)...>;
};

#pragma region inlinesrc
//!construct in place
template<typename T> constexpr T* mcsl::arr_span<T>::emplace(const uint i, auto&&... args) requires valid_ctor<T, decltype(args)...> {
   assume(i < _size);
   return new (begin() + i) T{args...};
}

template<typename T> constexpr const mcsl::arr_span<T> mcsl::arr_span<T>::make(const T* buf, const uint size) {
   return arr_span{const_cast<T*>(buf), size};
}
template<typename T> constexpr const mcsl::arr_span<T> mcsl::arr_span<T>::make(const T* begin, const T* end) {
   return arr_span{const_cast<T*>(begin), const_cast<T*>(end)};
}
template<typename T> constexpr const mcsl::arr_span<T> mcsl::arr_span<T>::make(const contig_t<T> auto& other) {
   return make(other.begin(), other.size());
}
template<typename T> constexpr const mcsl::arr_span<T> mcsl::arr_span<T>::make(const contig_t<T> auto& other, const uint size) {
   assert(other.size() >= size, __OVERSIZED_SPAN_MSG);
   return make(other.begin(), size);
}
template<typename T> constexpr const mcsl::arr_span<T> mcsl::arr_span<T>::make(const contig_t<T> auto& other, const uint begin, const uint size) {
   assert(other.size() >= (begin + size), __OVERSIZED_SPAN_MSG);
   return make(other.begin() + begin, size);
}

//slicing
#include "arr_span.hpp"
template<typename T> constexpr const mcsl::arr_span<T> mcsl::contig_base<T>::span(this const auto&& obj) {
   return {const_cast<T*>(obj.begin()), obj.size()};
}
template<typename T> constexpr const mcsl::arr_span<T> mcsl::contig_base<T>::span(this const auto&& obj, uint size) {
   assume(size <= obj.size());
   return {const_cast<T*>(obj.begin()), size};
}
template<typename T> constexpr const mcsl::arr_span<T> mcsl::contig_base<T>::span(this const auto&& obj, uint begin, uint size) {
   assume(begin + size <= obj.size());
   return {const_cast<T*>(obj.begin()) + begin, size};
}
template<typename T> constexpr mcsl::arr_span<T> mcsl::contig_base<T>::span(this auto&& obj) {
   return arr_span<T>::make(obj.begin(), obj.size());
}
template<typename T> constexpr mcsl::arr_span<T> mcsl::contig_base<T>::span(this auto&& obj, uint size) {
   assume(size <= obj.size());
   return arr_span<T>::make(obj.begin(), size);
}
template<typename T> constexpr mcsl::arr_span<T> mcsl::contig_base<T>::span(this auto&& obj, uint begin, uint size) {
   assume(begin + size <= obj.size());
   return arr_span<T>::make(obj.begin() + begin, size);
}

#pragma endregion inlinesrc


#endif //MCSL_ARR_SPAN_HPP