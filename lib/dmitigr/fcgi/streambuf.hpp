// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or fcgi.hpp

#ifndef DMITIGR_FCGI_STREAMBUF_HPP
#define DMITIGR_FCGI_STREAMBUF_HPP

#include "dmitigr/fcgi/types_fwd.hpp"

#include <streambuf>

namespace dmitigr::fcgi {

/**
 * @brief A FastCGI stream buffer.
 */
class Streambuf : public std::streambuf {
protected:

  /// @name Buffer management and positioning
  /// @{

  /**
   * @par Requires
   * The valid memory area in range of [buffer, buffer + size) and
   * `(buffer && (2048 <= size && size <= 65528))`.
   *
   * @returns `this`.
   *
   * @throws `std::runtime_error`.
   *
   * @par Exception safety guarantee
   * Basic.
   */
  virtual Streambuf* setbuf(char_type* buffer, std::streamsize size) override = 0;

  /**
   * @brief Sends the put area to a FastCGI client if
   * `(pbase() != nullptr && pbase() != pptr())`.
   *
   * @returns Zero on success, or non-zero otherwise.
   *
   * @throws `std::runtime_error`.
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
   * @throws `std::runtime_error`.
   *
   * @par Exception safety guarantee
   * Basic.
   */
  virtual int_type underflow() override = 0;

  /// @}


  /// @name Put area
  /// @{

  /**
   * Consumes the put area. Also consumes `c` if
   * `(traits_type::eq_int_type(c, traits_type::eof()) == false)`.
   *
   * @returns Some value other than `traits_type::eof()` to indicate success,
   * or `traits_type::eof()` to indicate failure.
   *
   * @throws `std::runtime_error`.
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

#ifdef DMITIGR_FCGI_HEADER_ONLY
#include "dmitigr/fcgi/streambuf.cpp"
#endif

#endif  // DMITIGR_FCGI_STREAMBUF_HPP
