// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or fcgi.hpp

#ifndef DMITIGR_FCGI_BASICS_HPP
#define DMITIGR_FCGI_BASICS_HPP

namespace dmitigr::fcgi {

/**
 * @brief A FastCGI role.
 */
enum class Role {
  /** The Responder role. */
  responder = 1,

  /** The Authorizer role. */
  authorizer = 2,

  /** The Filter role. */
  filter = 3
};

/**
 * @brief Represents a type of stream.
 */
enum class Stream_type {
  /**
   * A name-value pair stream used in sending name-value
   * pairs from a FastCGI client to a FastCGI server.
   */
  params = 4,

  /**
   * A stream used in sending arbitrary data
   * from a FastCGI client to a FastCGI server.
   */
  in = 5,

  /**
   * A stream used in sending arbitrary data
   * from a FastCGI server to a FastCGI client.
   */
  out = 6,

  /**
   * A stream used in sending error data
   * from a FastCGI server to a FastCGI client.
   */
  err = 7,

  /**
   * A stream used in sending additional data
   * from a FastCGI client to a FastCGI server.
   */
  data = 8
};

} // namespace dmitigr::fcgi

#ifdef DMITIGR_FCGI_HEADER_ONLY
#include "dmitigr/fcgi/basics.cpp"
#endif

#endif  // DMITIGR_FCGI_BASICS_HPP
