// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or fcgi.hpp

#ifndef DMITIGR_FCGI_SERVER_CONNECTION_HPP
#define DMITIGR_FCGI_SERVER_CONNECTION_HPP

#include "dmitigr/fcgi/connection.hpp"

namespace dmitigr::fcgi {

/**
 * @brief A FastCGI server connection.
 */
class Server_connection : public Connection {
public:
  /**
   * @returns The input stream, associated with the input data stream.
   */
  virtual Istream& in() = 0;

  /**
   * @returns The output stream, associated with the output data stream.
   */
  virtual Ostream& out() = 0;

  /**
   * @returns The output stream, associated with the error data stream.
   */
  virtual Ostream& err() = 0;

  /**
   * @returns The application status code for transmitting to the client
   * upon closing the connection. By default the returned value is `0`.
   *
   * @see set_application_status().
   */
  virtual int application_status() const = 0;

  /**
   * @brief Sets the application status code for transmitting to the client
   * upon closing the connection.
   *
   * @see application_status().
   */
  virtual void set_application_status(int status) = 0;

private:
  friend detail::iServer_connection;

  Server_connection() = default;
};

} // namespace dmitigr::fcgi

#ifdef DMITIGR_FCGI_HEADER_ONLY
#include "dmitigr/fcgi/server_connection.cpp"
#endif

#endif  // DMITIGR_FCGI_SERVER_CONNECTION_HPP
