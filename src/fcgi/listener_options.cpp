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

#include "listener_options.hpp"

namespace dmitigr::fcgi {

#ifdef _WIN32
DMITIGR_FCGI_INLINE Listener_options::Listener_options(std::string pipe_name)
  : options_{std::move(pipe_name)}
{}
#endif

DMITIGR_FCGI_INLINE Listener_options::Listener_options(std::filesystem::path path,
  const int backlog)
  : options_{std::move(path), backlog}
{}

DMITIGR_FCGI_INLINE Listener_options::Listener_options(std::string address,
  const int port, const int backlog)
  : options_{std::move(address), port, backlog}
{}

DMITIGR_FCGI_INLINE const net::Endpoint& Listener_options::endpoint() const noexcept
{
  return options_.endpoint();
}

DMITIGR_FCGI_INLINE std::optional<int> Listener_options::backlog() const noexcept
{
  return options_.backlog();
}

} // namespace dmitigr::fcgi
