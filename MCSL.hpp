#pragma once
#ifndef MCSL_HPP
#define MCSL_HPP

#include "MCSL_DEFINES.hpp"
#include "MCSL_MSG_LOCALIZATION.hpp"
// #include "throw.hpp"
#include "concepts.hpp"

//!TODO: so-called "pessimizing" moves

namespace mcsl {
   template<uint _capacity> struct uint_n;
   template<float_t T> struct Float;

   template<typename first_t, typename second_t = first_t> struct pair;
   // template<typename... Ts> struct tuple;
   struct range;

   template<typename T> struct container_base;

   template<typename T, class container> struct it;
   template<typename T, class container> struct span;

   template<typename T, uint _bufCapacity> class arr_list;

   template<typename T> class list;

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
   class cstr;
   class dyn_str_span;
   template<uint _size> class raw_str;
   template<uint _size> class raw_cstr;
   class str_slice;
   template<uint _size, typename size_t = ubyte> class raw_buf_str;
   const str_slice FMT(const char* buf);

   template<typename T> struct assoc_base;
   template<typename T, hash_t<T> Hash, cmp_t<T> KeyEq> class set;
   template<typename key_t, typename val_t, hash_t<key_t> Hash, cmp_t<key_t> KeyEq> class map;

   template<uint_t T, T _max, T _min, T _mult, T _inc, T _defaultSeed> struct lcg_engine; //linear congruential generator engine

   class Path;
   class C_File;
   class UnbufferedFile;
   class File;
   class Dir;
   struct FmtArgs;
};

#endif //MCSL_HPP