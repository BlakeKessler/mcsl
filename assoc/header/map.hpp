#ifndef MCSL_MAP_HPP
#define MCSL_MAP_HPP

#include "MCSL.hpp"

#include "arr_span.hpp"
#include "arr_list.hpp"
#include "list.hpp"
#include "math.hpp"
#include "throw.hpp"
#include "tuple.hpp"
#include <bit>

template<typename key_t, typename val_t, mcsl::Hasher<key_t> HashFunc, mcsl::Comparator<key_t> CmpFunc> class mcsl::map {
   private:
      struct entry {
         key_t key;
         val_t val;
         ulong hash;
      };

      arr_list<list<entry>> _buckets;
      uint _hashMask;
      uint _size;
      float _maxLoadFactor;

      void __rehashImpl(uint count);
   public:
      map(uint bucketCount = DEFAULT_HASH_TABLE_BUCKET_COUNT);

      uint size() const { return _size; }

      bool insert(const key_t& key, const val_t& val);
      template<typename... key_argv_t, typename... val_argv_t> bool emplace(tuple<key_argv_t...> keyArgs, tuple<val_argv_t...> valArgs) requires valid_ctor<key_t, key_argv_t...> && valid_ctor<val_t, val_argv_t...>;
      bool insert_or_assign(const key_t& key, const val_t& val);
      template<typename... key_argv_t, typename... val_argv_t> bool emplace_or_assign(tuple<key_argv_t...> keyArgs, tuple<val_argv_t...> valArgs) requires valid_ctor<key_t, key_argv_t...> && valid_ctor<val_t, val_argv_t...>;
      val_t& operator[](const key_t& key);
      const val_t& operator[](const key_t& key) const;
      bool remove(const key_t& key);
      bool remove(const arr_span<key_t&> key);
      bool contains(const key_t& key) const { return find(key); }

      val_t* find(const key_t& key);
      const val_t* find(const key_t& key) const;

      float load_factor() const { return ((float)_size) / _buckets.size(); }
      float& max_load_factor() { return _maxLoadFactor; }
      float  max_load_factor() const { return _maxLoadFactor; }

      void rehash() { return __rehashImpl(_buckets.size() * 2); }
      void rehash(uint count);
      void reserve(uint count) { rehash((uint)(count / _maxLoadFactor + 0.5)); }
};



#pragma region inlinesrc
#define tplt(ret_t) template<typename key_t, typename val_t, mcsl::Hasher<key_t> HashFunc, mcsl::Comparator<key_t> CmpFunc> ret_t mcsl::map<key_t, val_t, HashFunc, CmpFunc>

tplt()::map(uint bucketCount):
_buckets(), _size(0), _maxLoadFactor(DEFAULT_HASH_TABLE_LOAD_FACTOR) {
   bucketCount = std::bit_ceil(bucketCount);
   _hashMask = bucketCount - 1;
   while (_buckets.size() < bucketCount) {
      _buckets.emplace_back();
   }
}

//returns whether an element was removed
tplt(bool)::remove(const key_t& key) {
   ulong hash = HashFunc(key);
   list<entry>& bucket = _buckets[hash & _hashMask];
   for (auto it = bucket.begin(); it != bucket.end(); ++it) {
      if (it->hash == hash && CmpFunc(key, *it)) { //obj is in the map
         bucket.erase(it);
         --_size;
         return true;
      }
   }
   //obj is not in the map
   return false;
}
//returns whether an element was removed
tplt(bool)::remove(const arr_span<key_t&> keys) {
   bool didRemove = false;
   for (const key_t& key : keys) {
      didRemove |= remove(key);
   }
   return didRemove;
}

