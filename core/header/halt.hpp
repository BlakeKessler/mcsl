#pragma once
#ifndef MCSL_HALT_HPP
#define MCSL_HALT_HPP

#include "MCSL.hpp"

namespace mcsl {
   [[noreturn]] void exit(sint status);
   [[noreturn]] void quick_exit(sint status);
   [[noreturn]] void terminate();
   [[noreturn]] void abort();
};

#endif //MCSL_HALT_HPP