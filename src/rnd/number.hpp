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

#ifndef DMITIGR_RND_NUMBER_HPP
#define DMITIGR_RND_NUMBER_HPP

#include "exceptions.hpp"

#include <chrono>
#include <cstdlib>

namespace dmitigr::rnd {

// -----------------------------------------------------------------------------
// Number generators
// -----------------------------------------------------------------------------

/// Seeds the pseudo-random number generator.
inline void seed_by_now() noexcept
{
  const auto seed = std::chrono::duration_cast<std::chrono::seconds>(
    std::chrono::system_clock::now().time_since_epoch()).count();
  std::srand(static_cast<unsigned>(seed));
}

/**
 * @returns The random number.
 *
 * @remarks From TC++PL 3rd, 22.7.
 */
template<typename T>
constexpr T cpp_pl_3rd(const T maximum) noexcept
{
  const auto rand_num = static_cast<double>(std::rand());
  return static_cast<T>(maximum * (rand_num / RAND_MAX));
}

/**
 * @overload
 *
 * @par Requires
 * `(minimum < maximum)`.
 */
template<typename T>
constexpr T cpp_pl_3rd(const T minimum, const T maximum)
{
  if (!(minimum < maximum))
    throw Exception{"invalid interval for random number generation"};

  const auto range_length = maximum - minimum;
  return (cpp_pl_3rd(maximum) % range_length) + minimum;
}

} // namespace dmitigr::rnd

#endif  // DMITIGR_RND_NUMBER_HPP
