#pragma once
#ifndef MCSL_DYN_ARR_SPAN_HPP
#define MCSL_DYN_ARR_SPAN_HPP

#include "MCSL.hpp"
#include "contig_base.hpp"
#include "pair.hpp"
#include "raw_str.hpp"

#include <memory>
#include <utility>

template <typename T> class [[clang::trivial_abi]] mcsl::dyn_arr_span : public contig_base<T> {
   private:
      T* const* _ptrToBuf;
      uint _beginIndex;
      uint _size;

      static constexpr const raw_str _nameof = "dyn_arr_span";
   public:
      static constexpr const auto& nameof() { return _nameof; }

      constexpr dyn_arr_span();
      constexpr dyn_arr_span(T* const* ptrToBuf, const uint size);
      constexpr dyn_arr_span(T* const* ptrToBuf, const uint beginIndex, const uint size);
      constexpr dyn_arr_span(T* const* ptrToBuf, const mcsl::pair<uint,uint> bounds);
      constexpr dyn_arr_span(const contig_t<T> auto& other) requires   requires{other.first_index();} ;
      constexpr dyn_arr_span(const contig_t<T> auto& other) requires (!requires{other.first_index();});
      constexpr dyn_arr_span(const contig_t<T> auto& other, const uint size) requires   requires{other.first_index();} ;
      constexpr dyn_arr_span(const contig_t<T> auto& other, const uint size) requires (!requires{other.first_index();});
      constexpr dyn_arr_span(const contig_t<T> auto& other, const uint begin, const uint size) requires   requires{other.first_index();} ;
      constexpr dyn_arr_span(const contig_t<T> auto& other, const uint begin, const uint size) requires (!requires{other.first_index();});

      [[gnu::pure]] constexpr uint size() const { return _size; }

      //element access
      [[gnu::pure]] constexpr T* const* ptr_to_buf() { return _ptrToBuf; }
      [[gnu::pure]] constexpr T* data() { safe_mode_assert(_ptrToBuf); return *_ptrToBuf; }
      [[gnu::pure]] constexpr T* begin() { safe_mode_assert(_ptrToBuf); return data() + _beginIndex; }
      
      [[gnu::pure]] constexpr const T* const* ptr_to_buf() const { return _ptrToBuf; }
      [[gnu::pure]] constexpr const T* data() const { safe_mode_assert(_ptrToBuf); return *_ptrToBuf; }
      [[gnu::pure]] constexpr const T* begin() const { return data() + _beginIndex; }

      [[gnu::pure]] constexpr uint first_index() const { return _beginIndex; }
      [[gnu::pure]] constexpr uint last_index() const { return _beginIndex + _size; }

      //MODIFIERS
      constexpr T* emplace(const uint i, auto&&... args) requires valid_ctor<T, decltype(args)...>;
};

#pragma region src
template<typename T> constexpr mcsl::dyn_arr_span<T>::dyn_arr_span():
   _ptrToBuf{},
   _beginIndex{},
   _size{} {

}

template<typename T> constexpr mcsl::dyn_arr_span<T>::dyn_arr_span(T* const* ptrToBuf, const uint size):
   _ptrToBuf{ptrToBuf},
   _beginIndex{0},
   _size{size} {
      safe_mode_assert(_ptrToBuf || !_size);
}
template<typename T> constexpr mcsl::dyn_arr_span<T>::dyn_arr_span(T* const* ptrToBuf, const uint beginIndex, const uint size):
   _ptrToBuf{ptrToBuf},
   _beginIndex{beginIndex},
   _size{size} {
      safe_mode_assert(_ptrToBuf || !(_beginIndex || _size));
}

template<typename T> constexpr mcsl::dyn_arr_span<T>::dyn_arr_span(T* const* ptrToBuf, const mcsl::pair<uint,uint> bounds):
   dyn_arr_span{ptrToBuf, bounds.first, bounds.second-bounds.first} {

}

template<typename T> constexpr mcsl::dyn_arr_span<T>::dyn_arr_span(const contig_t<T> auto& other)
requires requires{other.first_index();}:
   dyn_arr_span{other.ptr_to_buf(), other.first_index(), other.size()} {

}
template<typename T> constexpr mcsl::dyn_arr_span<T>::dyn_arr_span(const contig_t<T> auto& other)
requires (!requires{other.first_index();}):
   dyn_arr_span{other.ptr_to_buf(), other.size()} {

}

template<typename T> constexpr mcsl::dyn_arr_span<T>::dyn_arr_span(const contig_t<T> auto& other, const uint size)
requires requires{other.first_index();}:
   dyn_arr_span{other.ptr_to_buf(), other.first_index(), size} {
      assert(size <= other.size(), __OVERSIZED_SPAN_MSG, ErrCode::SEGFAULT);
}
template<typename T> constexpr mcsl::dyn_arr_span<T>::dyn_arr_span(const contig_t<T> auto& other, const uint size)
requires (!requires{other.first_index();}):
   dyn_arr_span{other.ptr_to_buf(), size} {
      assert(size <= other.size(), __OVERSIZED_SPAN_MSG, ErrCode::SEGFAULT);
}

template<typename T> constexpr mcsl::dyn_arr_span<T>::dyn_arr_span(const contig_t<T> auto& other, const uint begin, const uint size)
requires requires{other.first_index();}:
   dyn_arr_span{other.ptr_to_buf(), other.first_index()+begin, size} {
      assert((begin+size) <= other.size(), __OVERSIZED_SPAN_MSG, ErrCode::SEGFAULT);
}
template<typename T> constexpr mcsl::dyn_arr_span<T>::dyn_arr_span(const contig_t<T> auto& other, const uint begin, const uint size)
requires (!requires{other.first_index();}):
   dyn_arr_span{other.ptr_to_buf(), begin, size} {
      assert((begin+size) <= other.size(), __OVERSIZED_SPAN_MSG, ErrCode::SEGFAULT);
}

//!construct in place
template<typename T> constexpr T* mcsl::dyn_arr_span<T>::emplace(const uint i, auto&&... args) requires valid_ctor<T, decltype(args)...> {
   safe_mode_assert(i < _size);

   return new (begin() + i) T{args...};
}

#pragma endregion src


#endif //MCSL_DYN_ARR_SPAN_HPP