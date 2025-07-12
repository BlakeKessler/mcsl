#pragma once
#ifndef MCSL_TUPLE_HPP
#define MCSL_TUPLE_HPP

#include "MCSL.hpp"

#include <tuple>
#include "hash.hpp"
#include <functional>

#pragma region inlinesrc
#pragma GCC diagnostic push
#pragma GCC diagnostic warning "-Wc++11-narrowing"

namespace mcsl { //!TODO: actually implement mcsl::tuple properly
   template<typename... Ts> using tuple = std::tuple<Ts...>;

   namespace {
      template<typename T, ulong... Seq, typename... Ts>
      constexpr T __fromTupleImpl(std::index_sequence<Seq...>, const mcsl::tuple<Ts...>& tuple) {
         return {std::get<Seq>(tuple)...};
      }
   }
   template<typename T, typename... Ts> T from_tuple(const mcsl::tuple<Ts...>& tuple) {
      return __fromTupleImpl<T>(std::make_index_sequence<sizeof...(Ts)>(), tuple);
   }
   template<typename... Ts> constexpr tuple<Ts...> make_tuple(Ts&&... argv) { return std::make_tuple(std::forward<Ts>(argv)...); }
}
#pragma GCC diagnostic pop

//default hash implementation for tuples
namespace {
   template<uint index, typename... Ts> uint64 __hash(const mcsl::tuple<Ts...>& obj, uint64 val) {
      if constexpr (index < sizeof...(Ts)) {
         const auto& elem = std::get<index, decltype(obj)>(obj);
         return mcsl::hash<decltype(elem)>::operator()(__hash<index + 1, Ts...>(elem), val);
      } else {
         return val;
      }
   }
};
template<typename... Ts> struct mcsl::hash<mcsl::tuple<Ts...>> {
   static uint64 operator()(const mcsl::tuple<Ts...>& obj) {
      return operator()(obj, 0);
   }
   static uint64 operator()(const mcsl::tuple<Ts...>& obj, uint64 seed) {
      return __hash<0, Ts...>(obj, seed);
   }
};

#endif //MCSL_TUPLE_HPP