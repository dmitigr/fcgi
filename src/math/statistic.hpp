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

#ifndef DMITIGR_MATH_STATISTIC_HPP
#define DMITIGR_MATH_STATISTIC_HPP

#include "../base/assert.hpp"

namespace dmitigr::math {

/**
 * @returns An average of values.
 *
 * @param data Input data
 */
template<class Container>
constexpr double avg(const Container& data) noexcept
{
  double result{};
  const auto data_size = data.size();
  for (const double num : data)
    result += (num / static_cast<double>(data_size));
  return result;
}

/**
 * @returns A variance of values.
 *
 * @param data Input data.
 * @param avg An average of `data`.
 * @param general Is the `data` represents general population?
 */
template<class Container>
constexpr double variance(const Container& data, const double avg,
  const bool general = true) noexcept
{
  const auto den = static_cast<double>(data.size() - !general);
  double result{};
  for (const double num : data) {
    const double d = num - avg;
    result += (d / den) * d; // (d * d) / den
  }
  DMITIGR_ASSERT(result >= 0);
  return result;
}

/// @overload
template<class Container>
constexpr double variance(const Container& data,
  const bool general = true) noexcept
{
  return variance(data, avg(data), general);
}

} // namespace dmitigr::math

#endif  // DMITIGR_MATH_STATISTIC_HPP
