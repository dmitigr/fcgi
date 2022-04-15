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
#include "streams.hpp"

#include <array>
#include <cstdio>
#include <iostream>
#include <limits>

namespace dmitigr::fcgi::detail {

/// The Server_connection implementation based on stack-allocated buffers.
class stack_buffers_Server_connection final : public iServer_connection {
public:
  /// The size of the buffer of Stream_type::in.
  static constexpr std::size_t in_buffer_size  = 16384;

  /// The size of the buffer of Stream_type::out.
  static constexpr std::size_t out_buffer_size = 65528;

  /// The size of the buffer of Stream_type::err.
  static constexpr std::size_t err_buffer_size = 65528;

  ~stack_buffers_Server_connection() override
  {
    try {
      close();

      // -----------------------------------------------------------------------
      // TODO: support for Begin_request_body::Flags::keep_conn flag.
      // To make it possible the io_ object should be passed back to the
      // Listener which can reuse it for a new Server_connection.
      //
      // Begin_request_body::Flags::keep_conn flag has no effect if any stream
      // is with failbit set.
      // const bool keep_conn = keep_connection() && !err().fail() &&
      //   !out().fail() && !in().fail();
      // -----------------------------------------------------------------------

    } catch (const std::exception& e) {
      std::clog << "error upon closing FastCGI connection: %s\n" << e.what();
    } catch (...) {
      std::clog << "unknown error upon closing FastCGI connection\n";
    }
  }

  explicit stack_buffers_Server_connection(std::unique_ptr<net::Descriptor> io,
    const Role role,
    const int request_id,
    const bool is_keep_connection)
    : iServer_connection{std::move(io), role, request_id, is_keep_connection}
    , in_{this, in_buffer_.data(),
      static_cast<std::streamsize>(in_buffer_.size())}
    , out_{this, out_buffer_.data(),
      static_cast<std::streamsize>(out_buffer_.size()), Stream_type::out}
    , err_{this, err_buffer_.data(),
      static_cast<std::streamsize>(err_buffer_.size()), Stream_type::err}
  {
    static_assert(in_buffer_size <= std::numeric_limits<std::streamsize>::max());
    static_assert(out_buffer_size <= std::numeric_limits<std::streamsize>::max());
    static_assert(err_buffer_size <= std::numeric_limits<std::streamsize>::max());
  }

  // ---------------------------------------------------------------------------
  // Connection overridings
  // ---------------------------------------------------------------------------

  void close() override
  {
    // Attention: the order is important!
    err().streambuf().close();
    out().streambuf().close();
    in().streambuf().close();
  }

  bool is_closed() const noexcept override
  {
    return (err().is_closed() && out().is_closed() && in().is_closed());
  }

  // ---------------------------------------------------------------------------
  // Server_connection overridings
  // ---------------------------------------------------------------------------

  const server_Istream& in() const noexcept
  {
    return in_;
  }

  server_Istream& in() noexcept override
  {
    return in_;
  }

  const server_Ostream& out() const noexcept
  {
    return out_;
  }

  server_Ostream& out() noexcept override
  {
    return out_;
  }

  const server_Ostream& err() const noexcept
  {
    return err_;
  }

  server_Ostream& err() noexcept override
  {
    return err_;
  }

private:
  std::array<server_Streambuf::char_type, in_buffer_size> in_buffer_;
  std::array<server_Streambuf::char_type, out_buffer_size> out_buffer_;
  std::array<server_Streambuf::char_type, err_buffer_size> err_buffer_;

  server_Istream in_;
  server_Ostream out_;
  server_Ostream err_;
};

} // namespace dmitigr::fcgi::detail
