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

namespace {
   template<uint index, typename... Ts> uint64 __hash(const mcsl::tuple<Ts...>& obj, uint64 val) {
      if constexpr (index < sizeof...(Ts)) {
         return __hash<index + 1, Ts...>(obj, mcsl::hash_algos::rapid_mix(val, std::hash<decltype(std::get<index, mcsl::tuple<Ts...>>(obj))>(std::get<index, mcsl::tuple<Ts...>>(obj))));
      } else {
         return val;
      }
   }
};

template<typename... Ts> struct std::hash<mcsl::tuple<Ts...>> {
   static uint64 operator()(const mcsl::tuple<Ts...>& obj) {
      return __hash<0, Ts...>(obj, 0);
   }
};

#endif //MCSL_TUPLE_HPP