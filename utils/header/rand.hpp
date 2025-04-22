#ifndef MCSL_RAND_HPP
#define MCSL_RAND_HPP

#include "MCSL.hpp"

//max == 0 -> skip modulus step
//!NOTE: maybe replace with a PCG[https://en.wikipedia.org/wiki/Permuted_congruential_generator]
template<mcsl::uint_t T = ulong,
   T _max = MCSL_LCG64_MIN,
   T _min = MCSL_LCG64_MAX,
   T _mult = MCSL_LCG64_MULT,
   T _inc = MCSL_LCG64_INC,
   T _defaultSeed = MCSL_LCG64_SEED
> //requires (_max == 0 || (_min <= _max && _mult <= (_max - _min + 1)))
struct mcsl::lcg_engine {
   private:
      T _seed;
   public:
      constexpr lcg_engine(T seed = _defaultSeed):_seed{seed} {}

      static constexpr T max() { return _max; }
      static constexpr T min() { return _min; }

      constexpr T& seed() { return _seed; }
      constexpr T  seed() const { return _seed; }

      constexpr T getCurr() const { return _seed + _min; }

      constexpr T rand();
      constexpr T operator()() { return rand(); }

      constexpr T skip(uint count);

      constexpr bool operator==(const lcg_engine& other) const { return _seed == other._seed; }
};

namespace mcsl {
   lcg_engine<ulong> rand;
   void srand(const ulong seed) { rand.seed() = seed; }
};


#pragma region inlinesrc

#include "type_traits.hpp"

// template<mcsl::uint_t T, T _max, T _min, T _mult, T _inc, T _defaultSeed>
// constexpr T mcsl::lcg_engine<T,_max,_min,_mult,_inc,_defaultSeed>::getCurr() const {
//    if constexpr (_min) { return _seed + _min; }
//    else { return _seed; }
// }

template<mcsl::uint_t T, T _max, T _min, T _mult, T _inc, T _defaultSeed>
constexpr T mcsl::lcg_engine<T,_max,_min,_mult,_inc,_defaultSeed>::skip(uint count) {
   while (count--) {
      rand();
   }
   return getCurr();
}

template<mcsl::uint_t T, T _max, T _min, T _mult, T _inc, T _defaultSeed>
constexpr T mcsl::lcg_engine<T,_max,_min,_mult,_inc,_defaultSeed>::rand() {
   _seed = _seed * _mult + _inc;
   if constexpr (_max != 0 && (_min != 0 || _max != TYPEMAX<T>())) {
      _seed %= _max - _min + 1;
   }
   return getCurr();
}

#pragma endregion inlinesrc

#endif //MCSL_RAND_HPP