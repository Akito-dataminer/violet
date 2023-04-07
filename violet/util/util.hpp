/*****************************
 * Copyright (c) 2023 Akito-Dataminer
 * This software is distributed under the MIT license.
 *****************************/

#pragma once

#include <utility>

#define STATIC_CONSTEXPR static constexpr
#define STATIC_CONSTINIT static constinit
#define STATIC_CONSTEVAL static consteval

namespace violet {
  using size_type = std::size_t;

  // template<typename T> constexpr bool false_v = false;

namespace util {
  STATIC_CONSTEXPR int const one_byte = 8;
  template<typename T>
  STATIC_CONSTEXPR int const bit_num = sizeof( T ) * violet::util::one_byte;

  template<bool Cond> using if_nullp_c = std::enable_if_t<Cond, std::nullptr_t>;

  template<typename T, std::size_t N>
  constexpr inline std::size_t ArraySize( T const (&)[N] ) noexcept { return N; }
} // util

} // violet
