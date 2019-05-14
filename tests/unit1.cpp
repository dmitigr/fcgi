// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or fcgi.hpp

#include "unit.hpp"
#include "dmitigr/fcgi.hpp"

#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

namespace fcgi = dmitigr::fcgi;

namespace {

constexpr auto pool_size = 64;

class Busyness_counter {
public:
  ~Busyness_counter()
  {
    value_--;
  }

  Busyness_counter()
  {
    value_++;
  }

  static int value()
  {
    return value_;
  }

private:
  static std::atomic<int> value_;
};

std::atomic<int> Busyness_counter::value_{};

bool is_ready()
{
  return Busyness_counter::value() <= pool_size;
}

} // namespace

int main(int, char**)
{
#ifdef _WIN32
  static constexpr char* const crlf = "\n";
  static constexpr char* const crlfcrlf = "\n\n";
#else
  static constexpr char* const crlf = "\r\n";
  static constexpr char* const crlfcrlf = "\r\n\r\n";
#endif

  try {
    static const auto serve = [](auto* server)
    {
      while (true) {
        const auto conn = server->accept();
        if (is_ready()) {
          const Busyness_counter counter;
          conn->out() << "Content-Type: text/plain" << crlfcrlf;
          std::this_thread::sleep_for(std::chrono::milliseconds{50}); // The job imitation.
          conn->out() << "Hello from dmitigr::fcgi!" << crlf;
        } else
          conn->out() << "Status: 503" << crlfcrlf; // Report "Service Unavailable".
        conn->close(); // Optional.
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

    const auto server = fcgi::Listener_options::make("0.0.0.0", port, backlog)->make_listener();
    ASSERT(!server->is_listening());
    server->listen();
    ASSERT(server->is_listening());

    std::vector<std::thread> threads(pool_size + overload_pool_size);
    for (auto& t : threads)
      t = std::thread{serve, server.get()};

    for (auto& t : threads)
      t.join();

    server->close();
    ASSERT(!server->is_listening());
  } catch (const std::exception& e) {
    std::cerr << "error: " << e.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "unknown error" << std::endl;
    return 2;
  }
  return 0;
}
