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

#include "../../src/base/assert.hpp"
#include "../../src/fcgi/fcgi.hpp"

#include <atomic>
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>

namespace {

constexpr std::size_t pool_size = 64;

class Busyness_counter {
public:
  ~Busyness_counter()
  {
    assert(value_ >= 1);
    value_--;
  }

  Busyness_counter()
  {
    value_++;
  }

  static std::size_t value()
  {
    return value_;
  }

private:
  static std::atomic<std::size_t> value_;
};

std::atomic<std::size_t> Busyness_counter::value_{};

bool is_ready()
{
  return Busyness_counter::value() <= pool_size;
}

} // namespace

int main(int, char**)
{
  namespace fcgi = dmitigr::fcgi;
  try {
    const auto serve = [](auto* const server)
    {
      while (true) {
        const auto conn = server->accept();
        if (is_ready()) {
          const Busyness_counter counter;
          conn->out() << "Content-Type: text/plain" << fcgi::crlfcrlf;
          // Simulate being busy.
          std::this_thread::sleep_for(std::chrono::milliseconds{50});
          conn->out() << "Hello from dmitigr::fcgi!" << fcgi::crlf;
        } else
          // Report "Service Unavailable".
          conn->out() << "Status: 503" << fcgi::crlfcrlf;
        conn->close(); // optional.
      }
    };

    const auto port = 9000;
    const auto backlog = 64;
    const auto overload_pool_size = std::thread::hardware_concurrency();
    std::clog << "Multi-threaded FastCGI server started:\n"
      << "  port = " << port << "\n"
      << "  backlog = " << backlog << "\n"
      << "  working thread pool size = " << pool_size << "\n"
      << "  overload thread pool size = " << overload_pool_size << std::endl;

    fcgi::Listener server{fcgi::Listener_options{"0.0.0.0", port, backlog}};
    DMITIGR_ASSERT(!server.is_listening());
    server.listen();
    DMITIGR_ASSERT(server.is_listening());

    std::vector<std::thread> threads(pool_size + overload_pool_size);
    for (auto& t : threads)
      t = std::thread{serve, &server};

    for (auto& t : threads)
      t.join();

    server.close();
    DMITIGR_ASSERT(!server.is_listening());
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "unknown error" << std::endl;
    return 2;
  }
}
