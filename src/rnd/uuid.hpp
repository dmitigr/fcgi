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

#ifndef DMITIGR_RND_UUID_HPP
#define DMITIGR_RND_UUID_HPP

#include "../base/assert.hpp"
#include "number.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <string>

namespace dmitigr::rnd {

/// An UUID.
class Uuid final {
public:
  /// Constructs Nil UUID.
  Uuid() = default;

  /**
   * @returns The random UUID (version 4).
   *
   * @remarks Please be sure to seed the pseudo-random number generator with
   * `std::srand()` (or use convenient `dmitigr::rnd::seed_by_now()`) before
   * calling this maker function.
   */
  static Uuid make_v4()
  {
    Uuid result;

    // Filling the data with random bytes.
    {
      constexpr auto minimum = static_cast<unsigned char>(1);
      constexpr auto maximum = std::numeric_limits<unsigned char>::max();
      for (std::size_t i{}; i < sizeof(result.data_.raw_); ++i)
        result.data_.raw_[i] = cpp_pl_3rd(minimum, maximum);
    }

    /*
     * Setting magic numbers for the "version 4" (pseudorandom) UUID.
     * See http://tools.ietf.org/html/rfc4122#section-4.4
     */
    result.data_.rep_.time_hi_and_version_ =
      (result.data_.rep_.time_hi_and_version_ & 0x0fff) | 0x4000;
    result.data_.rep_.clock_seq_hi_and_reserved_ =
      (result.data_.rep_.clock_seq_hi_and_reserved_ & 0x3f) | 0x80;

    DMITIGR_ASSERT(result.is_invariant_ok());
    return result;
  }

  /// @returns The string representation of the UUID.
  std::string to_string() const
  {
    constexpr std::size_t buf_size{36};
    char buf[buf_size + 1];
    const int count = std::snprintf(buf, sizeof(buf),
      "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
      data_.rep_.time_low_,
      data_.rep_.time_mid_,
      data_.rep_.time_hi_and_version_,
      data_.rep_.clock_seq_hi_and_reserved_,
      data_.rep_.clock_seq_low_,
      data_.rep_.node_[0],
      data_.rep_.node_[1],
      data_.rep_.node_[2],
      data_.rep_.node_[3],
      data_.rep_.node_[4],
      data_.rep_.node_[5]);
    DMITIGR_ASSERT(count == buf_size);
    return std::string{buf, buf_size};
  }

private:
  union {
    struct {
      std::uint32_t time_low_;
      std::uint16_t time_mid_;
      std::uint16_t time_hi_and_version_;
      std::uint8_t clock_seq_hi_and_reserved_;
      std::uint8_t clock_seq_low_;
      std::uint8_t node_[6];
    } rep_;
    unsigned char raw_[16]{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  } data_;

  bool is_invariant_ok() const noexcept
  {
    return std::any_of(std::cbegin(data_.raw_), std::cend(data_.raw_),
      [](const auto b) { return b != 0; });
  }
};

} // namespace dmitigr::rnd

#endif  // DMITIGR_RND_UUID_HPP
