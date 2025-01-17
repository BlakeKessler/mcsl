#pragma once
#ifndef MCSL_CONCEPTS
#define MCSL_CONCEPTS

#include "MCSL.hpp"

#include <concepts>


//!NOTE: FIX CONTAINER CONCEPTS


namespace mcsl {
   #pragma region basic
   template<typename lhs, typename rhs> concept same_t = std::same_as<lhs,rhs>;
   template<typename child_t, typename parent_t> concept is_t = std::derived_from<child_t,parent_t> || same_t<child_t,parent_t>;
   template<typename orig_t, typename target_t> concept castable_to = std::convertible_to<orig_t,target_t>;
   // template<typename lhs, typename rhs> concept priv_is_t = requires {
   //    std::is_base_of_v<rhs,lhs>;
   // };
   template<typename T> concept int_t = std::integral<T>;
   template<typename T> concept uint_t = std::unsigned_integral<T>;
   template<typename T> concept sint_t = std::signed_integral<T>;
   template<typename T> concept float_t = std::floating_point<T>;
   template<typename T> concept num_t = mcsl::int_t<T> || mcsl::float_t<T>;

   template<typename T> concept ptr_t = std::is_pointer<T>::value;
   #pragma endregion basic

   #pragma region containers
   template<typename T> concept contig_container_t = requires(T a) {
      { a.size() } -> int_t;
      { a.data() } -> ptr_t;
      { a.data() } -> same_t<decltype(a.begin())>;
      { a.data() } -> same_t<decltype(a.end()  )>;
      { a[0]     } -> same_t<decltype(*a.data())>;
   };
   template<typename T> concept container_t = requires(T a) {
      { a.begin() } -> same_t<decltype(a.end())>;
   };

   template<typename arr_t> concept contig_t = contig_container_t<arr_t> && arr_t::is_contig;

   template<typename string_t> concept str_t = requires (string_t str) {
      { str[0] } -> same_t<char&>;
   } && contig_t<string_t>;

   template<typename set_t, typename T> concept assoc_t = requires (set_t set, T obj) {
      { set.contains(obj) } -> same_t<bool>;
   } && set_t::is_assoc;
   #pragma endregion containers

   #pragma region utils
   template<typename T, typename func> concept hash_t = requires (T obj, func f) {
      { f(obj) } -> int_t;
   };
   template<typename T, typename func> concept cmp_t = requires (T lhs, T rhs, func f) {
      { f(lhs,rhs) } -> same_t<bool>;
   };

   template<typename func_t, typename ...Args> concept callable_t = requires (func_t f, Args... args) {
      f(args...);
   };
   #pragma endregion utils
};

#endif //MCSL_CONCEPTS