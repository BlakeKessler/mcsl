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

      struct it {
         private:
            node* ptr;

            void free() {
               std::destroy_at(ptr->objptr);
               mcsl::free(ptr->objptr);
               mcsl::free(ptr);
            }
         public:
            friend class list; //apparently necessary for `~list()` for some reason
            it(node* p):ptr{p} {}
            operator bool() const { return ptr; }

            T& operator*() { assume(ptr && ptr->objptr); return **ptr; }
            T* operator->() { assume(ptr && ptr->objptr); return *ptr; }
            const T& operator*() const { assume(ptr && ptr->objptr); return **ptr; }
            const T* operator->() const { assume(ptr && ptr->objptr); return *ptr; }

            it& operator++() { ptr = ptr->next; return self; }
            it& operator++(int) { it tmp = self; ptr = ptr->next; return tmp; }
            it& operator--() { ptr = ptr->prev; return self; }
            it& operator--(int) { it tmp = self; ptr = ptr->prev; return tmp; }
            it& operator+=(slong n) const {
               if (n > 0) {
                  [[likely]];
                  do { ++self; } while (--n);
               } else if (n < 0) {
                  do { --self; } while (++n);
               }
               return self;
            }
            it& operator-=(slong n) const { return self += (-n); }
            it operator+(slong n) { it tmp = self; tmp += n; return tmp; }
            it operator-(slong n) { it tmp = self; tmp -= n; return tmp; }

            bool operator==(const it other) const { return ptr == other.ptr; }
      };


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
      uint size() const { return _size; }

      T& first() { return *(_begin->objptr); }
      T& last() { return *(_end->prev->objptr); }
      const T& first() const { return *_begin; }
      const T& last() const { return *(_end->prev); }

      it push_back(const T& obj);
      it push_front(const T& obj);
      it emplace_back(auto... argv) requires valid_ctor<T, decltype(argv)...>;
      it emplace_front(auto... argv) requires valid_ctor<T, decltype(argv)...>;

      void pop_back();
      void pop_front();

      it insert(it pos, const T& obj); //insert before pos
      it emplace(it pos, auto... argv) requires valid_ctor<T, decltype(argv)...>; //emplace before pos

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
#define __APPEND(first, second) first->next = second; second->prev = first

template<typename T> mcsl::list<T>::list():
   _end(node::make()),
   _begin(_end),
   _size(0) {
}
template<typename T> mcsl::list<T>::~list() {
   node* i = _begin;
   while (i) { //while instead of do-while to protect against double-deletion
      it tmp = i;
      i = i->next;
      tmp.free();
   }
   
   _begin = nullptr;
   _end = nullptr;
   _size = 0;
}

