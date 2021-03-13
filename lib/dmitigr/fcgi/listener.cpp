// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or fcgi.hpp

#include "dmitigr/fcgi/basics.hpp"
#include "dmitigr/fcgi/listener.hpp"
#include "dmitigr/fcgi/listener_options.hpp"
#include "dmitigr/fcgi/server_connection.hpp"
#include "dmitigr/fcgi/streams.hpp"
#include <dmitigr/net/net.hpp>

#include <array>
#include <cassert>
#include <cstdio>
#include <limits>

namespace dmitigr::fcgi::detail {

/**
 * @brief The Server_connection implementation based on stack-allocated buffers.
 */
class stack_buffers_Server_connection final : public iServer_connection {
public:
  /** The size of the buffer of Stream_type::in. */
  static constexpr std::size_t in_buffer_size  = 16384;

  /** The size of the buffer of Stream_type::out. */
  static constexpr std::size_t out_buffer_size = 65528;

  /** The size of the buffer of Stream_type::err. */
  static constexpr std::size_t err_buffer_size = 65528;

  /**
   * @brief The destructor.
   */
  ~stack_buffers_Server_connection() override
  {
    try {
      close();

      // -----------------------------------------------------------------------
      // TODO: support for Begin_request_body::Flags::keep_conn flag.
      // To make it possible the io_ object should be passed back to the
      // Listener which can reuse it for a new Server_connection.
      //
      // Begin_request_body::Flags::keep_conn flag has no effect if any stream is with failbit set.
      // const bool keep_conn = keep_connection() && !err().fail() && !out().fail() && !in().fail();
      // -----------------------------------------------------------------------

    } catch (const std::exception& e) {
      std::fprintf(stderr, "dmitigr::fcgi: %s\n", e.what());
    } catch (...) {
      std::fprintf(stderr, "dmitigr::fcgi: failure\n");
    }
  }

  /**
   * @brief The constructor.
   */
  explicit stack_buffers_Server_connection(std::unique_ptr<net::Descriptor> io, const Role role,
    const int request_id, const bool is_keep_connection)
    : iServer_connection{std::move(io), role, request_id, is_keep_connection}
    , in_{this, in_buffer_.data(), static_cast<std::streamsize>(in_buffer_.size())}
    , out_{this, out_buffer_.data(), static_cast<std::streamsize>(out_buffer_.size()), Stream_type::out}
    , err_{this, err_buffer_.data(), static_cast<std::streamsize>(err_buffer_.size()), Stream_type::err}
  {
    static_assert(in_buffer_size <= std::numeric_limits<std::streamsize>::max());
    static_assert(out_buffer_size <= std::numeric_limits<std::streamsize>::max());
    static_assert(err_buffer_size <= std::numeric_limits<std::streamsize>::max());
  }

  // ---------------------------------------------------------------------------
  // Connection overridings
  // ---------------------------------------------------------------------------

  void close() override
  {
    // Attention: the order is important!
    err().streambuf()->close();
    out().streambuf()->close();
    in().streambuf()->close();
  }

  bool is_closed() override
  {
    return (err().is_closed() && out().is_closed() && in().is_closed());
  }

  // ---------------------------------------------------------------------------
  // Server_connection overridings
  // ---------------------------------------------------------------------------

  server_Istream& in() override
  {
    return in_;
  }

  server_Ostream& out() override
  {
    return out_;
  }

  server_Ostream& err() override
  {
    return err_;
  }

private:
  std::array<server_Streambuf::char_type, in_buffer_size> in_buffer_;
  std::array<server_Streambuf::char_type, out_buffer_size> out_buffer_;
  std::array<server_Streambuf::char_type, err_buffer_size> err_buffer_;

  server_Istream in_;
  server_Ostream out_;
  server_Ostream err_;
};

/**
 * @brief The implementation of Listener.
 */
class iListener final : public Listener {
public:
  /**
   * @brief See Listener::make().
   */
  explicit iListener(const Listener_options* const options)
    : listener_{net::Listener::make(static_cast<const iListener_options*>(options)->options_)}
    , listener_options_{static_cast<const iListener_options*>(options)->options_}
  {}

  const Listener_options* options() const override
  {
    return &listener_options_;
  }

  bool is_listening() const override
  {
    return listener_->is_listening();
  }

  void listen() override
  {
    listener_->listen();
  }

  bool wait(std::chrono::milliseconds timeout = std::chrono::milliseconds{-1}) override
  {
    return listener_->wait(timeout);
  }

  std::unique_ptr<Server_connection> accept() override
  {
    auto io = listener_->accept();
    detail::Header header{io.get()};

    const auto end_request = [&](const detail::Protocol_status protocol_status)
    {
      const detail::End_request_record record{header.request_id(), 0, protocol_status};
      const auto count = io->write(reinterpret_cast<const char*>(&record), sizeof(record));
      assert(count == sizeof(record));
    };

    if (header.record_type() == detail::Record_type::begin_request &&
      !header.is_management_record() &&
      header.content_length() == sizeof(detail::Begin_request_body)) {
      const detail::Begin_request_body body{io.get()};
      const auto role = body.role();
      if (role == Role::responder || role == Role::authorizer || role == Role::filter) {
        return std::make_unique<stack_buffers_Server_connection>(std::move(io), role, header.request_id(), body.is_keep_conn());
      } else {
        // This is a protocol violation.
        end_request(detail::Protocol_status::unknown_role);
        throw std::runtime_error{"dmitigr::fcgi: unknown role"};
      }
    } else {
      /*
       * Actualy, this is a protocol violation. But the FastCGI protocol has no such a protocol status.
       * Thus, detail::Protocol_status::cant_mpx_conn - is the best suited protocol status code here.
       */
      end_request(detail::Protocol_status::cant_mpx_conn);
      throw std::runtime_error{"dmitigr::fcgi: protocol violation"};
    }
  }

  void close() override
  {
    listener_->close();
  }

private:
  std::unique_ptr<net::Listener> listener_;
  iListener_options listener_options_;
};

std::unique_ptr<Listener> iListener_options::make_listener() const // declared in listener_options.hpp
{
  return std::make_unique<iListener>(this);
}

} // namespace dmitigr::fcgi::detail

namespace dmitigr::fcgi {

DMITIGR_FCGI_INLINE std::unique_ptr<Listener> Listener::make(const Listener_options* const options)
{
  using detail::iListener;
  return std::make_unique<iListener>(options);
}

} // namespace dmitigr::fcgi
