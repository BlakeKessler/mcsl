#pragma once
#ifndef MCSL_TUPLE_HPP
#define MCSL_TUPLE_HPP

#include "MCSL.hpp"

#include <tuple>

namespace mcsl { //!TODO: actually implement mcsl::tuple properly
   template<typename... Ts> using tuple = std::tuple<Ts...>;
}

#endif //MCSL_TUPLE_HPP