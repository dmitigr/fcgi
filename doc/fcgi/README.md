FastCGI implementation in C++ {#mainpage}
=========================================

Dmitigr Fcgi (hereinafter referred to as Fcgi) - is a reliable, easy-to-use
and high-performance [FastCGI] protocol implementation written in C++. Fcgi
is a part of the [Dmitigr Cefeika][dmitigr_cefeika] project.

**ATTENTION, this software is "beta" quality, and the API is a subject to change!**

Documentation
=============

The [Doxygen]-generated documentation is located [here][dmitigr_fcgi_doc]. There
is [overview class diagram][dmitigr_fcgi_doc_diagram].

Hello, World
============

```cpp
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
```

Hello, Multithreaded World
==========================

```cpp
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
```

Usage
=====

Please, see [Cefeika Usage][dmitigr_cefeika_usage] section for hints how to
link the library to a project.

Copyright
=========

Copyright (C) [Dmitry Igrishin][dmitigr_mail]

[dmitigr_mail]: mailto:dmitigr@gmail.com
[dmitigr_cefeika]: https://github.com/dmitigr/cefeika.git
[dmitigr_cefeika_usage]: https://github.com/dmitigr/cefeika.git#usage
[dmitigr_fcgi_doc]: http://dmitigr.ru/en/projects/cefeika/fcgi/doc/
[dmitigr_fcgi_doc_diagram]: http://dmitigr.ru/en/projects/cefeika/fcgi/doc/dmitigr_fcgi_overview.violet.html

[FastCGI]: https://en.wikipedia.org/wiki/FastCGI
[Doxygen]: http://doxygen.org/
