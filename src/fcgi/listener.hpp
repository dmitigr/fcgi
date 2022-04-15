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

#ifndef DMITIGR_FCGI_LISTENER_HPP
#define DMITIGR_FCGI_LISTENER_HPP

#include "dll.hpp"
#include "listener_options.hpp"
#include "types_fwd.hpp"

#include <chrono>

namespace dmitigr::fcgi {

/// A FastCGI listener.
class Listener final {
public:
  /// Constructs the listener.
  DMITIGR_FCGI_API explicit Listener(Listener_options options);

  /// @returns Options of the listener.
  DMITIGR_FCGI_API const Listener_options& options() const noexcept;

  /// @returns `true` if the listener is listening for new client connections.
  DMITIGR_FCGI_API bool is_listening() const noexcept;

  /**
   * @brief Starts listening.
   *
   * @par Requires
   * `!is_listening()`.
   */
  DMITIGR_FCGI_API void listen();

  /**
   * @brief Waits for a next connection to accept.
   *
   * @param timeout Maximum amount of time to wait before return. A special
   * value of `-1` denotes "eternity".
   *
   * @returns `true` if the connection is ready to be accepted before
   * the `timeout` elapses.
   *
   * @par Requires
   * `is_listening()`.
   *
   * @see accept().
   */
  DMITIGR_FCGI_API bool
  wait(std::chrono::milliseconds timeout = std::chrono::milliseconds{-1});

  /**
   * @brief Accepts a FastCGI connection, or rejects it in case of a
   * protocol violation.
   *
   * @returns An instance of the accepted FastCGI connection.
   *
   * @par Requires
   * `is_listening()`.
   *
   * @throws Exception in case of protocol violation.
   *
   * @see wait().
   */
  DMITIGR_FCGI_API std::unique_ptr<Server_connection> accept();

  /// Stops listening.
  DMITIGR_FCGI_API void close();

private:
  std::unique_ptr<net::Listener> listener_;
  Listener_options listener_options_;
};

} // namespace dmitigr::fcgi

#ifndef DMITIGR_FCGI_NOT_HEADER_ONLY
#include "listener.cpp"
#endif

#endif  // DMITIGR_FCGI_LISTENER_HPP