template<typename T> mcsl::list<T>::it mcsl::list<T>::push_back(const T& obj) {
   ++_size;
   node* ptr = node::make(_end, _end->prev);
   ptr->objptr = mcsl::malloc<T>(1);
   *(ptr->objptr) = obj;
   if (_begin == _end) {
      _begin = ptr;
   }
   return ptr;
}
template<typename T> mcsl::list<T>::it mcsl::list<T>::push_front(const T& obj) {
   ++_size;
   _begin = node::make(_begin, nullptr);
   _begin->objptr = mcsl::malloc<T>(1);
   *(_begin->objptr) = obj;
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
   *(ptr->objptr) = obj;
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
   it tmp = pos->next;
   if (pos->prev) {
      pos->prev->next = pos->next;
   }
   if (pos->next) {
      pos->next->prev = pos->prev;
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
   _end->prev->next = nullptr;
   _end->prev = _begin;
   _begin->prev = _end;
   _begin = oldlast;

   for (node* i = _end->prev; i != _end; i = i->next) {
      node* tmp = i->prev;
      i->prev = i->next;
      i->next = tmp;
   }

   return self;
}
template<typename T> mcsl::list<T>& mcsl::list<T>::unique() {
   it i = _begin;
   while (i != _end) {
      if (*i == *(i->next)) {
         erase(i->next);
      }
      else {
         ++i;
      }
   }
   return self;
}

template<typename T> void mcsl::list<T>::splice(it pos, list& other) {
   node* prev = pos->prev.ptr;
   __APPEND(prev, other._begin);
   __APPEND(other._end->prev, pos);
   _size += other._size;

   other._end->prev = nullptr;
   other._begin = other._end;
   other._size = 0;
}
template<typename T> void mcsl::list<T>::splice(it pos, list&& other) {
   node* prev = pos->prev.ptr;
   __APPEND(prev, other._begin);
   __APPEND(other._end->prev, pos);
   _size += other._size;
   
   it{other._end}.free();
   other._end = nullptr;
   other._begin = nullptr;
   other._size = 0;
}
template<typename T> void mcsl::list<T>::splice(it pos, list& other, it otherPos) {
   node* prev = pos->prev.ptr;
   node* tmp = otherPos->prev;

   __APPEND(prev, otherPos);
   __APPEND(tmp, otherPos->next);
   __APPEND(otherPos, pos);
   
   ++_size;
   --other._size;
}
template<typename T> void mcsl::list<T>::splice(it pos, list& other, it begin, it end) {
   node* prev = pos->prev;
   node* tmp = begin->prev;

   __APPEND(prev, begin);
   __APPEND(end->prev, pos);
   __APPEND(tmp, end);

   for (it i = begin; i != end; ++i) {
      ++_size;
      --other._size;
   }
}

template<typename T> mcsl::list<T>& mcsl::list<T>::sort() {
   auto [f,l] = __sortImpl(_begin, _end->prev, _size);
   f->prev = nullptr;
   _begin = f;
   l->next = _end;
   _end->prev = l;
   
   return self;
}
template<typename T> mcsl::list<T>& mcsl::list<T>::merge(list& other) {
   auto [f,l] = __mergeImpl(_begin, _end->prev, _size, other._begin, other._end->prev, other._size);
   f->prev = nullptr;
   _begin = f;
   l->next = _end;
   _end->prev = l;
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
   l->next = _end;
   _end->prev = l;
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
   l->next = _end;
   _end->prev = l;
   
   return self;
}
template<typename T> template<mcsl::cmp_t<T> comp> mcsl::list<T>& mcsl::list<T>::merge(list& other, comp cmp) {
   auto [f,l] = __mergeImpl(cmp, _begin, _end->prev, _size, other._begin, other._end->prev, other._size);
   f->prev = nullptr;
   _begin = f;
   l->next = _end;
   _end->prev = l;
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
   l->next = _end;
   _end->prev = l;
   _size += other._size;
   
   mcsl::free(other._end);
   other._end = nullptr;
   other._begin = nullptr;
   other._size = 0;

   return self;
}

#pragma region __impl

template<typename T> mcsl::pair<typename mcsl::list<T>::node*> mcsl::list<T>::__sortImpl(node* first, node* last, uint len) {
   assume(len);
   if (len == 1) {
      debug_assert(first == last);
      return {first, last};
   }
   if (len == 2) {
      if (*(first->objptr) < *(last->objptr)) {
         return {first, last};
      } else {
         __APPEND(last, first);
         return {last, first};
      }
   } else {
      uint newlen = len / 2;
      uint newlen2 = len - newlen;
      node* mdpt = (it{first} + newlen).ptr;
      auto [f1, l1] = __sortImpl(first, mdpt, newlen);
      auto [f2, l2] = __sortImpl(mdpt->next, last, newlen2);
      return __mergeImpl(f1, l1, newlen, f2, l2, newlen2);
   }
}
template<typename T> mcsl::pair<typename mcsl::list<T>::node*> mcsl::list<T>::__mergeImpl(node* lhsFirst, node* lhsLast, uint lhsLen, node* rhsFirst, node* rhsLast, uint rhsLen) {
   mcsl::pair<node*> bounds{};
   if (*(lhsFirst->objptr) < *(rhsFirst->objptr)) {
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
         __APPEND(curr, rhsFirst);
         bounds.second = rhsLast;
      } else if (!rhsLen) { //rhs done
         debug_assert(lhsLen);
         __APPEND(curr, lhsFirst);
         bounds.second = lhsLast;
      } else { //neither done
         if (*(lhsFirst->objptr) < *(rhsFirst->objptr)) {
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
   assume(len);
   if (len == 1) {
      debug_assert(first == last);
      return {first, last};
   }
   if (len == 2) {
      if (cmp(*(first->objptr), *(last->objptr))) {
         return {first, last};
      } else {
         __APPEND(last, first);
         return {last, first};
      }
   } else {
      uint newlen = len / 2;
      node* mdpt = (it{first} + newlen).ptr;
      auto [f1, l1] = __sortImpl(cmp, first, mdpt, newlen);
      auto [f2, l2] = __sortImpl(cmp, mdpt->next, last, len - newlen);
      return __mergeImpl(cmp, f1, l1, newlen, f2, l2, len - newlen);
   }
}
template<typename T> template<mcsl::cmp_t<T> comp> mcsl::pair<typename mcsl::list<T>::node*> mcsl::list<T>::__mergeImpl(comp cmp, node* lhsFirst, node* lhsLast, uint lhsLen, node* rhsFirst, node* rhsLast, uint rhsLen) {
   mcsl::pair<node*> bounds{};
   if (cmp(*(lhsFirst->objptr), *(rhsFirst->objptr))) {
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
         __APPEND(curr, rhsFirst);
         bounds.second = rhsLast;
      } else if (!rhsLen) { //rhs done
         debug_assert(lhsLen);
         __APPEND(curr, lhsFirst);
         bounds.second = lhsLast;
      } else { //neither done
         if (cmp(*(lhsFirst->objptr), *(rhsFirst->objptr))) {
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