#ifndef MCSL_LIST_HPP
#define MCSL_LIST_HPP

#include "MCSL.hpp"
#include "concepts.hpp"
#include "alloc.hpp"
#include "pair.hpp"

template<typename T> class mcsl::list {
   private:
      struct node {
         T* objptr;
         node* next;
         node* prev;

         T& operator*() { return *objptr; }
         T* operator->() { return objptr; }
         const T& operator*() const { return *objptr; }
         const T* operator->() const { return objptr; }

         static node* make() { return mcsl::calloc<node>(1); }
         static node* make(node* n, node* p) {
            node* ptr = make();
            if (n) { [[likely]];
               ptr->next = n;
               n->prev = ptr;
            }
            if (p) {
               ptr->prev = p;
               p->next = ptr;
            }
            return ptr;
         }
      };

   public:
      struct it {
         private:
            node* ptr;

            void free() {
               std::destroy_at(ptr->objptr);
               mcsl::free(ptr->objptr);
               mcsl::free(ptr);
            }
            void free_end() {
               mcsl::free(ptr);
            }
         public:
            friend class list; //apparently necessary for `~list()` for some reason
            it():ptr{} {}
            it(node* p):ptr{p} {}
            operator bool() const { return ptr; }

            T& operator*() const { assume(ptr && ptr->objptr); return *ptr->objptr; }
            T* operator->() const { assume(ptr && ptr->objptr); return ptr->objptr; }

            it next() const { return ptr->next; }
            it prev() const { return ptr->prev; }
            it& operator++() { ptr = ptr->next; return self; }
            it& operator++(int) { it tmp = self; ptr = ptr->next; return tmp; }
            it& operator--() { ptr = ptr->prev; return self; }
            it& operator--(int) { it tmp = self; ptr = ptr->prev; return tmp; }

            it& operator+=(slong n) {
               if (n > 0) {
                  [[likely]];
                  do { ++self; } while (--n);
               } else if (n < 0) {
                  do { --self; } while (++n);
               }
               return self;
            }
            it& operator-=(slong n) { return self += (-n); }
            it operator+(slong n) const { return it{self} += n; }
            it operator-(slong n) const { return it{self} -= n; }
            
            bool operator==(const it other) const { return ptr == other.ptr; }
      };
      struct const_it {
         private:
            const node* ptr;
         public:
            friend class list; //apparently necessary for `~list()` for some reason
            const_it():ptr{} {}
            const_it(const node* p):ptr{p} {}
            const_it(const it& p):ptr{p.ptr} {}
            operator bool() const { return ptr; }

            const T& operator*() { assume(ptr && ptr->objptr); return *ptr->objptr; }
            const T* operator->() { assume(ptr && ptr->objptr); return ptr->objptr; }
            const T& operator*() const { assume(ptr && ptr->objptr); return *ptr->objptr; }
            const T* operator->() const { assume(ptr && ptr->objptr); return ptr->objptr; }

            const_it next() { return ptr->next; }
            const_it prev() { return ptr->prev; }
            const_it& operator++() { ptr = ptr->next; return self; }
            const_it& operator++(int) { const_it tmp = self; ptr = ptr->next; return tmp; }
            const_it& operator--() { ptr = ptr->prev; return self; }
            const_it& operator--(int) { const_it tmp = self; ptr = ptr->prev; return tmp; }

            const_it& operator+=(slong n) {
               if (n > 0) {
                  [[likely]];
                  do { ++self; } while (--n);
               } else if (n < 0) {
                  do { --self; } while (++n);
               }
               return self;
            }
            const_it& operator-=(slong n) { return self += (-n); }
            const_it operator+(slong n) { const_it tmp = self; tmp += n; return tmp; }
            const_it operator-(slong n) { const_it tmp = self; tmp -= n; return tmp; }
            
            bool operator==(const const_it other) const { return ptr == other.ptr; }
      };

   private:
      node* _end;
      node* _begin;
      uint _size;

      static pair<node*> __sortImpl(node* first, node* last, uint len);
      template<cmp_t<T> comp> static pair<node*> __sortImpl(comp cmp, node* first, node* last, uint len);
      static pair<node*> __mergeImpl(node* lhsFirst, node* lhsLast, uint lhsLen, node* rhsFirst, node* rhsLast, uint rhsLen);
      template<cmp_t<T> comp> static pair<node*> __mergeImpl(comp cmp, node* lhsFirst, node* lhsLast, uint lhsLen, node* rhsFirst, node* rhsLast, uint rhsLen);
   public:
      list();
      ~list();

      it begin() { return _begin; }
      it end() { return _end; }
      const_it begin() const { return _begin; }
      const_it end() const { return _end; }
      uint size() const { return _size; }

      T& first() { return *_begin->objptr; }
      T& last() { return *_end->prev->objptr; }
      const T& first() const { return *_begin; }
      const T& last() const { return *_end->prev; }

      it push_back(const T& obj);
      it push_front(const T& obj);
      it emplace_back(auto... argv) requires valid_ctor<T, decltype(argv)...>;
      it emplace_front(auto... argv) requires valid_ctor<T, decltype(argv)...>;
      it UNSAFE_malloc_ptr_push_back(T* objptr);
      it UNSAFE_malloc_ptr_push_front(T* objptr);

      void pop_back();
      void pop_front();

      it insert(it pos, const T& obj); //insert before pos
      it emplace(it pos, auto... argv) requires valid_ctor<T, decltype(argv)...>; //emplace before pos
      it UNSAFE_malloc_ptr_insert(it pos, const T* objptr); //insert before pos

      it erase(it pos);
      it erase(it begin, it end);

      list& reverse();
      list& unique();
      void splice(it pos, list& other);
      void splice(it pos, list&& other);
      void splice(it pos, list& other, it otherPos);
      void splice(it pos, list& other, it begin, it end);


      list& sort();
      list& merge(list& other);
      list& merge(list&& other);
      template<cmp_t<T> comp> list& sort(comp cmp);
      template<cmp_t<T> comp> list& merge(list& other, comp cmp);
      template<cmp_t<T> comp> list& merge(list&& other, comp cmp);
};



