#pragma once
#ifndef MCSL_HALT_HPP
#define MCSL_HALT_HPP

#include "MCSL.hpp"

namespace mcsl {
   [[noreturn]] void exit();
   [[noreturn]] void quick_exit();
   [[noreturn]] void terminate();
   [[noreturn]] void abort();
   [[noreturn]] void unreachable();
};

#endif //MCSL_HALT_HPP