#pragma once
#ifndef ARR_LIST_HPP
#define ARR_LIST_HPP

#include "MCSL.hpp"
#include "dyn_arr.hpp"

#include <bit>

template<typename T, uint _bufCapacity = DEFAULT_ARR_LIST_BUF_SIZE> class mcsl::arr_list {
   private:
      mcsl::dyn_arr<T*> _buf;
      uint _size;

      static constexpr char _nameof[] = "arr_list";
   public:
      static const char* nameof() { return _nameof; }
      using it = mcsl::it<T,arr_list>;
      using const_it = mcsl::it<const T, const arr_list>;
      using span = mcsl::span<T, arr_list>;
      using view = mcsl::span<const T, const arr_list>;

      arr_list():_buf{},_size{} {}
      arr_list(arr_list&& other):_buf{std::move(other._buf)},_size{other._size} { if (this != &other) { other.release(); } }
      arr_list(const arr_list& other): arr_list{view{other}} {}
      arr_list(const view data);
      arr_list(castable_to<T> auto&&... initList);

      ~arr_list() { for (uint i = 0; i < _size; ++i) { std::destroy_at(&self[i]); } free(); }
      void free() const;

      operator span() { return span{self, 0, size()}; }
      operator view() const { return view{self, 0, size()}; }

      uint size() const { return _size; }
      uint capacity() const { return _buf.size() * _bufCapacity; }
      operator bool() const { return _buf.size() && _buf[0].size(); }

      it operator+(const uint i) { safe_mode_assert(i < size()); return it{_buf, i}; }
      T& operator[](const uint i) { safe_mode_assert(i < size()); return _buf[i / _bufCapacity][i % _bufCapacity]; }
      T& at(const uint i) { if (i >= size()) { __throw(ErrCode::SEGFAULT, "%s of size %u accessed at index %u", nameof(), size(), i); } if (!_buf.data()) { __throw(ErrCode::SEGFAULT, "null %s dereferenced", nameof()); } return self[i]; }
      it begin() { return it{self, 0}; }
      it end() { return it{self, size()}; }
      T& front() { return self[0]; }
      T& back() { return self[size()-1]; }

      const_it operator+(const uint i) const { safe_mode_assert(i < size()); return it{_buf, i}; }
      const T& operator[](const uint i) const { safe_mode_assert(i < size()); return _buf[i / _bufCapacity][i % _bufCapacity]; }
      const T& at(const uint i) const { if (i >= size()) { __throw(ErrCode::SEGFAULT, "%s of size %u accessed at index %u", nameof(), size(), i); } if (!_buf.data()) { __throw(ErrCode::SEGFAULT, "null %s dereferenced", nameof()); } return self[i]; }
      const_it begin() const { return it{self, 0}; }
      const_it end() const { return it{self, size()}; }
      const T& front() const { return self[0]; }
      const T& back() const { return self[size()-1]; }

      auto release() { auto* tmp = _buf.data(); _buf.release(); return tmp; }

      T* push_back(T&&);
      T* push_back(const T&);
      T* emplace(const uint i, auto&&... initList) requires valid_ctor<T, decltype(initList)...>;
      T* emplace_back(auto&&... initList) requires valid_ctor<T, decltype(initList)...>;
      T pop_back();
};


#pragma region inlinesrc

template<typename T, uint _bufCapacity> mcsl::arr_list<T,_bufCapacity>::arr_list(const view data):_buf{},_size{} {
   const it end = data.end();
   for (it i = data.begin(); i != end; ++i) {
      push_back(*i);
   }
}

template<typename T, uint _bufCapacity> mcsl::arr_list<T,_bufCapacity>::arr_list(castable_to<T> auto&&... initList):_buf{},_size{} {
   T* tmp = const_cast<T*>(std::data(std::initializer_list<T>{initList...}));

   for (uint i = 0; i < _size; ++i) {
      push_back(tmp[i]);
   }
}

template<typename T, uint _bufCapacity> void mcsl::arr_list<T,_bufCapacity>::free() const {
   for (uint i = 0; i < _buf.size(); ++i) {
      std::destroy_n(_buf[i], _bufCapacity);
      mcsl::free(_buf[i]);
   }
   _buf.free();
}

template<typename T, uint _bufCapacity> T* mcsl::arr_list<T,_bufCapacity>::push_back(T&& obj) {
   it tmp = end();
   ++_size;
   if (+tmp % _bufCapacity == 0) {
      _buf.push_back(mcsl::calloc<T>(_bufCapacity));
   }
   return &(*tmp = obj);
}
template<typename T, uint _bufCapacity> T* mcsl::arr_list<T,_bufCapacity>::push_back(const T& obj) {
   it tmp = end();
   ++_size;
   if (+tmp % _bufCapacity == 0) {
      _buf.push_back(mcsl::calloc<T>(_bufCapacity));
   }
   return &(*tmp = obj);
}

