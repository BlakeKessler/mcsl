#ifndef MCSL_SET_HPP
#define MCSL_SET_HPP

#include "MCSL.hpp"

#include "arr_span.hpp"
#include "arr_list.hpp"
#include "list.hpp"
#include "math.hpp"
#include <bit>

template<typename T, mcsl::hash_t<T> Hash = std::hash<T>, mcsl::cmp_t<T> KeyEq = std::equal_to<T>>
class mcsl::set {
   private:
      struct entry {
         T val;
         ulong hash;
      };

      arr_list<list<entry>> _buckets;
      uint _hashMask;
      uint _size;
      float _maxLoadFactor;

      Hash _hash;
      KeyEq _eq;

      template<typename other_t> concept span_compat_t<arr_span<other_t&>> = hash_compat_t<T, Hash, KeyEq><other_t>;

      void __rehashImpl(uint count);
   public:
      set(uint bucketCount = DEFAULT_HASH_TABLE_BUCKET_COUNT, Hash hash = {}, KeyEq keyEq = {});

      uint size() const { return _size; }

      bool insert(const T& obj);
      bool insert(const arr_span<T&> objs);
      bool emplace(auto... argv) requires valid_ctor<T, decltype(argv)...>;
      bool remove(const T& obj);
      bool remove(const arr_span<T&> obj);
      
      bool insert(const hash_compat_t<T, Hash, KeyEq> auto& obj) requires valid_ctor<T, other_t> { return emplace(obj); }
      bool insert(const hash_compat_span_t<T, Hash, KeyEq> auto objs) requires valid_ctor<T, decltype(objs[0])>;
      bool remove(const hash_compat_t<T, Hash, KeyEq> auto& obj);
      bool remove(const hash_compat_span_t<T, Hash, KeyEq> auto objs);

      T* find(const T& obj);
      const T* find(const T& obj) const;
      T* find(const hash_compat_t<T, Hash, KeyEq> auto& obj);
      const T* find(const hash_compat_t<T, Hash, KeyEq> auto& obj) const;

      bool contains(const T& obj) const { return find(obj); }
      bool contains(const hash_compat_t<T, Hash, KeyEq> auto& obj) const { return find(obj); }

      float load_factor() const { return ((float)_size) / _buckets.size(); }
      float& max_load_factor() { return _maxLoadFactor; }
      float  max_load_factor() const { return _maxLoadFactor; }

      void rehash() { return __rehashImpl(_buckets.size() * 2); }
      void rehash(uint count);
      void reserve(uint count) { rehash((uint)(count / _maxLoadFactor + 0.5)); }
};



#pragma region inlinesrc
#define tplt(ret_t) template<typename T, mcsl::hash_t<T> Hash, mcsl::cmp_t<T> KeyEq> ret_t mcsl::set<T, Hash, KeyEq>

tplt()::set(uint bucketCount, Hash hash, KeyEq keyEq):
_buckets(),_size(0),_maxLoadFactor(DEFAULT_HASH_TABLE_LOAD_FACTOR),_hash(hash),_eq(keyEq) {
   bucketCount = std::bit_ceil(bucketCount);
   _hashMask = bucketCount - 1;
   while (_buckets.size() < bucketCount) {
      _buckets.emplace_back();
   }
}

//returns whether an element was inserted
tplt(bool)::insert(const T& obj) {
   ulong hash = _hash(obj);
   list<entry>& bucket = _buckets[hash & _hashMask];
   for (auto it = bucket.begin(); it != bucket.end(); ++it) {
      if (it->hash == hash && _cmp(it->val, obj)) { //obj is already in the set
         return false;
      }
   }
   bucket.emplace_back(obj, hash);
   ++_size;
   if (_size > _maxLoadFactor * _buckets.size()) {
      rehash();
   }
   return true;
}
//returns whether an element was inserted
tplt(bool)::insert(const arr_span<T&> objs) {
   reserve(_size + objs.size());
   bool didInsert = false;
   for (const T& obj : objs) {
      didInsert |= insert(obj);
   }
   return didInsert;
}
//returns whether an element was inserted
tplt(bool)::insert(const hash_compat_span_t<T, Hash, KeyEq> auto objs) requires valid_ctor<T, decltype(objs[0])> {
   reserve(_size + objs.size());
   bool didInsert = false;
   for (const auto& obj : objs) {
      didInsert |= insert(obj);
   }
   return didInsert;
}
//returns whether an element was inserted
tplt(bool)::emplace(auto... argv) requires valid_ctor<T, decltype(argv)...> {
   entry* entryptr = mcsl::malloc<entry>(1);
   T& obj = entryptr->val;
   new (&obj) T (std::forward<decltype(argv)>(argv)...);
   ulong hash = _hash(obj);
   entryptr->hash = hash;
   list<entry>& bucket = _buckets[hash & _hashMask];
   for (auto it = bucket.begin(); it != bucket.end(); ++it) {
      if (it->hash == hash && _cmp(it->val, obj)) { //obj is already in the set
         std::destroy_at(&obj);
         mcsl::free(entryptr);
         return false;
      }
   }
   bucket.UNSAFE_malloc_ptr_push_back(entryptr);
   ++_size;
   if (_size > _maxLoadFactor * _buckets.size()) {
      rehash();
   }
   return true;
}

