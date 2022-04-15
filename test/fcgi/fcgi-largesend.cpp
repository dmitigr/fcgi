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

#include "../../src/fcgi/fcgi.hpp"
#include "../../src/rnd/string.hpp"

#include <iostream>

int main()
{
  namespace fcgi = dmitigr::fcgi;
  namespace rnd = dmitigr::rnd;

  rnd::seed_by_now();
  std::streamsize str_size{100500};
  try {
    const auto port = 9000;
    const auto backlog = 64;
    fcgi::Listener server{fcgi::Listener_options{"0.0.0.0", port, backlog}};
    server.listen();
    while (true) {
      if (const auto conn = server.accept()) {
        conn->out() << "Content-Type: text/plain" << fcgi::crlfcrlf;
        const std::string str = rnd::random_string("abc", str_size);
        conn->out() << str << "\n" << str_size;
        str_size += rnd::cpp_pl_3rd(str_size);
      }
    }
  } catch (const std::exception& e) {
    std::cerr << "Oops: " << e.what() << std::endl;
    return 1;
  }
}
