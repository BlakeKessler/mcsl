#pragma once
#ifndef ARR_LIST_HPP
#define ARR_LIST_HPP

#include "MCSL.hpp"
#include "dyn_arr.hpp"
#include "heap_buf.hpp"

template<typename T, uint _bufCapacity = DEFAULT_ARR_LIST_BUF_SIZE> class arr_list {
   private:
      mcsl::dyn_arr<mcsl::heap_buf<T,_bufCapacity>> _bufBuf;
   public:
      // typedef it = it<T,arr_list>;
      // typedef const_it = it<const T, const arr_list>;
      // typedef span = span<T, arr_list>;
      // typedef const_span = span<const T, const arr_list>;

      uint size() const;
      operator bool() const { return size(); }

      mcsl::it<T,arr_list> operator+(const uint i);
      T& operator[](const uint i);
      T& at(const uint i);
      mcsl::it<T,arr_list> begin();
      mcsl::it<T,arr_list> end();
      T& front();
      T& back();

      mcsl::it<const T, const arr_list> operator+(const uint i) const;
      const T& operator[](const uint i) const;
      const T& at(const uint i) const;
      mcsl::it<const T, const arr_list> begin() const;
      mcsl::it<const T, const arr_list> end() const;
      const T& front() const;
      const T& back() const;

      T* push_back(const T&);
      T* emplace_back(auto&&... initList) requires valid_ctor<T, decltype(initList)...>;
      T pop_back();
};

#endif //ARR_LIST_HPP