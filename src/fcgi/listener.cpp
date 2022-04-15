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
#include "../net/listener.hpp"
#include "basics.hpp"
#include "exceptions.hpp"
#include "listener.hpp"
#include "server_connection_stacked.cpp"

namespace dmitigr::fcgi {

DMITIGR_FCGI_INLINE Listener::Listener(Listener_options options)
  : listener_{net::Listener::make(options.options_)}
  , listener_options_{std::move(options)}
{}

DMITIGR_FCGI_INLINE const Listener_options& Listener::options() const noexcept
{
  return listener_options_;
}

DMITIGR_FCGI_INLINE bool Listener::is_listening() const noexcept
{
  return listener_->is_listening();
}

DMITIGR_FCGI_INLINE void Listener::listen()
{
  listener_->listen();
}

DMITIGR_FCGI_INLINE bool Listener::wait(const std::chrono::milliseconds timeout)
{
  return listener_->wait(timeout);
}

DMITIGR_FCGI_INLINE std::unique_ptr<Server_connection> Listener::accept()
{
  auto io = listener_->accept();
  detail::Header header{io.get()};

  const auto end_request = [&](const detail::Protocol_status protocol_status)
  {
    const detail::End_request_record record{header.request_id(),
      0, protocol_status};
    const auto count = io->write(reinterpret_cast<const char*>(&record),
      sizeof(record));
    DMITIGR_ASSERT(count == sizeof(record));
  };

  if (header.record_type() == detail::Record_type::begin_request &&
    !header.is_management_record() &&
    header.content_length() == sizeof(detail::Begin_request_body)) {
    const detail::Begin_request_body body{io.get()};
    const auto role = body.role();
    if (role == Role::responder ||
      role == Role::authorizer || role == Role::filter) {
      return std::make_unique<detail::stack_buffers_Server_connection>(
        std::move(io), role, header.request_id(), body.is_keep_conn());
    } else {
      // This is a protocol violation.
      end_request(detail::Protocol_status::unknown_role);
      throw Exception{"unknown FastCGI role"};
    }
  } else {
    /*
     * Actualy, this is a protocol violation. But the FastCGI protocol has no
     * such a protocol status. Thus, detail::Protocol_status::cant_mpx_conn -
     * is the best suited protocol status code here.
     */
    end_request(detail::Protocol_status::cant_mpx_conn);
    throw Exception{"FastCGI protocol violation"};
  }
}

DMITIGR_FCGI_INLINE void Listener::close()
{
  listener_->close();
}

} // namespace dmitigr::fcgi
