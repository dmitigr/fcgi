// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt

#include <dmitigr/fcgi.hpp>
#include <iostream>

int main()
{
  namespace fcgi = dmitigr::fcgi;
  try {
    const auto port = 9000;
    const auto backlog = 64;
    const auto server = fcgi::Listener_options::make("0.0.0.0", port, backlog)->make_listener();
    server->listen();
    while (true) {
      if (const auto conn = server->accept()) {
        conn->out() << "Content-Type: text/plain" << fcgi::crlfcrlf;
        conn->out() << "Hello from dmitigr::fcgi!";
      }
    }
  } catch (const std::exception& e) {
    std::cerr << "Oops: " << e.what() << std::endl;
    return 1;
  }
}
