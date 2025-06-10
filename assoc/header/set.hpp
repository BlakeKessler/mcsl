#ifndef MCSL_SET_HPP
#define MCSL_SET_HPP

#include "MCSL.hpp"

#include "arr_span.hpp"
#include "arr_list.hpp"
#include "list.hpp"
#include "math.hpp"
#include <bit>

template<typename T, mcsl::hash_t<T> HashFunc, mcsl::cmp_t<T> CmpFunc> class mcsl::set {
   private:
      struct entry {
         T val;
         ulong hash;
      };

      arr_list<list<entry>> _buckets;
      uint _size;
      float _maxLoadFactor;

      void __rehashImpl(uint count);
   public:
      set(uint bucketCount = DEFAULT_HASH_TABLE_BUCKET_COUNT);

      uint size() const { return _size; }

      bool insert(const T& obj);
      bool insert(const arr_span<T&> objs);
      bool emplace(auto... argv) requires valid_ctor<T, decltype(argv)...>;
      bool remove(const T& obj);
      bool remove(const arr_span<T&> obj);
      bool contains(const T& obj) const;

      T* find(const T& obj);
      const T* find(const T& obj) const;

      float load_factor() const { return ((float)_size) / _buckets.size(); }
      float& max_load_factor() { return _maxLoadFactor; }
      float  max_load_factor() const { return _maxLoadFactor; }

      void rehash() { return __rehashImpl(_buckets.size() * 2); }
      void rehash(uint count);
      void reserve(uint count) { rehash((uint)(count / _maxLoadFactor + 0.5)); }
};



#pragma region inlinesrc
#define tplt(ret_t) template<typename T, mcsl::hash_t<T> HashFunc, mcsl::cmp_t<T> CmpFunc> ret_t mcsl::set<T, HashFunc, CmpFunc>

tplt()::set(uint bucketCount):
_buckets(bucketCount), _size(0), _maxLoadFactor(DEFAULT_HASH_TABLE_LOAD_FACTOR) {
   
}

//returns whether an element was inserted
tplt(bool)::insert(const T& obj) {
   ulong hash = HashFunc(obj);
   list<entry>& bucket = _buckets(hash % buckets.size());
   for (auto it = bucket.begin(); it; ++it) {
      if (it->hash == hash && CmpFunc(obj, *it)) { //obj is already in the set
         return false;
      }
   }
   bucket.emplace_back(obj, hash);
   ++_size;
   if (_size > _maxLoadFactor * _buckets.size()) {
      rehash(_buckets.size() << 1);
   }
   return true;
}
//returns whether an element was inserted
tplt(bool)::insert(const arr_span<T&> objs) {
   bool didInsert = false;
   for (const T& obj : objs) {
      didInsert |= insert(obj);
   }
   return didInsert;
}

//returns whether an element was removed
tplt(bool)::remove(const T& obj) {
   ulong hash = HashFunc(obj);
   list<entry>& bucket = _buckets(hash % buckets.size());
   for (auto it = bucket.begin(); it; ++it) {
      if (it->hash == hash && CmpFunc(obj, *it)) { //obj is in the set
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

tplt(T*)::find(const T& obj) {
   ulong hash = HashFunc(obj);
   list<entry>& bucket = _buckets(hash % buckets.size());
   for (auto it = bucket.begin(); it; ++it) {
      if (it->hash == hash && CmpFunc(obj, *it)) { //obj is in the set
         return &(it->val);
      }
   }
   //obj is not in the set
   return nullptr;
}
tplt(const T*)::find(const T& obj) const {
   ulong hash = HashFunc(obj);
   list<entry>& bucket = _buckets(hash % buckets.size());
   for (auto it = bucket.begin(); it; ++it) {
      if (it->hash == hash && CmpFunc(obj, *it)) { //obj is in the set
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

   //rehash
   uint hashMask = (1 << count) - 1;
   while (i--) {
      auto& bucket = _buckets[i];
      for (auto it : bucket) { //check hash for each node
         uint newHash = it->hash & hashMask;
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