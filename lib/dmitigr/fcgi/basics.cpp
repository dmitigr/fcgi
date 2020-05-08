// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or fcgi.hpp

#include "dmitigr/fcgi/basics.hpp"
#include "dmitigr/fcgi/connection.hpp"
#include <dmitigr/base/debug.hpp>
#include <dmitigr/math/math.hpp>
#include <dmitigr/net/descriptor.hpp>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstring>
#include <istream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace dmitigr::fcgi::detail {

/**
 * @brief A type of record.
 */
enum class Record_type : unsigned char {
  /** A begin request record. */
  begin_request = 1,

  /** An abort request record. */
  abort_request = 2,

  /** An end of request record. */
  end_request = 3,

  /** See Stream_type::params. */
  params = static_cast<unsigned char>(Stream_type::params),

  /** See Stream_type::in. */
  in = static_cast<unsigned char>(Stream_type::in),

  /** See Stream_type::out. */
  out = static_cast<unsigned char>(Stream_type::out),

  /** See Stream_type::err. */
  err = static_cast<unsigned char>(Stream_type::err),

  /** See Stream_type::data. */
  data = static_cast<unsigned char>(Stream_type::data),

  /**
   * A management record that is a query from a HTTP server about
   * specific variables within a FastCGI server (or an application).
   * (The FastCGI server may receive such a query record at any time.)
   */
  get_values = 9,

  /**
   * A response to a get-values management record.
   * (Only known variables can be included to the response.)
   */
  get_values_result = 10,

  /** A response to an unknown management record. */
  unknown_type = 11
};

/**
 * @brief A protocol-level status code.
 */
enum class Protocol_status : unsigned char {
  /** A normal end of request. */
  request_complete = 0,

  /**
   * Rejecting a new request when a HTTP server sends concurrent requests
   * over one connection to a FastCGI server that is designed to process
   * one request at a time per connection.
   */
  cant_mpx_conn = 1,

  /**
   * Rejecting a new request when an application runs out of some
   * resource, e.g. database connections.
   */
  overloaded = 2,

  /**
   * Rejecting a new request when a HTTP server has specified a
   * role that is unknown to a FastCGI server.
   */
  unknown_role = 3
};

/**
 * @brief A FastCGI record header.
 */
struct Header final {
  /**
   * @brief The special ID of a request that used by management records only.
   */
  constexpr static int null_request_id = 0;

  /**
   * @brief The maximum content length.
   */
  constexpr static std::size_t max_content_length = 65535;

  /**
   * @brief The maximum padding length.
   */
  constexpr static std::size_t max_padding_length = 255;

  /**
   * @brief The default constructor.
   */
  Header() = default;

  /**
   * @brief Constructs by reading the header from `io`.
   */
  explicit Header(net::Descriptor* const io)
  {
    DMITIGR_ASSERT(io);

    const std::size_t count = io->read(reinterpret_cast<char*>(this), sizeof(*this));
    if (count != sizeof(*this))
      throw std::runtime_error{"dmitigr::fcgi: protocol violation"};
    check_validity();
  }

  /**
   * @brief The constructor.
   */
  Header(const Record_type record_type, const int request_id, const std::size_t content_len, const std::size_t padding_len)
    : record_type_{static_cast<unsigned char>(record_type)}
    , request_id_b1_{static_cast<unsigned char>((request_id >> 8) & 0xff)}
    , request_id_b0_{static_cast<unsigned char>( request_id       & 0xff)}
    , content_length_b1_{static_cast<unsigned char>((content_len >> 8) & 0xff)}
    , content_length_b0_{static_cast<unsigned char>( content_len       & 0xff)}
    , padding_length_{static_cast<unsigned char>(padding_len)}
  {
    DMITIGR_ASSERT((content_len <= max_content_length) && (padding_len <= max_padding_length));
  }

  /**
   * @brief The constructor.
   */
  Header(const Record_type record_type, const int request_id, const std::size_t content_len)
    : Header{record_type, request_id, content_len, dmitigr::math::padding(content_len, 8)}
  {}

  /**
   * @brief Checks the validity of the header.
   */
  void check_validity() const
  {
    if (protocol_version() != 1)
      throw std::runtime_error{"dmitigr::fcgi: protocol violation"};
  }

  /**
   * @returns The request ID.
   */
  int request_id() const
  {
    return (request_id_b1_ << 8) + request_id_b0_;
  }

  /**
   * @returns The content length.
   */
  std::size_t content_length() const
  {
    return (content_length_b1_ << 8) + content_length_b0_;
  }

  /**
   * @returns The padding length.
   */
  std::size_t padding_length() const
  {
    return padding_length_;
  }

