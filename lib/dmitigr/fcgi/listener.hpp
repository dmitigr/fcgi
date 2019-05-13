// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or fcgi.hpp

#ifndef DMITIGR_FCGI_LISTENER_HPP
#define DMITIGR_FCGI_LISTENER_HPP

#include "dmitigr/fcgi/dll.hpp"
#include "dmitigr/fcgi/types_fwd.hpp"

#include <chrono>
#include <functional>
#include <memory>

namespace dmitigr::fcgi {

/**
 * @brief Represents an abstraction of a FastCGI listener.
 */
class Listener {
public:
  /**
   * @brief The destructor.
   */
  virtual ~Listener() = default;

  /// @name Constructors
  /// @{

  /**
   * @returns A new instance of the FastCGI listener.
   */
  static DMITIGR_FCGI_API std::unique_ptr<Listener> make(const Listener_options* options);

  /// @}

  /**
   * @returns The options of the listener.
   */
  virtual const Listener_options* options() const = 0;

  /**
   * @returns `true` if the listener is listening for new client connections, or
   * `false` otherwise.
   */
  virtual bool is_listening() const = 0;

  /**
   * @brief Starts listening.
   *
   * @par Requires
   * `(!is_listening())`.
   */
  virtual void listen() = 0;

  /**
   * @brief Waits for the next connection to accept.
   *
   * @param timeout - maximum amount of time to wait before return.
   * A special value of `-1` denotes "eternity".
   *
   * @returns `true` if the connection is ready to be accepted before
   * the `timeout` elapses, or `false` otherwise.
   *
   * @par Requires
   * `(is_listening())`
   *
   * @see accept(), accept_if().
   */
  virtual bool wait(std::chrono::milliseconds timeout = std::chrono::milliseconds{-1}) = 0;

  /**
   * @brief Accepts a new FastCGI connection, or rejects it for a some reason.
   *
   * @returns An instance of the accepted FastCGI connection.
   *
   * @par Requires
   * `(is_listening())`
   *
   * @see wait(), accept_if().
   */
  virtual std::unique_ptr<Server_connection> accept() = 0;

  /**
   * @brief Accepts a new FastCGI connection, or rejects it for a some reason.
   *
   * The new connection will be also rejected if `(is_ready() == false)`. In this
   * case the client will be automatically informed about the server overload.
   *
   * @param is_ready - the function that will be called just before the creation
   * of the object of type Server_connection.
   *
   * @returns An instance of the accepted FastCGI connection, or `nullptr` if
   * `(is_ready() == false)`.
   *
   * @par Requires
   * `(is_listening())`
   *
   * @see wait(), accept().
   */
  virtual std::unique_ptr<Server_connection> accept_if(std::function<bool ()> is_ready) = 0;

  /**
   * @brief Stops listening.
   */
  virtual void close() = 0;

private:
  friend iListener;

  Listener() = default;
};

} // namespace dmitigr::fcgi

#ifdef DMITIGR_FCGI_HEADER_ONLY
#include "dmitigr/fcgi/listener.cpp"
#endif

#endif  // DMITIGR_FCGI_LISTENER_HPP
