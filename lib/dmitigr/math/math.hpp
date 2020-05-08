// -*- C++ -*-
// Copyright (C) 2020 Dmitry Igrishin
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//
// Dmitry Igrishin
// dmitigr@gmail.com

#ifndef DMITIGR_MATH_MATH_HPP
#define DMITIGR_MATH_MATH_HPP

#include "dmitigr/math/version.hpp"
#include <dmitigr/base/debug.hpp>

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
