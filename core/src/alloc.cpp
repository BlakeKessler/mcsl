#ifndef MCSL_ALLOC_CPP
#define MCSL_ALLOC_CPP

#include "alloc.hpp"
#include "assert.hpp"

#include <cstdlib>
#include <cwchar>
#include <cstring>

//!dynamically allocate a contiguous array large enough to hold itemCount items of type T according to the compilation mode
template<typename T> [[gnu::malloc]] T* mcsl::alloc(const uint itemCount) {
   #if defined DEBUG_MODE
      return mcsl::dalloc<T>(itemCount);
   #elif defined SAFE_MODE
      return mcsl::calloc<T>(itemCount);
   #elif defined FAST_MODE
      return mcsl::malloc<T>(itemCount);
   #else //default
      return mcsl::malloc<T>(itemCount);
   #endif
}
//!dynamically allocate a contiguous array large enough to hold itemCount items of type T (without initialization)
template<typename T> [[gnu::malloc]] T* mcsl::malloc(const uint itemCount) {
   if (!itemCount) { return nullptr; }
   T* ptr;
   if constexpr (is_t<T,void>) {
      ptr = std::malloc(itemCount * sizeof(ubyte));
   } else {
      ptr = static_cast<T*>(std::malloc(itemCount * sizeof(T)));
   }
   assert(ptr);
   return ptr;
}
//!dynamically allocate and null-initialize a contiguous array large enough to hold itemCount items of type T
template<typename T> [[gnu::malloc]] T* mcsl::calloc(const uint itemCount) {
   if (!itemCount) { return nullptr; }
   T* ptr;
   if constexpr (is_t<T,void>) {
      ptr = std::calloc(itemCount, sizeof(ubyte));
   } else {
      ptr = static_cast<T*>(std::calloc(itemCount, sizeof(T)));
   }
   assert(ptr);
   return ptr;
}
//!dynamically allocate and debug-initialize a contiguous array large enough to hold itemCount items of type T
template<typename T> [[gnu::malloc]] T* mcsl::dalloc(const uint itemCount) {
   if (!itemCount) { return nullptr; }
   T* ptr;
   if constexpr (is_t<T,void>) {
      ptr = std::malloc(itemCount * sizeof(ubyte));
   } else {
      ptr = static_cast<T*>(std::malloc(itemCount * sizeof(T)));
   }
   assert(ptr);
   const uint tmp = itemCount * sizeof(T) / sizeof(wchar_t);
   std::wmemset(static_cast<wchar_t*>(ptr), DEBUG_INT, tmp);
   std::memcpy(static_cast<wchar_t*>(ptr)+tmp, &DEBUG_INT, itemCount*sizeof(T) - tmp*sizeof(wchar_t));
   return ptr;
}
//!dynamically resize a contiguous array to be large enough to hold itemCount items of type T
template<typename T> [[gnu::malloc]] T* mcsl::realloc(T* buf, const uint newItemCount) {
   if (!newItemCount) { return nullptr; }
   T* ptr;
   if constexpr (is_t<T,void>) {
      ptr = std::realloc(buf, newItemCount * sizeof(ubyte));
   } else {
      ptr = static_cast<T*>(std::realloc(buf, newItemCount * sizeof(T)));
   }
   assert(ptr);
   return ptr;
}
// //!deallocate dynamically allocated memory
// void mcsl::free(void* ptr) {
//    if(ptr) {
//       std::free(ptr);
//    }
// }

#endif //MCSL_ALLOC_CPP