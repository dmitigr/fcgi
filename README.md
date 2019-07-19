The FastCGI implementation on modern C++ {#mainpage}
====================================================

**THIS PROJECT IS A PART OF [CEFEIKA][cefeika] PROJECT NOW!!!**

The implementation is aimed to be as simple as possible (maybe even "dumb") for
better understanding and correctness.

**ATTENTION, this software is "beta" quality, and the API is a subject to change!**

Any [feedback][dmitigr_mailbox] (*especially results of testing*) is highly appreciated!

Please note, there are [official Dmitigr Fcgi documentation site][doc].
Because of the Dmitigr Fcgi documentation is generated by [Doxygen], most of references
to the classes and methods on the official documentation site are clickable, which makes
the familiarization more convenient. Also there are [overview class diagram][doc_diagram]
of the API for better understanding.

Features
========

Reliable and fast implementation of the FastCGI protocol.

- Easy to use API;
- High performance;
- Can be used as shared, static or "header-only" library.

Hello, World
============

```cpp
#include <dmitigr/fcgi.hpp>

#include <iostream>

int main(int, char**)
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
  return 0;
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

int main(int, char**)
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
  } catch (...) {
    std::cerr << "unknown error" << std::endl;
    return 2;
  }
  return 0;
}
```

Tutorial
========

Client programs that use Dmitigr Fcgi must include header file `dmitigr/fcgi.hpp`.
and must link with `dmitigr_fcgi` (or the debug build - `dmitigr_fcgid`) if Dmitigr
Fcgi is used as a regular library. (There is no need in linkage if the header-only
version is used.)

**WARNING** Headers other than `dmitigr/fcgi.hpp` should be *avoided* to use in
applications since that headers are subject to reorganize. Also the namespace
`dmitigr::fcgi::detail`  consists of the implementation details and *should not*
be used in the client code.

Download
========

The Dmitigr Fcgi repository is located at Github [here][dmitigr_github].

Installation and consuming
==========================

Dependencies
------------

- [CMake] build system version 3.13+;
- C++17 compiler ([GCC] 7.3+ or [Microsoft Visual C++][Visual_Studio] 15.7+);
- [dmitigr_common] library.

Build time settings
-------------------

Settings that may be specified at build time by using [CMake] variables are:
  1. the type of the build (only meaningful to single-configuration generators);
  2. the flag to build the shared library (default is on);
  3. the flag to only install the header-only library (default is off);
  4. the flag to build the tests (default is on);
  5. installation directories.

Details:

|CMake variable|Possible values|Default on Unix|Default on Windows|
|:-------------|:--------------|:--------------|:-----------------|
|**The type of the build**||||
|CMAKE_BUILD_TYPE|Debug \| Release \| RelWithDebInfo \| MinSizeRel|Debug|Debug|
|**The flag to build the shared library**||||
|BUILD_SHARED_LIBS|On \| Off|On|On|
|**The flag to only install the header-only library**||||
|DMITIGR_FCGI_HEADER_ONLY|On \| Off|Off|Off|
|**The flag of building the tests**||||
|DMITIGR_FCGI_BUILD_TESTS|On \| Off|On|On|
|**Installation directories**||||
|CMAKE_INSTALL_PREFIX|*an absolute path*|"/usr/local"|"%ProgramFiles%\dmitigr_fcgi"|
|DMITIGR_FCGI_CMAKE_INSTALL_DIR|*a path relative to CMAKE_INSTALL_PREFIX*|"share/dmitigr_fcgi/cmake"|"cmake"|
|DMITIGR_FCGI_DOC_INSTALL_DIR|*a path relative to CMAKE_INSTALL_PREFIX*|"share/dmitigr_fcgi/doc"|"doc"|
|DMITIGR_FCGI_LIB_INSTALL_DIR|*a path relative to CMAKE_INSTALL_PREFIX*|"lib"|"lib"|
|DMITIGR_FCGI_INCLUDE_INSTALL_DIR|*a path relative to CMAKE_INSTALL_PREFIX*|"include"|"include"|

Installation in common
----------------------

Dmitigr Fcgi is depends on [dmitigr_common].

**WARNING** It's highly recommended to update the [dmitigr_common] library (just pull and reinstall)
before every build of the Dmitigr Fcgi library!

Installation on Linux
---------------------

    $ git clone https://github.com/dmitigr/fcgi.git
    $ mkdir -p fcgi/build
    $ cd fcgi/build
    $ cmake -DCMAKE_BUILD_TYPE=Debug ..
    $ make
    $ sudo make install

Installation on Microsoft Windows
---------------------------------

Run the Developer Command Prompt for Visual Studio and type:

    > git clone https://github.com/dmitigr/fcgi.git
    > mkdir fcgi\build
    > cd fcgi\build
    > cmake -G "Visual Studio 15 2017 Win64" ..
    > cmake --build . --config Debug

Next, run the Elevated Command Prompt (i.e. the command prompt with administrator privileges) and type:

    > cd fcgi\build
    > cmake -DBUILD_TYPE=Debug -P cmake_install.cmake

To make the installed DLL available for *any* application that depends on it, the symbolic link to the
dmitigr_fcgi.dll (or to the debug version - dmitigr_fcgid.dll) should be created:

  - in %SYSTEMROOT%\\System32 for the 64-bit DLL on 64-bit host (or for 32-bit DLL on 32-bit host);
  - in %SYSTEMROOT%\\SysWOW64 for the 32-bit DLL on 64-bit host.

To create the symbolic link run the Elevated Command Prompt and use `mklink` command, for example:

    > cd /d %SYSTEMROOT%\System32
    > mklink dmitigr_fcgid.dll "%ProgramFiles%\dmitigr_fcgi\lib\dmitigr_fcgid.dll"

Consuming
---------

If you are using CMake the consuming of the Dmitigr Fcgi library is quite simple. For example:

```cmake
cmake_minimum_required(VERSION 3.13)
project(foo)
find_package(dmitigr_fcgi REQUIRED) # find shared version of the Dmitigr Fcgi library
set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)
add_executable(foo foo.cpp)
target_link_libraries(foo dmitigr_fcgi)
```

The above code snippet is minimal CMakeLists.txt that enough to build the
application `foo` that depends on the Dmitigr Fcgi library.

To consume the header-only version of the Dmitigr Fcgi library just specify it by using
`CONFIGS` option of [find_package][cmake_find_package]:

```cmake
find_package(dmitigr_fcgi REQUIRED CONFIGS dmitigr_fcgi_interface-config.cmake)
# ...
target_link_libraries(foo dmitigr_fcgi_interface)
```

License
=======

Dmitigr Fcgi is distributed under zlib license. For conditions of distribution and use,
see file `LICENSE.txt`.

Donations
=========

Dmitigr Fcgi has been developed on the own funds. Donations are welcome! To make
a donation, please go [here][dmitigr_paypal].

Feedback
========

Any feedback are welcome. [Contact us][dmitigr_mailbox].

Copyright
=========

Copyright (C) Dmitry Igrishin

[doc]: http://dmitigr.ru/en/projects/fcgi/doc/
[doc_diagram]: http://dmitigr.ru/en/projects/fcgi/doc/overview.class.violet.html
[dmitigr_mailbox]: mailto:dmitigr@gmail.com
[dmitigr_paypal]: https://paypal.me/dmitigr
[dmitigr_github]: https://github.com/dmitigr/fcgi.git
[dmitigr_common]: https://github.com/dmitigr/common.git

[cmake_find_package]: https://cmake.org/cmake/help/latest/command/find_package.html

[CMake]: https://cmake.org/
[Doxygen]: http://doxygen.org/
[GCC]: https://gcc.gnu.org/
[Visual_Studio]: https://www.visualstudio.com/
[cefeika]: https://github.com/dmitigr/cefeika.git