#pragma region inlinesrc
#define __APPEND(first, second) debug_assert(first); first->next = second; second->prev = first
#define __SAFE_APPEND(first, second) if (first) { first->next = second; } second->prev = first

template<typename T> mcsl::list<T>::list():
   _end(node::make()),
   _begin(_end),
   _size(0) {
}
template<typename T> mcsl::list<T>::~list() {
   node* i = _begin;
   while (i != _end) { //while instead of do-while to protect against double-deletion
      it tmp = i;
      i = i->next;
      tmp.free();
   }
   it{_end}.free_end();
   
   _begin = nullptr;
   _end = nullptr;
   _size = 0;
}

template<typename T> mcsl::list<T>::it mcsl::list<T>::push_back(const T& obj) {
   ++_size;
   node* ptr = node::make(_end, _end->prev);
   ptr->objptr = mcsl::malloc<T>(1);
   *ptr->objptr = obj;
   if (_begin == _end) {
      _begin = ptr;
   }
   return ptr;
}
template<typename T> mcsl::list<T>::it mcsl::list<T>::push_front(const T& obj) {
   ++_size;
   _begin = node::make(_begin, nullptr);
   _begin->objptr = mcsl::malloc<T>(1);
   *_begin->objptr = obj;
   return _begin;
}
template<typename T> mcsl::list<T>::it mcsl::list<T>::emplace_back(auto... argv) requires valid_ctor<T, decltype(argv)...> {
   ++_size;
   node* ptr = node::make(_end, _end->prev);
   ptr->objptr = mcsl::malloc<T>(1);
   new (ptr->objptr) T(std::forward<decltype(argv)>(argv)...);
   if (_begin == _end) {
      _begin = ptr;
   }
   return ptr;
}
template<typename T> mcsl::list<T>::it mcsl::list<T>::emplace_front(auto... argv) requires valid_ctor<T, decltype(argv)...> {
   ++_size;
   _begin = node::make(_begin, nullptr);
   _begin->objptr = mcsl::malloc<T>(1);
   new (_begin->objptr) T(std::forward<decltype(argv)>(argv)...);
   return _begin;
}

