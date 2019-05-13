// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or fcgi.hpp

#ifndef DMITIGR_FCGI_BASICS_HPP
#define DMITIGR_FCGI_BASICS_HPP

namespace dmitigr::fcgi {

/**
 * @brief Represents a FastCGI role.
 */
enum class Role {
  /** Denotes the Responder role. */
  responder = 1,

  /** Denotes the Authorizer role. */
  authorizer = 2,

  /** Denotes the Filter role. */
  filter = 3
};

/**
 * @brief Represents a type of stream.
 */
enum class Stream_type {
  /**
   * Denotes the name-value pair stream used in sending
   * name-value pairs from FastCGI client to FastCGI server.
   */
  params = 4,

  /**
   * Denotes the stream used in sending arbitrary
   * data from FastCGI client to FastCGI server.
   */
  in = 5,

  /**
   * Denotes the stream used in sending arbitrary
   * data from the FastCGI server to the FastCGI client.
   */
  out = 6,

  /**
   * Denotes the stream used in sending error data
   * from the FastCGI server to the FastCGI client.
   */
  err = 7,

  /**
   * Denotes the stream used in sending additional
   * data from the FastCGI client to the FastCGI server.
   */
  data = 8
};

} // namespace dmitigr::fcgi

#ifdef DMITIGR_FCGI_HEADER_ONLY
#include "dmitigr/fcgi/basics.cpp"
#endif

#endif  // DMITIGR_FCGI_BASICS_HPP
