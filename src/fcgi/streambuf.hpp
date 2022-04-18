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

#ifndef DMITIGR_FCGI_STREAMBUF_HPP
#define DMITIGR_FCGI_STREAMBUF_HPP

#include "types_fwd.hpp"

#include <streambuf>

namespace dmitigr::fcgi {

/// A FastCGI stream buffer.
class Streambuf : public std::streambuf {
protected:

  /// @name Buffer management and positioning
  /// @{

  /**
   * @returns `this`.
   *
   * @par Requires
   * The valid memory area in range of [buffer, buffer + size) and
   * `buffer && (2048 <= size && size <= 65528)`.
   *
   * @throws Exception.
   *
   * @par Exception safety guarantee
   * Basic.
   */
  virtual Streambuf* setbuf(char_type* buffer, std::streamsize size) override = 0;

  /**
   * @brief Sends the put area to a FastCGI client if
   * `pbase() && pbase() != pptr()`.
   *
   * @returns `0` on success.
   *
   * @throws Exception.
   *
   * @par Exception safety guarantee
   * Basic.
   */
  virtual int sync() override = 0;

  /// @}

  /// @name Get area
  /// @{

  /**
   * @brief Fills the get area if `(eback() != nullptr)`.
   *
   * @returns `traits_type::to_int_type(c)`, where `c` is the first character of
   * the pending sequence, without moving the input sequence position past it,
   * or `traits_type::eof()` to indicate failure if the pending sequence is null.
   *
   * @throws Exception.
   *
   * @par Exception safety guarantee
   * Basic.
   */
  virtual int_type underflow() override = 0;

  /// @}


  /// @name Put area
  /// @{

  /**
   * @brief Consumes the put area. Also consumes `c` if
   * `traits_type::eq_int_type(c, traits_type::eof()) == false`.
   *
   * @returns Some value other than `traits_type::eof()` to indicate success,
   * or `traits_type::eof()` to indicate failure.
   *
   * @throws Exception.
   *
   * @par Exception safety guarantee
   * Basic.
   */
  virtual int_type overflow(int_type c = traits_type::eof()) override = 0;

  /// @}

private:
  friend detail::iStreambuf;

  Streambuf() = default;
};

} // namespace dmitigr::fcgi

#ifndef DMITIGR_FCGI_NOT_HEADER_ONLY
#include "streambuf.cpp"
#endif

#endif  // DMITIGR_FCGI_STREAMBUF_HPP
