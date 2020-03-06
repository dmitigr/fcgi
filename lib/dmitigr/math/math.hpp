// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or math.hpp

#ifndef DMITIGR_MATH_MATH_HPP
#define DMITIGR_MATH_MATH_HPP

#include "dmitigr/util/debug.hpp"

namespace dmitigr::math {

/**
 * @returns `true` if `number` is a power of 2, or `false` otherwise.
 */
template<typename T>
constexpr bool is_power_of_two(const T number)
{
  return (number & (number - 1)) == 0;
}

/**
 * @returns The number to add to `size` to
 * get the aligned value by using `alignment`.
 *
 * @par Requires
 * `is_power_of_two(alignment)`.
 */
template<typename T, typename U>
constexpr T padding(const T size, const U alignment)
{
  DMITIGR_REQUIRE(is_power_of_two(alignment), std::invalid_argument);
  const T a = alignment;
  return (static_cast<T>(0) - size) & (a - 1);
}

/**
 * @return The value of `size` aligned by using `alignment`.
 *
 * @par Requires
 * `is_power_of_two(alignment)`.
 */
template<typename T, typename U>
constexpr T aligned(const T size, const U alignment)
{
  DMITIGR_REQUIRE(is_power_of_two(alignment), std::invalid_argument);
  const T a = alignment;
  return (size + (a - 1)) & -a;
}

} // namespace dmitigr::math

#endif  // DMITIGR_MATH_MATH_HPP
