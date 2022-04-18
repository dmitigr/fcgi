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

#ifndef DMITIGR_FCGI_STREAMS_HPP
#define DMITIGR_FCGI_STREAMS_HPP

#include "dll.hpp"
#include "types_fwd.hpp"

#include <istream>
#include <ostream>

namespace dmitigr::fcgi {

/// A data stream.
class Stream {
public:
  /// The destructor.
  virtual ~Stream() = default;

  /// @returns The controlled Streambuf instance.
  virtual const Streambuf& streambuf() const noexcept = 0;

  /// @overload
  virtual Streambuf& streambuf() noexcept = 0;

  /// @returns `true` if the stream is closed (i.e. unusable anymore).
  virtual bool is_closed() const noexcept = 0;

  /**
   * @returns The type of stream. The value of Stream_type::data is returned
   * when the stream is switched to transmit the data file input to filter it
   * out (as prescribed for Role::filter).
   *
   * @remarks Since the data file input follows the content and `eof() == true`
   * right after all of content of Stream_type::in is read, the stream error
   * state flags must be cleared before reading the data file input.
   *
   * See also clear().
   */
  virtual Stream_type stream_type() const noexcept = 0;

private:
  friend Istream;
  friend Ostream;

  Stream() = default;
};

/// An input data stream.
class Istream : public Stream, public std::istream {
private:
  friend detail::iIstream;

  using std::istream::istream;
};

/// An output data stream.
class Ostream : public Stream, public std::ostream {
private:
  friend detail::iOstream;

  using std::ostream::ostream;
};

/// Inserts `CRLF` sequence into the `ostr`.
DMITIGR_FCGI_API std::ostream& crlf(std::ostream& ostr);

/// Inserts `CRLFCRLF` sequence into the `ostr`.
DMITIGR_FCGI_API std::ostream& crlfcrlf(std::ostream& ostr);

} // namespace dmitigr::fcgi

#ifndef DMITIGR_FCGI_NOT_HEADER_ONLY
#include "streams.cpp"
#endif

#endif  // DMITIGR_FCGI_STREAMS_HPP
