// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or fcgi.hpp

#include <dmitigr/fcgi.hpp>

#include <iostream>

int main(int, char**)
{
#ifdef _WIN32
  const char* const crlf = "\n";
  const char* const crlfcrlf = "\n\n";
#else
  const char* const crlf = "\r\n";
  const char* const crlfcrlf = "\r\n\r\n";
#endif

  try {
    const auto port = 9000;
    const auto backlog = 64;
    const auto server = dmitigr::fcgi::Listener_options::make("0.0.0.0", port, backlog)->make_listener();
    server->listen();
    while (true) {
      if (const auto conn = server->accept()) {
        conn->out() << "Content-Type: text/plain" << crlfcrlf;
        conn->out() << "Hello from dmitigr::fcgi!" << crlf;
      }
    }
  } catch (const std::exception& e) {
    std::cerr << "Oops: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
