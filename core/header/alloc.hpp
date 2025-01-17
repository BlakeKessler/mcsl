#pragma once
#ifndef MCSL_ALLOC_HPP
#define MCSL_ALLOC_HPP

#include "MCSL.hpp"
#include "concepts.hpp"

namespace mcsl {
   template<typename T> [[gnu::malloc]] T* alloc(const uint itemCount);
   template<typename T> [[gnu::malloc]] T* malloc(const uint itemCount);
   template<typename T> [[gnu::malloc]] T* calloc(const uint itemCount);
   template<typename T> [[gnu::malloc]] T* dalloc(const uint itemCount);
   template<typename T> [[gnu::malloc]] T* realloc(T* buf, const uint newItemCount);
   void free(void* ptr);
}

#include "../src/alloc.cpp"

#endif //MCSL_ALLOC_HPP