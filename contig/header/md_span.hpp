#ifndef MCSL_MD_SPAN_HPP
#define MCSL_MD_SPAN_HPP

#include "MCSL.hpp"

template<typename T, uint dim>
class mcsl::md_span<T, dim> : public mcsl::contig_base<T> {
   private:
      T* _buf;
   public:
      constexpr md_span():_buf{} {}
      constexpr md_span(T* buf):_buf{buf} {}
      constexpr md_span(const contig_t<T> auto& other):_buf{other.begin()} { assume(other.size()) >= size(); }

      constexpr static md_span make(T* buf) { return md_span{buf}; }
      constexpr static md_span make(contig_t<T> auto& buf) { return md_span{buf}; }
      constexpr static const md_span make(const T* buf) { return md_span{const_cast<T*>(buf)}; }
      constexpr static const md_span make(const contig_t<T> auto& buf) { return md_span{const_cast<mcsl::remove_const<decltype(buf)>>(buf)}; }

      constexpr uint size() const { return dim; }
      constexpr uint elemCount() const { return dim; }

      constexpr T* const* ptr_to_buf() { return &_buf; }
      constexpr T* data() { return _buf; }
      constexpr T* begin() { return _buf; }

      constexpr T* emplace(uint i, auto... argv) requires valid_ctor<T, decltype(argv)...> {
         return new (_buf + i) T(std::forward<decltype(argv)>(argv)...);
      }
};

template<typename T, uint dim, uint dim2, uint... dims>
class mcsl::md_span<T, dim, dim2, dims...> : public mcsl::contig_base<md_span<T, dim2, dims...>> {
   private:
      T* _buf;
   public:
      constexpr md_span():_buf{} {}
      constexpr md_span(T* buf):_buf{buf} {}
      constexpr md_span(const contig_t<T> auto& other):_buf{other.begin()} { assume(other.size()) >= size(); }

      constexpr static md_span make(T* buf) { return md_span{buf}; }
      constexpr static md_span make(contig_t<T> auto& buf) { return md_span{buf}; }
      constexpr static const md_span make(const T* buf) { return md_span{const_cast<T*>(buf)}; }
      constexpr static const md_span make(const contig_t<T> auto& buf) { return md_span{const_cast<mcsl::remove_const<decltype(buf)>>(buf)}; }

      constexpr uint size() const { return dim; }
      constexpr uint elemCount() const { return dim * dim2 * (dims * ...); }
      
      constexpr T* const* ptr_to_buf() { return &_buf; }
      constexpr T* data() { return _buf; }
      constexpr md_span<T, dim2, dims...> begin() { return _buf; }

      constexpr T* emplace(uint i, uint i2, is_t<uint> auto... is, auto... argv) requires (sizeof...(is) == sizeof...(dims)) && valid_ctor<T, decltype(argv)...> {
         return self[i].emplace(i2, is..., std::forward<decltype(argv)>(argv)...);
      }
};

#endif //MCSL_MD_SPAN_HPP