tplt(bool)::insert(const key_t& key, const val_t& val) {
   ulong hash = HashFunc(key);
   list<entry>& bucket = _buckets[hash & _hashMask];
   for (auto it = bucket.begin(); it != bucket.end(); ++it) {
      if (it->hash == hash && CmpFunc(key, it->key)) { //key is already in the set
         return false;
      }
   }
   entry* entryptr = mcsl::malloc<entry>(1);
   entryptr->key = key;
   entryptr->val = val;
   entryptr->hash = hash;
   bucket.UNSAFE_malloc_ptr_push_back(entryptr);
   ++_size;
   if (_size > _maxLoadFactor * _buckets.size()) {
      rehash();
   }
   return true;
}
template<typename key_t, typename val_t, mcsl::Hasher<key_t> HashFunc, mcsl::Comparator<key_t> CmpFunc>
template<typename... key_argv_t, typename... val_argv_t>
bool mcsl::map<key_t, val_t, HashFunc, CmpFunc>::emplace(tuple<key_argv_t...> keyArgs, tuple<val_argv_t...> valArgs)
requires valid_ctor<key_t, key_argv_t...> && valid_ctor<val_t, val_argv_t...> {
   entry* entryptr = mcsl::malloc<entry>(1);
   entryptr->key = from_tuple<key_t>(keyArgs);
   key_t& key = entryptr->key;
   ulong hash = HashFunc(key);
   entryptr->hash = hash;
   list<entry>& bucket = _buckets[hash & _hashMask];
   for (auto it = bucket.begin(); it != bucket.end(); ++it) {
      if (it->hash == hash && CmpFunc(key, it->key)) { //key is already in the set
         return false;
      }
   }
   entryptr->val = from_tuple<val_t>(valArgs);
   bucket.UNSAFE_malloc_ptr_push_back(entryptr);
   ++_size;
   if (_size > _maxLoadFactor * _buckets.size()) {
      rehash();
   }
   return true;
}
tplt(bool)::insert_or_assign(const key_t& key, const val_t& val) {
   ulong hash = HashFunc(key);
   list<entry>& bucket = _buckets[hash & _hashMask];
   for (auto it = bucket.begin(); it != bucket.end(); ++it) {
      if (it->hash == hash && CmpFunc(key, it->key)) { //key is already in the set
         it->val = val;
         return false;
      }
   }
   entry* entryptr = mcsl::malloc<entry>(1);
   entryptr->key = key;
   entryptr->val = val;
   entryptr->hash = hash;
   bucket.UNSAFE_malloc_ptr_push_back(entryptr);
   ++_size;
   if (_size > _maxLoadFactor * _buckets.size()) {
      rehash();
   }
   return true;
}
template<typename key_t, typename val_t, mcsl::Hasher<key_t> HashFunc, mcsl::Comparator<key_t> CmpFunc>
template<typename... key_argv_t, typename... val_argv_t>
bool mcsl::map<key_t, val_t, HashFunc, CmpFunc>::emplace_or_assign(tuple<key_argv_t...> keyArgs, tuple<val_argv_t...> valArgs)
requires valid_ctor<key_t, key_argv_t...> && valid_ctor<val_t, val_argv_t...> {
   entry* entryptr = mcsl::malloc<entry>(1);
   entryptr->key = from_tuple<key_t>(keyArgs);
   key_t& key = entryptr->key;
   ulong hash = HashFunc(key);
   entryptr->hash = hash;
   list<entry>& bucket = _buckets[hash & _hashMask];
   for (auto it = bucket.begin(); it != bucket.end(); ++it) {
      if (it->hash == hash && CmpFunc(key, it->key)) { //key is already in the set
         std::destroy_at(&(it->val));
         it->val = from_tuple(valArgs);
         std::destroy_at(&key);
         mcsl::free(entryptr);
         return false;
      }
   }
   entryptr->val = from_tuple<val_t>(valArgs);
   bucket.UNSAFE_malloc_ptr_push_back(entryptr);
   ++_size;
   if (_size > _maxLoadFactor * _buckets.size()) {
      rehash();
   }
   return true;
}

tplt(val_t&)::operator[](const key_t& key) {
   ulong hash = HashFunc(key);
   list<entry>& bucket = _buckets[hash & _hashMask];
   for (auto it = bucket.begin(); it != bucket.end(); ++it) {
      if (it->hash == hash && CmpFunc(key, it->key)) { //obj is already in the set
         return it->val;
      }
   }
   entry* entryptr = mcsl::calloc<entry>(1);
   entryptr->key = key;
   entryptr->hash = hash;
   bucket.UNSAFE_malloc_ptr_push_back(entryptr);
   ++_size;
   if (_size > _maxLoadFactor * _buckets.size()) {
      rehash();
   }
   return entryptr->val;
}
tplt(const val_t&)::operator[](const key_t& key) const {
   ulong hash = HashFunc(key);
   list<entry>& bucket = _buckets[hash & _hashMask];
   for (auto it = bucket.begin(); it != bucket.end(); ++it) {
      if (it->hash == hash && CmpFunc(key, it->key)) { //obj is already in the set
         return it->val;
      }
   }
   mcsl::__throw(ErrCode::SEGFAULT, FMT("key not in map"));
}

tplt(val_t*)::find(const key_t& key) {
   ulong hash = HashFunc(key);
   list<entry>& bucket = _buckets[hash & _hashMask];
   for (auto it = bucket.begin(); it != bucket.end(); ++it) {
      if (it->hash == hash && CmpFunc(key, *it)) { //obj is in the map
         return &(it->val);
      }
   }
   //obj is not in the map
   return nullptr;
}
tplt(const val_t*)::find(const key_t& key) const {
   ulong hash = HashFunc(key);
   const list<entry>& bucket = _buckets[hash & _hashMask];
   for (auto it = bucket.begin(); it != bucket.end(); ++it) {
      if (it->hash == hash && CmpFunc(key, it->key)) { //obj is in the map
         return &(it->val);
      }
   }
   //obj is not in the map
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

#endif //MCSL_MAP_HPP