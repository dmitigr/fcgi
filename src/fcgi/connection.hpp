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

#ifndef DMITIGR_FCGI_CONNECTION_HPP
#define DMITIGR_FCGI_CONNECTION_HPP

#include "types_fwd.hpp"

#include <cstddef>
#include <optional>
#include <string>

namespace dmitigr::fcgi {

/// A FastCGI connection.
class Connection {
public:
  /// The destructor.
  virtual ~Connection() noexcept = default;

  /// @returns The request identifier. (Always a non-zero value.)
  virtual int request_id() const noexcept = 0;

  /// @returns The role of a FastCGI application that serves this connection.
  virtual Role role() const noexcept = 0;

  /// @returns The number of parameters.
  virtual std::size_t parameter_count() const noexcept = 0;

  /// @returns The parameter index if presents.
  virtual std::optional<std::size_t>
  parameter_index(std::string_view name) const noexcept = 0;

  /**
   * @returns The parameter.
   *
   * @par Requires
   * `index < parameter_count()`.
   */
  virtual std::string_view parameter(std::size_t index) const = 0;

  /**
   * @overload
   *
   * @par Requires
   * `parameter_index(name)`.
   */
  virtual std::string_view parameter(std::string_view name) const = 0;

  /**
   * @brief Closes the connection.
   *
   * @remarks Using this method is optional since it is implicitly called upon
   * the object destruction. The main reason of using it explicitly is to catch
   * possibly-thrown exceptions (which are always catched by the destructor).
   *
   * @par Effects
   * `is_closed()`.
   */
  virtual void close() = 0;

  /// @returns `true` if the connection is closed.
  virtual bool is_closed() const noexcept = 0;

private:
  friend Server_connection;

  Connection() noexcept = default;
};

} // namespace dmitigr::fcgi

#endif  // DMITIGR_FCGI_CONNECTION_HPP
