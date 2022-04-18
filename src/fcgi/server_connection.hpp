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

#ifndef DMITIGR_FCGI_SERVER_CONNECTION_HPP
#define DMITIGR_FCGI_SERVER_CONNECTION_HPP

#include "connection.hpp"

namespace dmitigr::fcgi {

/// A FastCGI server connection.
class Server_connection : public Connection {
public:
  /// @returns The input stream, associated with the input data stream.
  virtual Istream& in() noexcept = 0;

  /// @returns The output stream, associated with the output data stream.
  virtual Ostream& out() noexcept = 0;

  /// @returns The output stream, associated with the error data stream.
  virtual Ostream& err() noexcept = 0;

  /**
   * @returns The application status code for transmitting to the client
   * upon closing the connection. By default the returned value is `0`.
   *
   * @see set_application_status().
   */
  virtual int application_status() const noexcept = 0;

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

#ifndef DMITIGR_FCGI_NOT_HEADER_ONLY
#include "server_connection.cpp"
#endif

#endif  // DMITIGR_FCGI_SERVER_CONNECTION_HPP
