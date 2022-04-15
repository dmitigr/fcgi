# FastCGI implementation in C++ {#mainpage}

`dmitigr::fcgi` - is a [FastCGI] protocol implementation written in C++.

## Hello, World

```cpp
#include <dmitigr/fcgi/fcgi.hpp>
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

## Hello, Multithreaded World

```cpp
#include <dmitigr/fcgi/fcgi.hpp>
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

## Usage

Please, see [usage section][dmitigr_cpplipa_usage] for hints how to link the
library to a project.

[dmitigr_cpplipa_usage]: https://github.com/dmitigr/cpplipa.git#usage

[FastCGI]: https://en.wikipedia.org/wiki/FastCGI
