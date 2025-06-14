#pragma once
#ifndef MCSL_PAIR_HPP
#define MCSL_PAIR_HPP

#include "MCSL.hpp"

template<typename first_t, typename second_t> struct [[clang::trivial_abi]] mcsl::pair {
   static constexpr const char _nameof[] = "pair";
   static constexpr const auto& nameof() { return _nameof; }


   
   first_t first;
   second_t second;
   
   constexpr pair():first{},second{} {}
   constexpr pair(const first_t& a, const second_t& b):first{a},second{b} {}
};

// namespace mcsl {
//    // template<typename first_t, typename second_t> mcsl::pair(first_t,second_t) -> mcsl::pair<first_t,second_t>;
// }

#endif //MCSL_PAIR_HPP

/*
#pragma once
#ifndef MCSL_PAIR_HPP
#define MCSL_PAIR_HPP

#include "MCSL.hpp"

template<mcsl::diff_t<void> first_t, mcsl::diff_t<void> second_t>
struct mcsl::pair<first_t, second_t> {
   static constexpr const char _nameof[] = "pair";
   static constexpr const auto& nameof() { return _nameof; }


   
   first_t first;
   second_t second;
   
   constexpr pair():first{},second{} {}
   constexpr pair(const first_t& a, const second_t& b):first{a},second{b} {}
};

template<mcsl::diff_t<void> first_t>
struct mcsl::pair<first_t, void> {
   static constexpr const char _nameof[] = "pair";
   static constexpr const auto& nameof() { return _nameof; }


   
   first_t first;
   
   constexpr pair():first{} {}
   constexpr pair(const first_t& a):first{a} {}
};

template<mcsl::diff_t<void> second_t>
struct mcsl::pair<void, second_t> {
   static constexpr const char _nameof[] = "pair";
   static constexpr const auto& nameof() { return _nameof; }


   
   second_t second;
   
   constexpr pair():second{} {}
   constexpr pair(const second_t& b):second{b} {}
};

namespace mcsl {
   template<diff_t<void> first_t, diff_t<void> second_t> mcsl::pair(first_t,second_t) -> mcsl::pair<first_t,second_t>;
   template<diff_t<void> first_t> mcsl::pair(first_t) -> mcsl::pair<first_t,void>;
}

#endif //MCSL_PAIR_HPP
*/