//returns whether an element was removed
tplt(bool)::remove(const T& obj) {
   ulong hash = _hash(obj);
   list<entry>& bucket = _buckets[hash & _hashMask];
   for (auto it = bucket.begin(); it != bucket.end(); ++it) {
      if (it->hash == hash && _cmp(*it, obj)) { //obj is in the set
         bucket.erase(it);
         --_size;
         return true;
      }
   }
   //obj is not in the set
   return false;
}
//returns whether an element was removed
tplt(bool)::remove(const arr_span<T&> objs) {
   bool didRemove = false;
   for (const T& obj : objs) {
      didRemove |= remove(obj);
   }
   return didRemove;
}
//returns whether an element was removed
tplt(bool)::remove(const hash_compat_t<T, Hash, KeyEq> auto& obj) {
   ulong hash = _hash(obj);
   list<entry>& bucket = _buckets[hash & _hashMask];
   for (auto it = bucket.begin(); it != bucket.end(); ++it) {
      if (it->hash == hash && _cmp(*it, obj)) { //obj is in the set
         bucket.erase(it);
         --_size;
         return true;
      }
   }
   //obj is not in the set
   return false;
}
//returns whether an element was removed
tplt(bool)::remove(const hash_compat_span_t<T, Hash, KeyEq> auto objs) {
   bool didRemove = false;
   for (const auto& obj : objs) {
      didRemove |= remove(obj);
   }
   return didRemove;
}

tplt(T*)::find(const T& obj) {
   ulong hash = _hash(obj);
   list<entry>& bucket = _buckets[hash & _hashMask];
   for (auto it = bucket.begin(); it != bucket.end(); ++it) {
      if (it->hash == hash && _cmp(*it, obj)) { //obj is in the set
         return &(it->val);
      }
   }
   //obj is not in the set
   return nullptr;
}
tplt(const T*)::find(const T& obj) const {
   ulong hash = _hash(obj);
   const list<entry>& bucket = _buckets[hash & _hashMask];
   for (auto it = bucket.begin(); it != bucket.end(); ++it) {
      if (it->hash == hash && _cmp(it->val, obj)) { //obj is in the set
         return &(it->val);
      }
   }
   //obj is not in the set
   return nullptr;
}

tplt(T*)::find(const hash_compat_t<T, Hash, KeyEq> auto& obj) {
   ulong hash = _hash(obj);
   list<entry>& bucket = _buckets[hash & _hashMask];
   for (auto it = bucket.begin(); it != bucket.end(); ++it) {
      if (it->hash == hash && _cmp(*it, obj)) { //obj is in the set
         return &(it->val);
      }
   }
   //obj is not in the set
   return nullptr;
}
tplt(const T*)::find(const hash_compat_t<T, Hash, KeyEq> auto& obj) const {
   ulong hash = _hash(obj);
   const list<entry>& bucket = _buckets[hash & _hashMask];
   for (auto it = bucket.begin(); it != bucket.end(); ++it) {
      if (it->hash == hash && _cmp(it->val, obj)) { //obj is in the set
         return &(it->val);
      }
   }
   //obj is not in the set
   return nullptr;
}

tplt(void)::rehash(uint count) {
   if (!count) { rehash(); }
   count = std::bit_ceil(count);
   //calculate required number of buckets
   if (_size > _maxLoadFactor * _buckets.size()) {
      count = max(count, std::bit_ceil((uint)(_size / _maxLoadFactor + 0.5)));
   }
   if (count <= _buckets.size()) {
      return;
   }
   return __rehashImpl(count);
}
tplt(void)::__rehashImpl(uint count) {
   debug_assert(std::popcount(count) == 1);

   //add new buckets
   uint i = _buckets.size();
   do {
      _buckets.emplace_back();
   } while (_buckets.size() < count);
   _hashMask = count - 1;

   //rehash
   while (i--) {
      mcsl::list<entry>& bucket = _buckets[i];
      for (auto it = bucket.begin(); it != bucket.end(); ++it) { //check hash for each node
         uint newHash = it->hash & _hashMask;
         if (newHash != i) { //move node if necessary
            auto& newBucket = _buckets[newHash];
            newBucket.splice(newBucket.end(), bucket, it);
         }
      }
   };
}


#undef tplt
#pragma endregion inlinesrc

#endif //MCSL_SET_HPP