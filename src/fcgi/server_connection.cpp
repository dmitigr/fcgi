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

#include "../base/assert.hpp"
#include "basics.hpp"
#include "exceptions.hpp"
#include "server_connection.hpp"

namespace dmitigr::fcgi::detail {

/// The base implementation of the Server_connection.
class iServer_connection : public Server_connection {
public:
  /// The constructor.
  explicit iServer_connection(std::unique_ptr<net::Descriptor> io,
    const Role role, const int request_id, const bool is_keep_connection)
    : is_keep_connection_{is_keep_connection}
    , role_{role}
    , request_id_{request_id}
  {
    io_ = std::move(io);
    DMITIGR_ASSERT(io_);
  }

  // ---------------------------------------------------------------------------
  // Connection overridings
  // ---------------------------------------------------------------------------

  int request_id() const noexcept override
  {
    return request_id_;
  }

  Role role() const noexcept override
  {
    return role_;
  }

  std::size_t parameter_count() const noexcept override
  {
    return parameters_.pair_count();
  }

  std::optional<std::size_t>
  parameter_index(const std::string_view name) const noexcept override
  {
    return parameters_.pair_index(name);
  }

  std::string_view parameter(const std::size_t index) const override
  {
    if (!(index < parameter_count()))
      throw Exception{"cannot get FastCGI parameter by using invalid index"};
    return parameters_.pair(index).value();
  }

  std::string_view parameter(const std::string_view name) const override
  {
    if (const auto index = parameter_index(name))
      return parameters_.pair(*index).value();
    else
      throw Exception{std::string{"cannot get FastCGI parameter "}.append(name)};
  }

  // ---------------------------------------------------------------------------
  // Server_connection overridings
  // ---------------------------------------------------------------------------

  int application_status() const noexcept override
  {
    return application_status_;
  }

  void set_application_status(const int status) override
  {
    application_status_ = status;
  }

  bool is_keep_connection() const
  {
    return is_keep_connection_;
  }

private:
  friend server_Istream;
  friend server_Streambuf;

  bool is_keep_connection_{};
  Role role_{};
  int request_id_{};
  int application_status_{};
  std::unique_ptr<net::Descriptor> io_;
  detail::Names_values parameters_;
};

} // namespace dmitigr::fcgi::detail
