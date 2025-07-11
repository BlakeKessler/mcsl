#pragma once
#ifndef MCSL_PAIR_HPP
#define MCSL_PAIR_HPP

#include "MCSL.hpp"

#include "hash.hpp"

template<typename first_t, typename second_t> struct [[clang::trivial_abi]] mcsl::pair {
   static constexpr const char _nameof[] = "pair";
   static constexpr const auto& nameof() { return _nameof; }


   
   first_t first;
   second_t second;
   
   constexpr pair():first{},second{} {}
   constexpr pair(const first_t& a, const second_t& b):first{a},second{b} {}
};

template<typename first_t, typename second_t> struct std::hash<mcsl::pair<first_t, second_t>> {
   static uint64 operator()(const mcsl::pair<first_t, second_t>& obj) {
      return mcsl::hash_algos::rapid_mix(std::hash<first_t>()(obj.first), std::hash<second_t>()(obj.second));
   }
};

#endif //MCSL_PAIR_HPP