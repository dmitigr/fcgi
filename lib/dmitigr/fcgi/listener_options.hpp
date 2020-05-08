// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or fcgi.hpp

#ifndef DMITIGR_FCGI_LISTENER_OPTIONS_HPP
#define DMITIGR_FCGI_LISTENER_OPTIONS_HPP

#include "dmitigr/fcgi/dll.hpp"
#include "dmitigr/fcgi/types_fwd.hpp"

#include <dmitigr/base/filesystem.hpp>
#include <dmitigr/net/types_fwd.hpp>

#include <memory>
#include <optional>
#include <string>

namespace dmitigr::fcgi {

/**
 * @brief FastCGI Listener options.
 */
class Listener_options {
public:
  /**
   * @brief The destructor.
   */
  virtual ~Listener_options() = default;

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
   * `(endpoint_id()->communication_mode() == Communication_mode::wnp)`.
   */
  static DMITIGR_FCGI_API std::unique_ptr<Listener_options> make(std::string pipe_name);
#else
  /**
   * @returns A new instance of the options for listeners of
   * Unix Domain Sockets (UDS).
   *
   * @param path - the path to the socket.
   * @param backlog - the maximum size of the queue of pending connections.
   *
   * @par Effects
   * `(endpoint_id()->communication_mode() == Communication_mode::uds)`.
   */
  static DMITIGR_FCGI_API std::unique_ptr<Listener_options> make(std::filesystem::path path, int backlog);
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
   * `(port > 0)`.
   *
   * @par Effects
   * `(endpoint_id()->communication_mode() == Communication_mode::net)`.
   */
  static DMITIGR_FCGI_API std::unique_ptr<Listener_options> make(std::string address, int port, int backlog);

  /**
   * @returns A new instance of the Listener initialized with this instance.
   *
   * @see Listener::make().
   */
  virtual std::unique_ptr<Listener> make_listener() const = 0;

  /**
   * @returns The copy of this instance.
   */
  virtual std::unique_ptr<Listener_options> to_listener_options() const = 0;

  /// @}

  /**
   * @returns The endpoint identifier.
   */
  virtual const net::Endpoint_id* endpoint_id() const = 0;

  /**
   * @returns The value of backlog if the communication mode of the endpoint is
   * not `Communication_mode::wnp`, or `std::nullopt` otherwise.
   */
  virtual std::optional<int> backlog() const = 0;

private:
  friend detail::iListener_options;

  Listener_options() = default;
};

} // namespace dmitigr::fcgi

#ifdef DMITIGR_FCGI_HEADER_ONLY
#include "dmitigr/fcgi/listener_options.cpp"
#endif

#endif  // DMITIGR_FCGI_LISTENER_OPTIONS_HPP