template<typename T, uint _bufCapacity> T* mcsl::arr_list<T,_bufCapacity>::emplace(const uint i, auto&&... initList) requires valid_ctor<T, decltype(initList)...> {
   return new (_buf[i / _bufCapacity] + (i % _bufCapacity)) T{initList...};
}
template<typename T, uint _bufCapacity> T* mcsl::arr_list<T,_bufCapacity>::emplace_back(auto&&... initList) requires valid_ctor<T, decltype(initList)...> {
   it tmp = end();
   ++_size;
   if (+tmp % _bufCapacity == 0) {
      _buf.push_back(mcsl::calloc<T>(_bufCapacity));
   }
   return emplace(+tmp, initList...);
}

template<typename T, uint _bufCapacity> T mcsl::arr_list<T,_bufCapacity>::pop_back() {
   T poppedElem = back();
   if (--_size % _bufCapacity == 0) {
      mcsl::free(_buf[_size / _bufCapacity]);
      _buf[_size / _bufCapacity] = nullptr;
   }
   return poppedElem;
}

#pragma endregion inlinesrc


template<typename T, uint _bufCapacity> struct mcsl::it<T, mcsl::arr_list<T,_bufCapacity>> {
   public:
      using buf_t = arr_list<T,_bufCapacity>;
      using view_buf_t = arr_list<const T,_bufCapacity>;
      using const_it = buf_t::const_it;

      using span = buf_t::span;
      using view = buf_t::view;
   private:
      buf_t& _buf;
      uint _index;
   public:
      it(buf_t& buf, uint index = 0):_buf{buf},_index{index} {}

      it& operator++()    { ++_index; return self; }
      it  operator++(int) { it tmp = self; ++self; return tmp; }
      it& operator--()    { --_index; return self; }
      it  operator--(int) { it tmp = self; --self; return tmp; }
      it& operator+=(const sint i) { _index += i; return self; }
      it& operator-=(const sint i) { _index -= i; return self; }

      it operator+(const sint i) { return it{_buf, _index + i}; }
      it operator-(const sint i) { return it{_buf, _index - i}; }
      T& operator* () { return _buf[_index]; }
      T* operator->() { return (T*)self; }
      T& operator[](const sint i) { return _buf[_index + i]; }
      
      const_it operator+(const sint i) const { return it{_buf, _index + i}; }
      const_it operator-(const sint i) const { return it{_buf, _index - i}; }
      const T& operator* () const { return _buf[_index]; }
      const T* operator->() const { return (T*)self; }
      const T& operator[](const sint i) const { return _buf[_index + i]; }

      sint operator<=>(const_it& other) const { safe_mode_assert(&_buf == &other._buf); return _index - other._index; }
      operator const_it() const { return const_it{_buf, _index}; }
      operator T*() const { return &_buf[_index]; }
      operator uint () const { return _index; }
      uint operator+() const { return _index; }
};

#pragma region span

//!span
template<typename T, uint _bufCapacity> struct mcsl::span<T, mcsl::arr_list<T,_bufCapacity>> {
   public:
      using buf_t = arr_list<T,_bufCapacity>;

      using it = buf_t::it;
      using const_it = buf_t::const_it;
      using view = buf_t::view;
   private:
      buf_t& _buf;
      uint _begin;
      uint _end;
   public:
      span(buf_t& buf): span{buf, buf.size()} {}
      span(buf_t& buf, uint end): span{buf, 0, end} {}
      span(buf_t& buf, uint begin, uint end):_buf{buf},_begin{begin},_end{end} { safe_mode_assert(_begin >= _end && _end <= _buf.size()); }

      uint size() const { return _end - _begin; }

      it operator+(const sint i) { safe_mode_assert(_begin + i < _end); return _buf + (_begin + i); }
      T& operator[](const sint i) { return *(self + i); }
      it begin() { return _buf + _begin; }
      it end() { return _buf + _end; }
      T& front() { return _buf[_begin]; }
      T& back() { return _buf[_end - 1]; }
      
      const_it operator+(const sint i) const { safe_mode_assert(_begin + i < _end); return _buf + (_begin + i); }
      const T& operator[](const sint i) const { return *(self + i); }
      const_it begin() const { return _buf + _begin; }
      const_it end() const { return _buf + _end; }
      const T& front() const { return _buf[_begin]; }
      const T& back() const { return _buf[_end - 1]; }
};

//!view
template<typename T, uint _bufCapacity> struct mcsl::span<T, const mcsl::arr_list<T,_bufCapacity>> {
   public:
      using buf_t = arr_list<T,_bufCapacity>;

      using it = buf_t::it;
      using const_it = buf_t::const_it;
      using view = buf_t::view;
   private:
      buf_t& _buf;
      uint _begin;
      uint _end;
   public:
      span(buf_t& buf): span{buf, buf.size()} {}
      span(buf_t& buf, uint end): span{buf, 0, end} {}
      span(buf_t& buf, uint begin, uint end):_buf{buf},_begin{begin},_end{end} { safe_mode_assert(_begin >= _end && _end <= _buf.size()); }

      uint size() const { return _end - _begin; }
      
      const_it operator+(const sint i) const { safe_mode_assert(_begin + i < _end); return _buf + (_begin + i); }
      const T& operator[](const sint i) const { return *(self + i); }
      const_it begin() const { return _buf + _begin; }
      const_it end() const { return _buf + _end; }
      const T& front() const { return _buf[_begin]; }
      const T& back() const { return _buf[_end - 1]; }
};

#pragma endregion span

#endif //ARR_LIST_HPP