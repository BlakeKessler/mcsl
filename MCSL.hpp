#pragma once
#ifndef MCSL_HPP
#define MCSL_HPP

#include "MCSL_DEFINES.hpp"
#include "mcsl_throw.hpp"
#include "concepts.hpp"

namespace mcsl {
   template<uint _capacity> struct uint_n;

   template<typename first_t, typename second_t = first_t> struct pair;

   template<typename T> struct container_base;

   template<typename T> struct contig_base;
   template<typename T, uint _capacity> class buf;
   template<typename T, uint _capacity> class heap_buf;
   template<typename T, uint _size> class static_arr;
   template<typename T> class array;
   template<typename T> class dyn_arr;
   template<typename T> class arr_span;
   template<typename T> class dyn_arr_span;

   template<typename char_t> struct str_base;
   class string;
   class dyn_str_span;
   template<uint _size> class raw_str;
   class raw_str_span;
   template<uint _size, typename size_t = ubyte> class raw_buf_str;

   struct str_hash_func;
   template<typename T> struct hash_func;
}



// //for nameof() functins
// #include "raw_str.hpp"

#endif //MCSL_HPP