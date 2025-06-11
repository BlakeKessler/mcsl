#pragma once
#ifndef MCSL_TUPLE_HPP
#define MCSL_TUPLE_HPP

#include "MCSL.hpp"

#include <tuple>

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

#endif //MCSL_TUPLE_HPP