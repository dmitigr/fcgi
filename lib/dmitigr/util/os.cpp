// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or util.hpp

#include "dmitigr/util/debug.hpp"
#include "dmitigr/util/os.hpp"
#include "dmitigr/util/implementation_header.hpp"

#include <cstdlib>
#include <memory>
#include <system_error>

#ifdef _WIN32

/*
 * For historical reasons, the Windows.h header defaults to including the
 * Winsock.h header file for Windows Sockets 1.1. The declarations in the
 * Winsock.h header file will conflict with the declarations in the Winsock2.h
 * header file required by Windows Sockets 2.0. The WIN32_LEAN_AND_MEAN macro
 * prevents the Winsock.h from being included by the Windows.h header.
 *
 * https://docs.microsoft.com/en-us/windows/desktop/winsock/include-files-2
 * https://social.msdn.microsoft.com/Forums/vstudio/en-US/671124df-c42b-48b8-a4ac-3413230bc43b/dll-compilationredefinition-error
 */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include <Winnls.h>
#include <Lmcons.h>

// IO headers
#include <io.h>
#include <Stdio.h>

#include <direct.h> // _getcwd()

#else // Unix

#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>

#endif

namespace dmitigr::os {

namespace {

inline char* getcwd__(char* buffer, std::size_t size)
{
#ifdef _WIN32
  return _getcwd(buffer, int(size));
#else
  return getcwd(buffer, size);
#endif
}

inline char* os_cwd()
{
  constexpr std::size_t max_path_size = 128 * 128;
  std::size_t sz = 16;
  auto* buf = static_cast<char*>(std::calloc(1, sz));
  while (!getcwd__(buf, sz)) {
    if (errno == ERANGE) {
      sz *= 2;
      if (sz > max_path_size) {
        std::free(buf);
        return nullptr;
      } else {
        if (auto* new_buf = static_cast<char*>(std::realloc(buf, sz)))
          buf = new_buf;
        else {
          std::free(buf);
          return nullptr;
        }
      }
    } else { // ENOMEM
      DMITIGR_ASSERT_NOTHROW(!buf);
      return nullptr;
    }
  }

  return buf;
}

} // namespace

DMITIGR_UTIL_INLINE std::string current_working_directory()
{
  std::unique_ptr<char[], void (*)(void*)> guarded{os_cwd(), &std::free};
  return guarded ? guarded.get() : std::string{};
}

DMITIGR_UTIL_INLINE std::string current_username()
{
  std::string result;
#ifdef _WIN32
  constexpr DWORD max_size = UNLEN + 1;
  result.resize(max_size);
  DWORD sz{max_size};
  if (::GetUserName(result.data(), &sz) != 0)
    result.resize(sz - 1);
  else
    throw std::system_error{int(::GetLastError()), std::system_category(), "dmitigr::os::current_username()"};
#else
  struct passwd pwd;
  struct passwd *pwd_ptr{};
  const uid_t uid = geteuid();
  const std::size_t bufsz = []()
  {
    auto result = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (result == -1)
      result = 16384;
    return result;
  }();
  const std::unique_ptr<char[]> buf{new char[bufsz]};
  const int s = getpwuid_r(uid, &pwd, buf.get(), bufsz, &pwd_ptr);
  if (pwd_ptr == nullptr) {
    if (s == 0)
      throw std::runtime_error{"current username is unavailable (possible something wrong with the OS)"};
    else
      throw std::system_error{s, std::system_category(), "dmitigr::os::current_username()"};
  } else
    result = pwd.pw_name;
#endif
  return result;
}

DMITIGR_UTIL_INLINE std::optional<std::string> environment_variable(const std::string& name)
{
#ifdef _WIN32
  const std::unique_ptr<char, void(*)(void*)> buffer{nullptr, &std::free};
  char* result = buffer.get();
  const auto err = _dupenv_s(&result, nullptr, name.c_str());
  if (err)
    throw std::system_error{err, std::system_category(), "dmitigr::os::environment_variable()"};
#else
  const char* const result = std::getenv(name.c_str());
#endif
  return result ? std::make_optional(std::string{result}) : std::nullopt;
}

} // namespace dmitigr::os

#include "dmitigr/util/implementation_footer.hpp"
