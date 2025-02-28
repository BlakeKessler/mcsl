#pragma once
#ifndef MCSL_STRING_HPP
#define MCSL_STRING_HPP

#include "MCSL.hpp"
#include "str_base.hpp"
#include "dyn_arr.hpp"
#include "raw_str.hpp"

//!dynamically-sized string
class mcsl::string : public mcsl::dyn_arr<char> {
      static constexpr raw_str _nameof = "string";
   public:
      using dyn_arr<char>::dyn_arr;

      [[gnu::pure]] constexpr static const auto& nameof() { return _nameof; }

      string& operator+=(const str_t auto& other);
      string& operator*=(const uint repeatCount);
};

#endif //MCSL_STRING_HPP