// -*- C++ -*-
//
// Copyright 2022 Dmitry Igrishin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef DMITIGR_MATH_ALIGNMENT_HPP
#define DMITIGR_MATH_ALIGNMENT_HPP

#include "exceptions.hpp"

namespace dmitigr::math {

/// @returns `true` if `number` is a power of 2.
template<typename T>
constexpr bool is_power_of_two(const T number) noexcept
{
  return (number & (number - 1)) == 0;
}

/**
 * @returns The size of padding.
 *
 * @param value A value for which a padding need to be calculated.
 * @param alignment An aligment to calculated the padding.
 *
 * @par Requires
 * `(size >= 0 && is_power_of_two(alignment))`.
 */
template<typename T>
constexpr auto padding(const T value, const T alignment)
{
  if (!(value >= 0))
    throw Exception{"cannot calculate padding for a negative value"};
  else if (!is_power_of_two(alignment))
    throw Exception{"cannot calculate padding with alignment that is not "
      "power of 2"};
  return (static_cast<T>(0) - value) & static_cast<T>(alignment - 1);
}

/**
 * @returns The value aligned by using `alignment`.
 *
 * @par Requires
 * `(value >= 0 && is_power_of_two(alignment))`.
 */
template<typename T>
constexpr T aligned(const T value, const T alignment)
{
  if (!(value >= 0))
    throw Exception{"cannot align a negative value"};
  else if (!is_power_of_two(alignment))
    throw Exception{"cannot align a value with alignment that is not "
      "power of 2"};
  return (value + (alignment - 1)) & -alignment;
}

} // namespace dmitigr::math

#endif  // DMITIGR_MATH_ALIGNMENT_HPP
