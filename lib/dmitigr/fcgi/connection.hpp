// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or fcgi.hpp

#ifndef DMITIGR_FCGI_CONNECTION_HPP
#define DMITIGR_FCGI_CONNECTION_HPP

#include "dmitigr/fcgi/types_fwd.hpp"

#include <cstddef>
#include <optional>
#include <string>

namespace dmitigr::fcgi {

/**
 * @brief Represents an abstraction of a FastCGI connection.
 */
class Connection {
public:
  /**
   * @brief Represents an abstraction of parameter received from a FastCGI client.
   */
  class Parameter {
  public:
    /**
     * @returns The parameter name.
     */
    virtual std::string_view name() const = 0;

    /**
     * @returns The parameter value.
     */
    virtual std::string_view value() const = 0;
  };

  /**
   * @brief The destructor.
   */
  virtual ~Connection() = default;

  /**
   * @returns The request identifier. (Always a non-zero value.)
   */
  virtual int request_id() const = 0;

  /**
   * @returns The role of the FastCGI application that serves this connection.
   */
  virtual Role role() const = 0;

  /**
   * @returns The number of parameters.
   */
  virtual std::size_t parameter_count() const = 0;

  /**
   * @returns The parameter index if parameter with the specified `name` is
   * present, or `std::nullopt` otherwise.
   */
  virtual std::optional<std::size_t> parameter_index(std::string_view name) const = 0;

  /**
   * @returns The parameter.
   *
   * @par Requires
   * `(index < parameter_count())`.
   */
  virtual const Parameter* parameter(std::size_t index) const = 0;

  /**
   * @overload
   *
   * @par Requires
   * `has_parameter(name)`.
   */
  virtual const Parameter* parameter(std::string_view name) const = 0;

  /**
   * @returns `true` if the parameter named by `name` is presents, or
   * `false` otherwise.
   */
  virtual bool has_parameter(std::string_view name) const = 0;

  /**
   * @returns `(parameter_count() > 0)`.
   */
  virtual bool has_parameters() const = 0;

  /**
   * @brief Closes the connection.
   *
   * @remarks Using this method is optional since it is implicitly called upon
   * the object destruction. The main reason of using it explicitly is to catch
   * possibly-thrown exceptions (which are always catched by the destructor).
   *
   * @par Effects
   * `(is_closed() == true)`.
   */
  virtual void close() = 0;

  /**
   * @returns `true` if the connection is closed, or `false` otherwise.
   */
  virtual bool is_closed() = 0;

private:
  friend Server_connection;

  Connection() = default;
};

} // namespace dmitigr::fcgi

#endif  // DMITIGR_FCGI_CONNECTION_HPP
