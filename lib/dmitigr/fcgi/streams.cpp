// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or fcgi.hpp

#include "dmitigr/fcgi/basics.hpp"
#include "dmitigr/fcgi/streambuf.hpp"
#include "dmitigr/fcgi/streams.hpp"
#include "dmitigr/fcgi/implementation_header.hpp"

#include <dmitigr/common/debug.hpp>

namespace dmitigr::fcgi {

/**
 * @internal
 *
 * @brief Represents the base of the Istream implementation.
 */
class iIstream : public Istream {
private:
  friend server_Istream;

  using Istream::Istream;
};

/**
 * @internal
 *
 * @brief Represents Istream implementation for the FastCGI server.
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
    DMITIGR_ASSERT(stream_type() == Stream_type::params);
    connection->parameters_ = detail::Name_value_pairs{*this, 32};
    if (!eof() || bad())
      throw std::runtime_error{"dmitigr::fcgi: unexpected input stream state after parameters read attempt"};

    // Resetting the stream.
    const auto role = connection->role();
    if (role == Role::responder || role == Role::filter) {
      clear();
      streambuf()->reset_reader(Stream_type::in);
    }
  }

  // Istream overridings:

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
 * @internal
 *
 * @brief Represents the base of the Ostream implementation.
 */
class iOstream : public Ostream {
private:
  friend server_Ostream;

  using Ostream::Ostream;
};

/**
 * @internal
 *
 * @brief Represents Ostream implementation for the FastCGI server.
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
    DMITIGR_ASSERT(stream_type() == Stream_type::out || stream_type() == Stream_type::err);
  }

  // Ostream overridings:

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

} // namespace dmitigr::fcgi

#include "dmitigr/fcgi/implementation_footer.hpp"
