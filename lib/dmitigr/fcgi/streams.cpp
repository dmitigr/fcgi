// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or fcgi.hpp

#include "dmitigr/fcgi/basics.hpp"
#include "dmitigr/fcgi/streambuf.hpp"
#include "dmitigr/fcgi/streams.hpp"

#include <cassert>

namespace dmitigr::fcgi::detail {

/**
 * @brief The base implementation of Istream.
 */
class iIstream : public Istream {
private:
  friend server_Istream;

  using Istream::Istream;
};

/**
 * @brief The Istream implementation for a FastCGI server.
 */
class server_Istream final : public iIstream {
public:
  /**
   * @brief The constructor.
   */
  server_Istream(iServer_connection* const connection, char_type* const buffer, const std::streamsize buffer_size)
    : iIstream{&streambuf_}
    , streambuf_{connection, buffer, buffer_size, Stream_type::params}
  {
    // Reading the parameters.
    assert(stream_type() == Stream_type::params);
    connection->parameters_ = detail::Names_values{*this, 32};
    if (!eof() || bad())
      throw std::runtime_error{"dmitigr::fcgi: unexpected input stream state after parameters read attempt"};

    // Resetting the stream.
    const auto role = connection->role();
    if (role == Role::responder || role == Role::filter) {
      clear();
      streambuf()->reset_reader(Stream_type::in);
    }
  }

  server_Streambuf* streambuf() const override
  {
    return &streambuf_;
  }

  bool is_closed() const override
  {
    return streambuf_.is_closed();
  }

  Stream_type stream_type() const override
  {
    return streambuf_.stream_type();
  }

private:
  mutable server_Streambuf streambuf_;
};

// =============================================================================

/**
 * @brief The base implementation of Ostream.
 */
class iOstream : public Ostream {
private:
  friend server_Ostream;

  using Ostream::Ostream;
};

/**
 * @brief The Ostream implementation for a FastCGI server.
 */
class server_Ostream final : public iOstream {
public:
  /**
   * @brief The constructor.
   */
  server_Ostream(iServer_connection* const connection, char_type* const buffer,
    const std::streamsize buffer_size, const Stream_type type)
    : iOstream{&streambuf_}
    , streambuf_{connection, buffer, buffer_size, type}
  {
    assert(stream_type() == Stream_type::out || stream_type() == Stream_type::err);
  }

  server_Streambuf* streambuf() const override
  {
    return &streambuf_;
  }

  bool is_closed() const override
  {
    return streambuf_.is_closed();
  }

  Stream_type stream_type() const override
  {
    return streambuf_.stream_type();
  }

private:
  mutable server_Streambuf streambuf_;
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
