// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or fcgi.hpp

#ifndef DMITIGR_FCGI_STREAMS_HPP
#define DMITIGR_FCGI_STREAMS_HPP

#include "dmitigr/fcgi/dll.hpp"
#include "dmitigr/fcgi/types_fwd.hpp"

#include <istream>
#include <ostream>

namespace dmitigr::fcgi {

/**
 * @brief Represents an abstraction of a data stream.
 */
class Stream {
public:
  /**
   * @returns The controlled Streambuf instance.
   */
  virtual Streambuf* streambuf() const = 0;

  /**
   * @returns `true` if the stream is closed (i.e. unusable anymore), or
   * `false` otherwise.
   */
  virtual bool is_closed() const = 0;

  /**
   * @returns The type of stream. The value of Stream_type::data is returned
   * when the stream is switched to transmit the data file input to filter it
   * out (as prescribed for the role of Filter).
   *
   * @remarks Since the data file input follows the content and `eof() == true`
   * right after all of content of type Stream_type::in is read, the stream
   * error state flags must be cleared before reading the data file input.
   *
   * See also clear().
   */
  virtual Stream_type stream_type() const = 0;

private:
  friend Istream;
  friend Ostream;

  Stream() = default;
};

/**
 * @brief Represents an abstraction of an input data stream.
 */
class Istream : public Stream, public std::istream {
private:
  friend iIstream;

  using std::istream::istream;
};

/**
 * @brief Represents an abstraction of an output data stream.
 */
class Ostream : public Stream, public std::ostream {
private:
  friend iOstream;

  using std::ostream::ostream;
};

/**
 * @brief Inserts CRLF sequence into the `ostr`.
 */
DMITIGR_FCGI_API std::ostream& crlf(std::ostream& ostr);

/**
 * @brief Inserts CRLFCRLF sequence into the `ostr`.
 */
DMITIGR_FCGI_API std::ostream& crlfcrlf(std::ostream& ostr);

} // namespace dmitigr::fcgi

#ifdef DMITIGR_FCGI_HEADER_ONLY
#include "dmitigr/fcgi/streams.cpp"
#endif

#endif  // DMITIGR_FCGI_STREAMS_HPP
