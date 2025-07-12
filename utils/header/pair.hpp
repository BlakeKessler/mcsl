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

//default hash implementation for pairs
template<typename first_t, typename second_t> struct mcsl::hash<mcsl::pair<first_t, second_t>> {
   static uint64 operator()(const mcsl::pair<first_t, second_t>& obj) {
      return mcsl::hash_algos::rapid_mix(mcsl::hash<first_t>()(obj.first), mcsl::hash<second_t>()(obj.second));
   }
   static uint64 operator()(const mcsl::pair<first_t, second_t>& obj, uint64 seed) {
      return mcsl::hash_algos::rapid_mix(operator()(obj), seed);
   }
};

#endif //MCSL_PAIR_HPP