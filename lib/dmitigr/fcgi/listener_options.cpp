// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or fcgi.hpp

#include "dmitigr/fcgi/listener_options.hpp"
#include <dmitigr/net/net.hpp>
#include <dmitigr/base/debug.hpp>

namespace dmitigr::fcgi::detail {

/**
 * @brief The implementation of Listener_options.
 */
class iListener_options final : public Listener_options {
public:
  /**
   * @brief See Listener_options::make().
   */
  explicit iListener_options(net::Listener_options options)
    : options_{std::move(options)}
  {
    DMITIGR_ASSERT(is_invariant_ok());
  }

#ifdef _WIN32
  /**
   * @brief See Listener_options::make().
   */
  explicit iListener_options(std::string pipe_name)
    : iListener_options{net::Listener_options{std::move(pipe_name)}}
  {}
#else
  /**
   * @brief See Listener_options::make().
   */
  iListener_options(std::filesystem::path path, const int backlog)
    : iListener_options{net::Listener_options{std::move(path), backlog}}
  {}
#endif

  /**
   * @brief See Listener_options::make().
   */
  iListener_options(std::string address, const int port, const int backlog)
    : iListener_options{net::Listener_options{std::move(address), port, backlog}}
  {}

  // Listener overridinds:

  std::unique_ptr<Listener> make_listener() const override; // defined in listener.cpp

  std::unique_ptr<Listener_options> to_listener_options() const override
  {
    return std::make_unique<iListener_options>(options_);
  }

  const net::Endpoint& endpoint() const override
  {
    return options_.endpoint();
  }

  std::optional<int> backlog() const override
  {
    return options_.backlog();
  }

private:
  friend iListener;

  net::Listener_options options_;

  constexpr bool is_invariant_ok() const
  {
    return true;
  }

  iListener_options() = default;
};

} // namespace dmitigr::fcgi::detail

namespace dmitigr::fcgi {

#ifdef _WIN32
DMITIGR_FCGI_INLINE std::unique_ptr<Listener_options>
Listener_options::make(std::string pipe_name)
{
  using detail::iListener_options;
  return std::make_unique<iListener_options>(std::move(pipe_name));
}
#else
DMITIGR_FCGI_INLINE std::unique_ptr<Listener_options>
Listener_options::make(std::filesystem::path path, const int backlog)
{
  using detail::iListener_options;
  return std::make_unique<iListener_options>(std::move(path), backlog);
}
#endif

DMITIGR_FCGI_INLINE std::unique_ptr<Listener_options>
Listener_options::make(std::string address, const int port, const int backlog)
{
  using detail::iListener_options;
  return std::make_unique<iListener_options>(std::move(address), port, backlog);
}

} // namespace dmitigr::fcgi