  /**
   * @returns The protocol version.
   */
  int protocol_version() const
  {
    return protocol_version_;
  }

  /**
   * @returns The type of record to which this header belongs.
   */
  Record_type record_type() const
  {
    return Record_type{record_type_};
  }

  /**
   * @returns `true` if this header is represents a header
   * of a management record, or `false` otherwise.
   */
  bool is_management_record() const
  {
    return (request_id() == null_request_id);
  }

private:
  unsigned char protocol_version_{1}; // the only valid value is 1.
  unsigned char record_type_{};
  unsigned char request_id_b1_{};
  unsigned char request_id_b0_{};
  unsigned char content_length_b1_{};
  unsigned char content_length_b0_{};
  unsigned char padding_length_{};
  unsigned char reserved_{};
};

/**
 * @brief A FastCGI begin-request record body.
 */
struct Begin_request_body final {
  /**
   * @brief Control bits.
   */
  enum class Flags : unsigned char {
    /**
     * The bit that instructs a FastCGI server do not close a connection after
     * responding to the request. (A HTTP server retains responsibility for the
     * connection in this case.)
     */
    keep_conn = 1
  };

  /**
   * @brief The default constructor.
   */
  Begin_request_body() = default;

  /**
   * @brief Constructs by reading the record from `io`.
   */
  explicit Begin_request_body(net::Descriptor* const io)
  {
    DMITIGR_ASSERT(io);

    const std::size_t count = io->read(reinterpret_cast<char*>(this), sizeof(*this));
    if (count != sizeof(*this))
      throw std::runtime_error{"dmitigr::fcgi: protocol violation"};
  }

  /**
   * @returns The role.
   */
  Role role() const
  {
    return Role{(role_b1_ << 8) + role_b0_};
  }

  /**
   * @returns `true` if the `Flags::keep_conn` flag is set, or `false` otherwise.
   */
  bool is_keep_conn() const
  {
    return (flags_ & static_cast<unsigned char>(Flags::keep_conn));
  }

private:
  unsigned char role_b1_{};
  unsigned char role_b0_{};
  unsigned char flags_{};
  unsigned char reserved_[5]{0, 0, 0, 0, 0};
};

/**
 * @brief A FastCGI end-request body.
 */
struct End_request_body final {
  /**
   * @brief The default constructor.
   */
  End_request_body() = default;

  /**
   * @brief The constructor.
   */
  End_request_body(const int application_status, const Protocol_status protocol_status)
    : application_status_b3_{static_cast<unsigned char>((application_status >> 24) & 0xff)}
    , application_status_b2_{static_cast<unsigned char>((application_status >> 16) & 0xff)}
    , application_status_b1_{static_cast<unsigned char>((application_status >>  8) & 0xff)}
    , application_status_b0_{static_cast<unsigned char>( application_status        & 0xff)}
    , protocol_status_{static_cast<unsigned char>(protocol_status)}
  {}

private:
  unsigned char application_status_b3_{};
  unsigned char application_status_b2_{};
  unsigned char application_status_b1_{};
  unsigned char application_status_b0_{};
  unsigned char protocol_status_{};
  unsigned char reserved_[3]{0, 0, 0};
};

/**
 * @brief A FastCGI end-request record.
 */
struct End_request_record final {
  /**
   * @brief The constructor.
   */
  End_request_record(const Header header, const End_request_body body)
    : header_{header}
    , body_{body}
  {}

  /**
   * @brief The constructor.
   */
  End_request_record(const int request_id, const int application_status, const Protocol_status protocol_status)
    : header_{Record_type::end_request, request_id, sizeof(body_), 0}
    , body_{application_status, protocol_status}
  {}

private:
  Header header_;
  End_request_body body_;
};

/**
 * @brief A FastCGI unknown-type record body.
 */
struct Unknown_type_body final {
  /**
   * @brief The constructor.
   */
  explicit Unknown_type_body(const Record_type record_type)
    : record_type_{static_cast<unsigned char>(record_type)}
  {}

private:
  unsigned char record_type_;
  unsigned char reserved_[7]{0, 0, 0, 0, 0, 0, 0};
};

/**
 * @brief A FastCGI unknown-type record.
 */
struct Unknown_type_record final {
  /**
   * @brief The constructor.
   */
  explicit Unknown_type_record(const Record_type record_type)
    : header_{detail::Record_type::unknown_type, detail::Header::null_request_id, sizeof(body_), 0}
    , body_{record_type}
  {
    static_assert(sizeof(body_) % 8 == 0);
  }

private:
  Header header_;
  Unknown_type_body body_;
};

// -----------------------------------------------------------------------------
// Names_values
// -----------------------------------------------------------------------------

/**
 * @brief A value type of Names_values container.
 */
