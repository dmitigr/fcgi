// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or fcgi.hpp

#include <dmitigr/fcgi.hpp>
#include <iostream>
#include <thread>
#include <vector>

namespace {

constexpr std::size_t pool_size = 64;

} // namespace

int main()
{
  namespace fcgi = dmitigr::fcgi;
  try {
    const auto serve = [](auto* const server)
    {
      while (true) {
        const auto conn = server->accept();
        conn->out() << "Content-Type: text/plain" << fcgi::crlfcrlf;
        conn->out() << "Hello from dmitigr::fcgi!";
        conn->close(); // Optional.
      }
    };

    const auto port = 9000;
    const auto backlog = 64;
    std::clog << "Multi-threaded FastCGI server started:\n"
              << "  port = " << port << "\n"
              << "  backlog = " << backlog << "\n"
              << "  thread pool size = " << pool_size << std::endl;

    const auto server = fcgi::Listener_options::make("0.0.0.0", port, backlog)->make_listener();
    server->listen();
    std::vector<std::thread> threads(pool_size);
    for (auto& t : threads)
      t = std::thread{serve, server.get()};

    for (auto& t : threads)
      t.join();

    server->close(); // Optional.
  } catch (const std::exception& e) {
    std::cerr << "error: " << e.what() << std::endl;
    return 1;
  }
}
