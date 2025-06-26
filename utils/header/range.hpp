#ifndef MCSL_RANGE_HPP
#define MCSL_RANGE_HPP

#include "MCSL.hpp"

#include "assert.hpp"

struct mcsl::range {
   private:
      uint _begin;
      uint _end;
   public:
      range(): range(0, 0) {}
      range(uint end): range(0, end) {}
      range(uint begin, uint end): _begin(begin), _end(end) { debug_assert(end >= begin); }

      uint begin() const { return _begin; }
      uint end() const { return _end; }

      uint size() const { return _end - _begin; }
};

#endif //MCSL_RANGE_HPP