template<typename T> mcsl::list<T>::it mcsl::list<T>::UNSAFE_malloc_ptr_push_back(T* objptr) {
   ++_size;
   node* ptr = node::make(_end, _end->prev);
   ptr->objptr = objptr;
   if (_begin == _end) {
      _begin = ptr;
   }
   return ptr;
}
template<typename T> mcsl::list<T>::it mcsl::list<T>::UNSAFE_malloc_ptr_push_front(T* objptr) {
   ++_size;
   _begin = node::make(_begin, nullptr);
   _begin->objptr = objptr;
   return _begin;
}

template<typename T> void mcsl::list<T>::pop_back() {
   assume(_size);
   --_size;
   it ptr = _end->prev;
   _end->prev = ptr->prev;
   if (ptr->prev) {
      ptr->prev->next = _end;
   }
   ptr.free();
}
template<typename T> void mcsl::list<T>::pop_front() {
   assume(_size);
   --_size;
   node* ptr = _begin;
   _begin = ptr->next;
   _begin->prev = nullptr;
   ptr->free();
}

template<typename T> mcsl::list<T>::it mcsl::list<T>::insert(it pos, const T& obj) {
   ++_size;
   node* ptr = node::make(pos.ptr);
   ptr->objptr = mcsl::malloc<T>(1);
   *ptr->objptr = obj;
   if (pos.ptr == _begin) {
      _begin = ptr;
   }
}
template<typename T> mcsl::list<T>::it mcsl::list<T>::UNSAFE_malloc_ptr_insert(it pos, const T* objptr) {
   ++_size;
   node* ptr = node::make(pos.ptr);
   ptr->objptr = objptr;
   if (pos.ptr == _begin) {
      _begin = ptr;
   }
}
template<typename T> mcsl::list<T>::it mcsl::list<T>::emplace(it pos, auto... argv) requires valid_ctor<T, decltype(argv)...> {
   ++_size;
   node* ptr = node::make(pos.ptr);
   ptr->objptr = mcsl::malloc<T>(1);
   new (ptr->objptr) T(std::forward<decltype(argv)>(argv)...);
   if (pos.ptr == _begin) {
      _begin = ptr;
   }
   return ptr;
}

template<typename T> mcsl::list<T>::it mcsl::list<T>::erase(it pos) {
   assume(pos);
   it tmp = pos.next();
   if (pos.ptr->prev) {
      pos.ptr->prev->next = pos.ptr->next;
   }
   if (pos.ptr->next) {
      pos.ptr->next->prev = pos.ptr->prev;
   }
   pos.free();
   --_size;
   return tmp;
}
template<typename T> mcsl::list<T>::it mcsl::list<T>::erase(it begin, it end) {
   assume(begin);
   assume(end);

   //remove [begin, end) from the list
   if (begin->prev) {
      begin->prev->next = end;
   }
   end->prev = begin->prev;

   //free [begin, end)
   while (begin != end) {
      assume(end != _end);
      begin++.free();
      --_size;
   }

   return begin;
}

template<typename T> mcsl::list<T>& mcsl::list<T>::reverse() {
   if (_size <= 1) { return self; }

   node* oldlast = _end->prev;

   for (node* i = _end->prev; i; i = i->next) {
      debug_assert(i);
      node* tmp = i->prev;
      i->prev = i->next;
      i->next = tmp;
   }

   __APPEND(_begin, _end);
   oldlast->prev = nullptr;
   _begin = oldlast;

   return self;
}
template<typename T> mcsl::list<T>& mcsl::list<T>::unique() {
   it i = _begin;
   it next = i.next();
   while (next != _end) {
      if (*i == *(next)) { //remove duplicate (invalidates next)
         erase(next);
      }
      else { //advance
         i = next;
      }
      next = i.next(); //update next
   }
   return self;
}

