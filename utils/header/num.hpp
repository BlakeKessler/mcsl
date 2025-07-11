#ifndef MCSL_NUM_HPP
#define MCSL_NUM_HPP

#include "MCSL.hpp"

#define NUM(bits) \
struct mcsl::num##bits { \
   union {               \
      uint##bits u;      \
      sint##bits s;      \
      float##bits f;     \
   };                    \
   NumType type;         \
   num##bits(): u{}, type{NumType::null} {} \
   num##bits( uint##bits U): u{U}, type{NumType::UINT} {} \
   num##bits( sint##bits S): s{S}, type{NumType::SINT} {} \
   num##bits(float##bits F): f{F}, type{NumType::REAL} {} \
}

// NUM(8);
// NUM(16);
NUM(32);
NUM(64);
// NUM(128);

#undef NUM

#endif //MCSL_NUM_HPP