class Name_value final : public Connection_parameter {
public:
  /**
   * @brief The constructor.
   */
  Name_value(std::unique_ptr<char[]> data,
    const std::size_t name_size, const std::size_t value_size)
    : data_{std::move(data)}
    , name_size_{name_size}
    , value_size_{value_size}
  {}

  /**
   * @returns The name.
   */
  std::string_view name() const override
  {
    return std::string_view{data_.get(), name_size_};
  }

  /**
   * @returns The value.
   */
  std::string_view value() const override
  {
    return std::string_view{data_.get() + name_size_, value_size_};
  }

private:
  std::unique_ptr<char[]> data_;
  std::size_t name_size_{};
  std::size_t value_size_{};
};

/**
 * @brief A container of name-value pairs to store variable-length values.
 */
class Names_values final {
public:
  /**
   * @brief The default constructor.
   */
  Names_values() = default;

  /**
   * @brief Constructs by reading the given `stream`.
   *
   * @param stream - the stream to read from;
   * @param reserve - the number of name-value pairs for which memory should
   * be allocated at once.
   *
   * @remarks Each name-value pair is transmitted as sequence of:
   *   - the length of the name;
   *   - the length of the value;
   *   - the name;
   *   - the value.
   *
   * @remarks Lengths of 127 bytes and less are encoded in one byte,
   * while longer lengths are always encoded in four bytes.
   */
  explicit Names_values(std::istream& stream, const std::size_t reserve = 0)
  {
    DMITIGR_ASSERT(stream && (reserve <= 64));

    using Traits_type = std::istream::traits_type;

    const auto read_length = [&]() -> int
    {
      // Note: length can be 1 or 4 bytes.
      const auto result = stream.get();
      if (result != Traits_type::eof()) {
        if ((result & 0x80) != 0) {
          std::array<unsigned char, 3> buf;
          stream.read(reinterpret_cast<char*>(buf.data()), buf.size());
          if (stream.gcount() == static_cast<std::streamsize>(buf.size()))
            return ((result & 0x7f) << 24) + (buf[0] << 16) + (buf[1] << 8) + buf[2];
          else
            throw std::runtime_error{"dmitigr::fcgi: read_length() error"};
        }
      }
      return result;
    };

    const auto read_data = [&](const int count) -> std::unique_ptr<char[]>
    {
      std::unique_ptr<char[]> result{new char[count]};
      stream.read(result.get(), count);
      if (stream.gcount() == count)
        return result;
      else
        throw std::runtime_error{"dmitigr::fcgi: read_data() error"};
    };

    pairs_.reserve(reserve);
    while (true) {
      if (const int name_length = read_length(); name_length != Traits_type::eof()) {
        if (const int value_length = read_length(); value_length != Traits_type::eof()) {
          auto data = read_data(name_length + value_length);
          add(std::move(data), name_length, value_length);
        } else
          throw std::runtime_error{"dmitigr::fcgi: protocol violation"};
      } else
        break;
    }
  }

  /**
   * @returns The pair count.
   */
  std::size_t pair_count() const
  {
    return pairs_.size();
  }

  /**
   * @returns The pair index by the given `name`.
   */
  std::optional<std::size_t> pair_index(const std::string_view name) const
  {
    const auto b = cbegin(pairs_);
    const auto e = cend(pairs_);
    const auto i = std::find_if(b, e, [&](const auto& p) { return p.name() == name; });
    return i != e ? std::make_optional<std::size_t>(i - b) : std::nullopt;
  }

  /**
   * @returns `true` if this instance has at least one name-value pair.
   */
  bool has_pairs() const
  {
    return !pairs_.empty();
  }

  /**
   * @returns The pair by the given `index`.
   */
  const Name_value* pair(const std::size_t index) const
  {
    DMITIGR_ASSERT(index < pair_count());
    return &pairs_[index];
  }

  /**
   * @brief Adds the name-value pair.
   *
   * @param data - the name-value data.
   * @param name_size - the size of name in `data`.
   * @param value_size - the size of value in `data`.
   */
  void add(std::unique_ptr<char[]> data, const std::size_t name_size, const std::size_t value_size)
  {
    pairs_.emplace_back(std::move(data), name_size, value_size);
  }

  /**
   * @overload
   */
  void add(const char* const name, const char* const value)
  {
    DMITIGR_ASSERT(name && value);
    const std::size_t name_size = std::strlen(name);
    const std::size_t value_size = std::strlen(value);
    std::unique_ptr<char[]> data{new char[name_size + value_size]};
    add(std::move(data), name_size, value_size);
  }

private:
  std::vector<Name_value> pairs_;
};

} // namespace dmitigr::fcgi::detail
