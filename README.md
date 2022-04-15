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
    fcgi::Listener server{fcgi::Listener_options{"0.0.0.0", port, backlog}};
    server.listen();
    while (true) {
      if (const auto conn = server.accept()) {
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

constexpr std::size_t pool_size{64};

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

    fcgi::Listener server{fcgi::Listener_options{"0.0.0.0", port, backlog}};
    server.listen();
    std::vector<std::thread> threads(pool_size);
    for (auto& t : threads)
      t = std::thread{serve, &server};

    for (auto& t : threads)
      t.join();

    server.close(); // Optional.
  } catch (const std::exception& e) {
    std::cerr << "error: " << e.what() << std::endl;
    return 1;
  }
}
```

## Usage

### Quick usage as header-only library

Copy the contents of the `src` directory to a project directory which is under
an include path of a compiler, for example, `src/3rdparty/dmitigr`.

Create `hello.cpp`:

```C++
#include "dmitigr/fcgi/fcgi.hpp"

int main()
{
  // Application code here...
}
```

Compile `hello.cpp`:

```
g++ -std=c++17 -ohello hello.cpp
```

### Quick usage with CMake

Create build directory, configure, build and install:

```
cd fcgi
mkdir build && cd build
cmake ..
cmake --build .
sudo cmake --install .
```

Create `hello/hello.cpp`:

```C++
#include "dmitigr/fcgi/fcgi.hpp"

int main()
{
  // Application code here...
}
```

Create `hello/CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.16)
project(foo)
find_package(dmitigr_cpplipa REQUIRED COMPONENTS fcgi)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(hello hello.cpp)
target_link_libraries(hello dmitigr_fcgi)
```

Compile `hello/hello.cpp`:

```
mkdir hello/build && cd hello/build
cmake ..
cmake --build .
```

### Advanced usage

For more details please, see [usage section][dmitigr_cpplipa_usage] for hints
how to link `dmitigr::fcgi`.

[dmitigr_cpplipa_usage]: https://github.com/dmitigr/cpplipa.git#usage

[FastCGI]: https://en.wikipedia.org/wiki/FastCGI
