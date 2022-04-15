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

#ifndef DMITIGR_FCGI_LISTENER_OPTIONS_HPP
#define DMITIGR_FCGI_LISTENER_OPTIONS_HPP

#include "../fs/filesystem.hpp"
#include "../net/listener.hpp"
#include "dll.hpp"
#include "types_fwd.hpp"

#include <optional>
#include <string>

namespace dmitigr::fcgi {

/**
 * @brief FastCGI Listener options.
 */
class Listener_options final {
public:
  /// @name Constructors
  /// @{

#ifdef _WIN32
  /**
   * @returns A new instance of the options for listeners of
   * Windows Named Pipes (WNP).
   *
   * @param pipe_name - the pipe name.
   *
   * @par Effects
   * `endpoint()->communication_mode() == Communication_mode::wnp`.
   */
  DMITIGR_FCGI_API Listener_options(std::string pipe_name);
#else
  /**
   * @returns A new instance of the options for listeners of
   * Unix Domain Sockets (UDS).
   *
   * @param path - the path to the socket.
   * @param backlog - the maximum size of the queue of pending connections.
   *
   * @par Effects
   * `endpoint()->communication_mode() == Communication_mode::uds`.
   */
  DMITIGR_FCGI_API Listener_options(std::filesystem::path path, int backlog);
#endif
  /**
   * @overload
   *
   * @returns A new instance of the options for listeners of network.
   *
   * @param address - IPv4 or IPv6 address to use for binding on.
   * @param port - the port number to use for binding on.
   * @param backlog - the maximum size of the queue of pending connections.
   *
   * @par Requires
   * `port > 0`.
   *
   * @par Effects
   * `endpoint()->communication_mode() == Communication_mode::net`.
   */
  DMITIGR_FCGI_API Listener_options(std::string address, int port, int backlog);

  /// @}

  /**
   * @returns The endpoint identifier.
   */
  DMITIGR_FCGI_API const net::Endpoint& endpoint() const noexcept;

  /**
   * @returns The value of backlog if the communication mode of the endpoint is
   * not `Communication_mode::wnp`, or `std::nullopt` otherwise.
   */
  DMITIGR_FCGI_API std::optional<int> backlog() const noexcept;

private:
  friend Listener;

  net::Listener_options options_;

  Listener_options() noexcept = default;
};

} // namespace dmitigr::fcgi

#ifndef DMITIGR_FCGI_NOT_HEADER_ONLY
#include "listener_options.cpp"
#endif

#endif  // DMITIGR_FCGI_LISTENER_OPTIONS_HPP
