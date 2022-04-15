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

#include "../base/assert.hpp"
#include "basics.hpp"
#include "exceptions.hpp"
#include "streambuf.hpp"
#include "streams.hpp"

namespace dmitigr::fcgi::detail {

/// The base implementation of Istream.
class iIstream : public Istream {
private:
  friend server_Istream;

  using Istream::Istream;
};

/// The Istream implementation for a FastCGI server.
class server_Istream final : public iIstream {
public:
  server_Istream(iServer_connection* const connection,
    char_type* const buffer, const std::streamsize buffer_size)
    : iIstream{&streambuf_}
    , streambuf_{connection, buffer, buffer_size, Stream_type::params}
  {
    // Reading the parameters.
    DMITIGR_ASSERT(stream_type() == Stream_type::params);
    connection->parameters_ = detail::Names_values{*this, 32};
    if (!eof() || bad())
      throw Exception{"unexpected FastCGI input stream state after parameters "
        "read attempt"};

    // Resetting the stream.
    const auto role = connection->role();
    if (role == Role::responder || role == Role::filter) {
      clear();
      streambuf().reset_reader(Stream_type::in);
    }
  }

  const server_Streambuf& streambuf() const noexcept override
  {
    return streambuf_;
  }

  server_Streambuf& streambuf() noexcept override
  {
    return streambuf_;
  }

  bool is_closed() const noexcept override
  {
    return streambuf_.is_closed();
  }

  Stream_type stream_type() const noexcept override
  {
    return streambuf_.stream_type();
  }

private:
  server_Streambuf streambuf_;
};

// =============================================================================

/// The base implementation of Ostream.
class iOstream : public Ostream {
private:
  friend server_Ostream;

  using Ostream::Ostream;
};

/// The Ostream implementation for a FastCGI server.
class server_Ostream final : public iOstream {
public:
  server_Ostream(iServer_connection* const connection, char_type* const buffer,
    const std::streamsize buffer_size, const Stream_type type)
    : iOstream{&streambuf_}
    , streambuf_{connection, buffer, buffer_size, type}
  {
    DMITIGR_ASSERT(stream_type() == Stream_type::out ||
      stream_type() == Stream_type::err);
  }

  const server_Streambuf& streambuf() const noexcept override
  {
    return streambuf_;
  }

  server_Streambuf& streambuf() noexcept override
  {
    return streambuf_;
  }

  bool is_closed() const noexcept override
  {
    return streambuf_.is_closed();
  }

  Stream_type stream_type() const noexcept override
  {
    return streambuf_.stream_type();
  }

private:
  server_Streambuf streambuf_;
};

} // namespace dmitigr::fcgi::detail

namespace dmitigr::fcgi {

DMITIGR_FCGI_INLINE std::ostream& crlf(std::ostream& ostr)
{
  return ostr.write("\r\n", 2);
}

DMITIGR_FCGI_INLINE std::ostream& crlfcrlf(std::ostream& ostr)
{
  return ostr.write("\r\n\r\n", 4);
}

} // namespace dmitigr::fcgi
