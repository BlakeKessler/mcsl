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
      struct it;

      uint size() const;
      operator bool() const { return size(); }

      it operator+(const uint i);
      T& operator[](const uint i);
      T& at(const uint i);
      it begin();
      it end();
      T& front();
      T& back();

      const it operator+(const uint i) const;
      const T& operator[](const uint i) const;
      const T& at(const uint i) const;
      const it begin() const;
      const it end() const;
      const T& front() const;
      const T& back() const;

      T* push_back(const T&);
      T* emplace_back(auto&&... initList) requires valid_ctor<T, decltype(initList)...>;
      T pop_back();
};

#endif //ARR_LIST_HPP