#pragma once
#ifndef MCSL_PAIR_HPP
#define MCSL_PAIR_HPP

#include "MCSL.hpp"
#include "raw_str.hpp"

template<typename first_t, typename second_t> struct [[clang::trivial_abi]] mcsl::pair {
   static constexpr const raw_str _nameof = "pair";
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