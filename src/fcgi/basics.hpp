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

#ifndef DMITIGR_FCGI_BASICS_HPP
#define DMITIGR_FCGI_BASICS_HPP

namespace dmitigr::fcgi {

/// FastCGI role.
enum class Role {
  /// The Responder role.
  responder = 1,

  /// The Authorizer role.
  authorizer = 2,

  /// The Filter role.
  filter = 3
};

/// Represents a type of stream.
enum class Stream_type {
  /**
   * @brief A name-value pair stream used in sending name-value
   * pairs from a FastCGI client to a FastCGI server.
   */
  params = 4,

  /**
   * @brief A stream used in sending arbitrary data
   * from a FastCGI client to a FastCGI server.
   */
  in = 5,

  /**
   * @brief A stream used in sending arbitrary data
   * from a FastCGI server to a FastCGI client.
   */
  out = 6,

  /**
   * @brief A stream used in sending error data
   * from a FastCGI server to a FastCGI client.
   */
  err = 7,

  /**
   * @brief A stream used in sending additional data
   * from a FastCGI client to a FastCGI server.
   */
  data = 8
};

} // namespace dmitigr::fcgi

#ifndef DMITIGR_FCGI_NOT_HEADER_ONLY
#include "basics.cpp"
#endif

#endif  // DMITIGR_FCGI_BASICS_HPP
