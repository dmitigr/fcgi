# -*- cmake -*-
#
# Copyright 2022 Dmitry Igrishin
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_cpplipa_set_library_info(fcgi 1 0 0 "FastCGI implementation")

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_fcgi_headers
  basics.hpp
  connection.hpp
  exceptions.hpp
  listener.hpp
  listener_options.hpp
  server_connection.hpp
  streambuf.hpp
  streams.hpp
  types_fwd.hpp
  )

set(dmitigr_fcgi_implementations
  basics.cpp
  listener.cpp
  listener_options.cpp
  server_connection.cpp
  server_connection_stacked.cpp
  streambuf.cpp
  streams.cpp
  )

# ------------------------------------------------------------------------------
# Tests
# ------------------------------------------------------------------------------

if(DMITIGR_CPPLIPA_TESTS)
  set(dmitigr_fcgi_tests hello hellomt largesend overload)
  set(dmitigr_fcgi_tests_target_link_libraries dmitigr_base dmitigr_rnd)
  if(UNIX)
    list(APPEND dmitigr_fcgi_tests_target_link_libraries pthread)
  endif()
endif()