template<typename T> void mcsl::list<T>::splice(it pos, list& other) {
   node* prev = pos.ptr->prev;
   __APPEND(prev, other._begin);
   __APPEND(other._end->prev, pos.ptr);
   _size += other._size;

   other._end->prev = nullptr;
   other._begin = other._end;
   other._size = 0;
}
template<typename T> void mcsl::list<T>::splice(it pos, list&& other) {
   node* prev = pos.ptr->prev;
   __APPEND(prev, other._begin);
   __APPEND(other._end->prev, pos.ptr);
   _size += other._size;
   
   it{other._end}.free();
   other._end = nullptr;
   other._begin = nullptr;
   other._size = 0;
}
template<typename T> void mcsl::list<T>::splice(it pos, list& other, it otherPos) {
   assume(otherPos != other.end());
   if (otherPos == other.begin()) {
      other._begin = otherPos.ptr->next;
   }
   if (pos == begin()) {
      _begin = otherPos.ptr;
   }
   node* prev = pos.ptr->prev;
   node* tmp = otherPos.ptr->prev;

   __SAFE_APPEND(prev, otherPos.ptr);
   __SAFE_APPEND(tmp, otherPos.ptr->next);
   __APPEND(otherPos.ptr, pos.ptr);
   
   ++_size;
   --other._size;
}
template<typename T> void mcsl::list<T>::splice(it pos, list& other, it begin, it end) {
   node* prev = pos.ptr->prev;
   node* tmp = begin.ptr->prev;

   __APPEND(prev, begin.ptr);
   __APPEND(end.ptr->prev, pos.ptr);
   __APPEND(tmp, end.ptr);

   for (it i = begin; i != end; ++i) {
      ++_size;
      --other._size;
   }
}

template<typename T> mcsl::list<T>& mcsl::list<T>::sort() {
   if (_size <= 1) { return self; }
   
   auto [f,l] = __sortImpl(_begin, _end->prev, _size);
   f->prev = nullptr;
   _begin = f;
   __APPEND(l, _end);

   return self;
}
template<typename T> mcsl::list<T>& mcsl::list<T>::merge(list& other) {
   auto [f,l] = __mergeImpl(_begin, _end->prev, _size, other._begin, other._end->prev, other._size);
   f->prev = nullptr;
   _begin = f;
   __APPEND(l, _end);
   _size += other._size;
   
   other._begin = other._end;
   other._end->next = nullptr;
   other._end->prev = nullptr;
   other._size = 0;

   return self;
}
template<typename T> mcsl::list<T>& mcsl::list<T>::merge(list&& other) {
   auto [f,l] = __mergeImpl(_begin, _end->prev, _size, other._begin, other._end->prev, other._size);
   f->prev = nullptr;
   _begin = f;
   __APPEND(l, _end);
   _size += other._size;
   
   mcsl::free(other._end);
   other._end = nullptr;
   other._begin = nullptr;
   other._size = 0;

   return self;
}
template<typename T> template<mcsl::cmp_t<T> comp> mcsl::list<T>& mcsl::list<T>::sort(comp cmp) {
   auto [f,l] = __sortImpl(cmp, _begin, _end->prev, _size);
   f->prev = nullptr;
   _begin = f;
   __APPEND(l, _end);
   
   return self;
}
template<typename T> template<mcsl::cmp_t<T> comp> mcsl::list<T>& mcsl::list<T>::merge(list& other, comp cmp) {
   auto [f,l] = __mergeImpl(cmp, _begin, _end->prev, _size, other._begin, other._end->prev, other._size);
   f->prev = nullptr;
   _begin = f;
   __APPEND(l, _end);
   _size += other._size;
   
   other._begin = other._end;
   other._end->next = nullptr;
   other._end->prev = nullptr;
   other._size = 0;

   return self;
}
template<typename T> template<mcsl::cmp_t<T> comp> mcsl::list<T>& mcsl::list<T>::merge(list&& other, comp cmp) {
   auto [f,l] = __mergeImpl(cmp, _begin, _end->prev, _size, other._begin, other._end->prev, other._size);
   f->prev = nullptr;
   _begin = f;
   __APPEND(l, _end);
   _size += other._size;
   
   mcsl::free(other._end);
   other._end = nullptr;
   other._begin = nullptr;
   other._size = 0;

   return self;
}

#pragma region __impl

