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

#ifndef DMITIGR_RND_STRING_HPP
#define DMITIGR_RND_STRING_HPP

#include "number.hpp"

#include <string>

namespace dmitigr::rnd {

// -----------------------------------------------------------------------------
// Strings generators
// -----------------------------------------------------------------------------

/**
 * @returns The random string.
 *
 * @param size The result size.
 * @param palette The palette of characters the result will consist of.
 */
inline std::string random_string(const std::string& palette,
  const std::string::size_type size)
{
  std::string result;
  result.resize(size);
  if (const auto pallete_size = palette.size()) {
    for (std::string::size_type i{}; i < size; ++i)
      result[i] = palette[cpp_pl_3rd(pallete_size - 1)];
  }
  return result;
}

/**
 * @returns The random string.
 *
 * @param beg The start of source range.
 * @param end The past of end of source range.
 * @param size The result size.
 *
 * @par Requires
 * `(beg <= end)`.
 */
inline std::string random_string(const char beg, const char end,
  const std::string::size_type size)
{
  if (!(beg <= end))
    throw Exception{"invalid character range for random string generation"};

  std::string result;
  if (beg < end) {
    result.resize(size);
    const auto len = end - beg;
    for (std::string::size_type i{}; i < size; ++i)
      result[i] = static_cast<char>((cpp_pl_3rd(end) % len) + beg);
  }
  return result;
}

} // namespace dmitigr::rnd

#endif  // DMITIGR_RND_STRING_HPP