template<typename T> mcsl::pair<typename mcsl::list<T>::node*> mcsl::list<T>::__sortImpl(node* first, node* last, uint len) {
   debug_assert(len);
   if (len == 1) {
      debug_assert(first == last);
      return {first, first};
   }
   else {
      uint len1 = len / 2;
      uint len2 = len - len1;
      node* mdpt1 = (it{first} + (slong)(len1 - 1)).ptr;
      node* mdpt2 = mdpt1->next;
      auto [f1, l1] = __sortImpl(first, mdpt1, len1);
      auto [f2, l2] = __sortImpl(mdpt2, last, len2);
      return __mergeImpl(f1, l1, len1, f2, l2, len2);
   }
}
template<typename T> mcsl::pair<typename mcsl::list<T>::node*> mcsl::list<T>::__mergeImpl(node* lhsFirst, node* lhsLast, uint lhsLen, node* rhsFirst, node* rhsLast, uint rhsLen) {
   mcsl::pair<node*> bounds{};
   if (*lhsFirst->objptr < *rhsFirst->objptr) {
      bounds.first = lhsFirst;
      lhsFirst = lhsFirst->next;
      --lhsLen;
   } else {
      bounds.first = rhsFirst;
      rhsFirst = rhsFirst->next;
      --rhsLen;
   }

   node* curr = bounds.first;
   LOOP_BEGIN: {
      debug_assert(lhsFirst);
      debug_assert(rhsFirst);
      if (!lhsLen) { //lhs done
         debug_assert(rhsLen);
         __APPEND(curr, rhsFirst);
         bounds.second = rhsLast;
      } else if (!rhsLen) { //rhs done
         debug_assert(lhsLen);
         __APPEND(curr, lhsFirst);
         bounds.second = lhsLast;
      } else { //neither done
         if (*lhsFirst->objptr < *rhsFirst->objptr) {
            __APPEND(curr, lhsFirst);
            curr = lhsFirst;
            lhsFirst = lhsFirst->next;
            --lhsLen;
         } else {
            __APPEND(curr, rhsFirst);
            curr = rhsFirst;
            rhsFirst = rhsFirst->next;
            --rhsLen;
         }
         goto LOOP_BEGIN;
      }
   }
   return bounds;
}

template<typename T> template<mcsl::cmp_t<T> comp> mcsl::pair<typename mcsl::list<T>::node*> mcsl::list<T>::__sortImpl(comp cmp, node* first, node* last, uint len) {
   debug_assert(len);
   if (len == 1) {
      debug_assert(first == last);
      return {first, first};
   }
   else {
      uint len1 = len / 2;
      uint len2 = len - len1;
      node* mdpt1 = (it{first} + (slong)(len1 - 1)).ptr;
      node* mdpt2 = mdpt1->next;
      auto [f1, l1] = __sortImpl(cmp, first, mdpt1, len1);
      auto [f2, l2] = __sortImpl(cmp, mdpt2, last, len2);
      return __mergeImpl(cmp, f1, l1, len1, f2, l2, len2);
   }
}
template<typename T> template<mcsl::cmp_t<T> comp> mcsl::pair<typename mcsl::list<T>::node*> mcsl::list<T>::__mergeImpl(comp cmp, node* lhsFirst, node* lhsLast, uint lhsLen, node* rhsFirst, node* rhsLast, uint rhsLen) {
   mcsl::pair<node*> bounds{};
   if (cmp(*lhsFirst->objptr, *rhsFirst->objptr)) {
      bounds.first = lhsFirst;
      lhsFirst = lhsFirst->next;
      --lhsLen;
   } else {
      bounds.first = rhsFirst;
      rhsFirst = rhsFirst->next;
      --rhsLen;
   }
   
   node* curr = bounds.first;
   LOOP_BEGIN: {
      if (!lhsLen) { //lhs done
         debug_assert(rhsLen);
         __APPEND(curr, rhsFirst);
         bounds.second = rhsLast;
      } else if (!rhsLen) { //rhs done
         debug_assert(lhsLen);
         __APPEND(curr, lhsFirst);
         bounds.second = lhsLast;
      } else { //neither done
         if (cmp(*lhsFirst->objptr, *rhsFirst->objptr)) {
            __APPEND(curr, lhsFirst);
            curr = lhsFirst;
            lhsFirst = lhsFirst->next;
            --lhsLen;
         } else {
            __APPEND(curr, rhsFirst);
            curr = rhsFirst;
            rhsFirst = rhsFirst->next;
            --rhsLen;
         }
         goto LOOP_BEGIN;
      }
   }
   return bounds;
}

#pragma endregion __impl

#undef __APPEND

#pragma endregion inlinesrc

#endif //MCSL_LIST_HPP