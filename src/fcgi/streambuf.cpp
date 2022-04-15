// -*- C++ -*-
//
// Copyright 2022 Dmitry Igrishin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "basics.hpp"
#include "exceptions.hpp"
#include "server_connection.hpp"
#include "streambuf.hpp"
#include "../base/assert.hpp"
#include "../math/alignment.hpp"

#include <algorithm>
#include <array>
#include <iostream>
#include <limits>

/*
 * By defining DMITIGR_FCGI_DEBUG some convenient stuff for debugging
 * will be available, for example, server_Streambuf::print().
 */
//#define DMITIGR_FCGI_DEBUG
#ifdef DMITIGR_FCGI_DEBUG
#include <iostream>
#endif

namespace dmitigr::fcgi::detail {

/**
 * @brief The base implementation of Streambuf.
 */
class iStreambuf : public Streambuf {
private:
  friend server_Streambuf;

  using Streambuf::Streambuf;
};

/**
 * @brief The implementation of the `std::streambuf` for a FastCGI server.
 */
class server_Streambuf final : public iStreambuf {
public:
  using Type = Stream_type;

  ~server_Streambuf() override
  {
    try {
      close();
    } catch (const std::exception& e) {
      std::clog << "error upon closing FastCGI stream buffer: %s\n" << e.what();
    } catch (...) {
      std::clog << "uknown error upon closing FastCGI stream buffer\n";
    }
  }

  server_Streambuf(const server_Streambuf&) = delete;
  server_Streambuf& operator=(const server_Streambuf&) = delete;
  server_Streambuf(server_Streambuf&&) = delete;
  server_Streambuf& operator=(server_Streambuf&&) = delete;

  /**
   * @brief The constructor.
   */
  server_Streambuf(iServer_connection* const connection,
    char_type* const buffer, const std::streamsize buffer_size, const Type type)
    : type_{type}
    , connection_{connection}
  {
    DMITIGR_ASSERT(connection);
    setg(nullptr, nullptr, nullptr);
    setp(nullptr, nullptr);
    setbuf(buffer, buffer_size);
    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @brief Closes the stream.
   *
   * If this instance represents the streambuf of the output stream (either
   * Stream_type::err or Stream_type::out) then transmits the end records to
   * the FastCGI client and sets `is_end_of_stream_` to `true`. (The
   * transmission of the end records takes place if and only if it is not
   * contradicts the protocol.)
   *
   * @par Effects
   * `is_closed()`. Also, unsets both the get area and the put area.
   */
  void close()
  {
    if (is_closed())
      return;

    if (!is_reader()) {
      const auto& inbuf = dynamic_cast<server_Streambuf&>(
        connection_->in().streambuf());
      DMITIGR_ASSERT(inbuf.is_reader() && !inbuf.is_closed());
      const auto role = connection_->role();
      DMITIGR_ASSERT(role == Role::authorizer || inbuf.type_ != Type::params);
      if (role != Role::filter ||
        inbuf.type_ == Type::data || inbuf.unread_content_length_ == 0) {
        is_end_records_must_be_transmitted_ = true;
        sync();
      } else
        throw Exception{"not all FastCGI stdin has been read by Filter"};

      DMITIGR_ASSERT(is_end_of_stream_ && !is_end_records_must_be_transmitted_);
    }

    setg(nullptr, nullptr, nullptr);
    setp(nullptr, nullptr);

    DMITIGR_ASSERT(is_closed());
    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @returns `true` if this instance is for receiving the
   * data from the FastCGI client, or `false` otherwise.
   */
  bool is_reader() const
  {
    return type_ == Type::in || type_ == Type::params || type_ == Type::data;
  }

  /**
   * @returns `true` if this instance is unusable anymore, or `false` otherwise.
   */
  bool is_closed() const
  {
    return is_reader() ? !eback() : !pbase();
  }

  /**
   * @returns `true` if this instance is ready to switching to the filter mode.
   */
  bool is_ready_to_filter_data() const
  {
    return !is_closed() && (connection_->role() == Role::filter) &&
      (type_ == Type::in) && is_end_of_stream_;
  }

  /**
   * @returns The stream type.
   */
  Type stream_type() const
  {
    return type_;
  }

protected:

  // std::streambuf overridings:

  server_Streambuf* setbuf(char_type* const buffer,
    const std::streamsize size) override
  {
    DMITIGR_ASSERT(buffer && (2048 <= size && size <= 65528));

    if ((eback() != nullptr && eback() != buffer_) ||
        (pbase() != nullptr && pbase() != buffer_ + sizeof(detail::Header)))
      throw Exception{"cannot set FastCGI buffer (there are pending data)"};

    constexpr std::streamsize alignment = 8;
    buffer_ = buffer;
    buffer_size_ = size - (alignment - math::padding(size, alignment)) % alignment;

    if (is_reader()) {
      setg(buffer_, buffer_, buffer_);
      buffer_end_ = buffer_;
      setp(nullptr, nullptr);
    } else {
      /*
       * First sizeof(detail::Header) bytes of the buffer_ are reserved for Header.
       * Last byte of the buffer_ is reserved for byte passed to overflow(). Thus,
       * epptr() can be used to store this byte.
       */
      setg(nullptr, nullptr, nullptr);
      setp(buffer_ + sizeof(detail::Header), buffer_ + buffer_size_ - 1);
    }

    DMITIGR_ASSERT(is_invariant_ok());

    return this;
  }

  int sync() override
  {
    const auto ch = overflow(traits_type::eof());
    return traits_type::eq_int_type(ch, traits_type::eof()) ? -1 : 0;
  }

  int_type underflow() override
  {
    DMITIGR_ASSERT(is_reader() && !is_closed());

    if (is_end_of_stream_)
      return traits_type::eof();

    detail::Header header{};
    std::size_t read_header_length{};
    while (true) {
      // Reading the stream records.
      if (gptr() == buffer_end_) {
        const std::streamsize count = connection_->io_->read(buffer_, buffer_size_);
        if (count > 0) {
          buffer_end_ = buffer_ + count;
          setg(buffer_, buffer_, buffer_end_);
        } else
          throw Exception{"FastCGI protocol violation"};
      }

      DMITIGR_ASSERT(buffer_end_ - gptr() > 0);

      // Setting up the get area for the content.
      if (unread_content_length_ > 0) {
        const std::streamsize count = std::min(unread_content_length_,
          buffer_end_ - gptr());
        unread_content_length_ -= count;
        if (!is_content_must_be_discarded_) {
          // Get area now contains all the available content bytes.
          setg(gptr(), gptr(), gptr() + count);
          DMITIGR_ASSERT(is_invariant_ok());
          return traits_type::to_int_type(*gptr());
        } else {
          gbump(static_cast<int>(count)); // Discarding.
          if (unread_content_length_ > 0)
            continue;
          else
            is_content_must_be_discarded_ = false;
        }
      }

      DMITIGR_ASSERT(unread_content_length_ == 0);

      // Skipping the padding.
      if (unread_padding_length_ > 0) {
        const std::streamsize count = std::min(unread_padding_length_,
          buffer_end_ - gptr());
        unread_padding_length_ -= count;
        gbump(static_cast<int>(count)); // Skipping.
        if (unread_padding_length_ > 0)
          continue;
      }

      DMITIGR_ASSERT(unread_padding_length_ == 0);

      // ---------------
      // The start point
      // ---------------

      // Accumulating the header.
      {
        const std::size_t count = std::min(sizeof(header) - read_header_length,
          static_cast<std::size_t>(buffer_end_ - gptr()));
        std::memcpy(reinterpret_cast<char*>(&header) + read_header_length,
          gptr(), count);
        read_header_length += count;
        gbump(static_cast<int>(count)); // Already consumed.
        if (read_header_length < sizeof(header))
          continue;
      }

      DMITIGR_ASSERT(read_header_length == sizeof(header));

      // Processing the header.
      {
        const auto set_end_of_stream = [&]()
        {
          setg(gptr(), gptr(), gptr());
          is_end_of_stream_ = true;
          DMITIGR_ASSERT(is_invariant_ok());
        };

        setg(gptr(), gptr(), gptr());
        const auto phr = process_header(header);
        switch(phr) {
        case Process_header_result::request_rejected:
          set_end_of_stream();
          return traits_type::eof();

        case Process_header_result::management_processed:
          continue;

        case Process_header_result::content_must_be_consumed:
          if (unread_content_length_ == 0) {
            set_end_of_stream();
            if (is_ready_to_filter_data())
              reset_reader(Type::data);

            return traits_type::eof();
          } else
            continue;

        case Process_header_result::content_must_be_discarded:
          is_content_must_be_discarded_ = true;
          continue;
        }
      }
    }
  }

  int_type overflow(const int_type ch) override
  {
    DMITIGR_ASSERT(!is_reader() && !is_closed());

    if (is_end_of_stream_)
      return traits_type::eof();

    const bool is_eof = traits_type::eq_int_type(ch, traits_type::eof());

    DMITIGR_ASSERT(pbase() == (buffer_ + sizeof(detail::Header)));
    if (std::streamsize content_length = pptr() - pbase()) {
      /*
       * If `ch` is not EOF we need to place `ch` at the location pointed to by
       * pptr(). (It's ok if pptr() == epptr() since that location is a valid
       * writable location reserved for extra `ch`.)
       * The content should be aligned by padding if necessary, and the record
       * header must be injected at the reserved space [buffer_, pbase()).
       * After that the result record will be ready to send to a client.
       */

      // Store `ch` if it's not EOF.
      if (!is_eof) {
        DMITIGR_ASSERT(pptr() <= epptr());
        *pptr() = static_cast<char>(ch);
        pbump(1); // Yes, pptr() > epptr() is possible here, but this is ok.
        content_length++;
      }

      // Aligning the content by padding if necessary.
      const auto padding_length =
        dmitigr::math::padding<std::streamsize>(content_length, 8);
      DMITIGR_ASSERT(padding_length <= epptr() - pptr() + 1);
      std::memset(pptr(), 0, static_cast<std::size_t>(padding_length));
      pbump(static_cast<int>(padding_length));

      // Injecting the header.
      auto* const header = reinterpret_cast<detail::Header*>(buffer_);
      *header = detail::Header{static_cast<detail::Record_type>(type_),
        connection_->request_id(),
        static_cast<std::size_t>(content_length),
        static_cast<std::size_t>(padding_length)};

      // Sending the record.
      if (const auto record_size = pptr() - buffer_;
        static_cast<std::size_t>(record_size) > sizeof(detail::Header)) {
        const std::streamsize count = connection_->io_->write(static_cast<const char*>(buffer_), record_size);
        DMITIGR_ASSERT(count == record_size);
        is_put_area_at_least_once_consumed_ = true;
      }
    }
    setp(buffer_ + sizeof(detail::Header), buffer_ + buffer_size_ - 1);

    if (is_end_records_must_be_transmitted_) {
      /*
       * We'll use buffer_ directly here. (Space before pbase() will be used.)
       * data_size is a size of data in the buffer_ to send.
       */
      std::streamsize data_size{};

      const auto is_empty = [this]()
      {
        return pptr() == pbase() && !is_put_area_at_least_once_consumed_;
      };

      if (type_ != Type::err || !is_empty()) {
        /*
         * When transmitting a stream other than stderr, at least one record of
         * the stream type must be trasmitted, even if the stream is empty.
         * When transmitting a stream of type stderr and there is no errors to
         * report, either no stderr records or one zero-length stderr record
         * must be transmitted. (As optimization, no stderr records are
         * transmitted if the stream is empty.)
         */
        auto* const header = reinterpret_cast<detail::Header*>(buffer_ + data_size);
        *header = detail::Header{static_cast<detail::Record_type>(type_),
          connection_->request_id(), 0, 0};
        data_size += sizeof(detail::Header);
      }

      /*
       * Assume that the stream of type `out` is closes last. (This must be
       * guaranteed by the implementation of Listener.)
       */
      if (type_ == Type::out) {
        auto* const record = reinterpret_cast<detail::End_request_record*>(
          buffer_ + data_size);
        *record = detail::End_request_record{
          connection_->request_id(),
          connection_->application_status(),
          detail::Protocol_status::request_complete};
        data_size += sizeof(detail::End_request_record);
      }

      if (data_size > 0) {
        const std::streamsize count = connection_->io_->write(
          static_cast<const char*>(buffer_), data_size);
        DMITIGR_ASSERT(count == data_size);
      }

      is_end_records_must_be_transmitted_ = false;
      is_end_of_stream_ = true;
    }

    DMITIGR_ASSERT(is_invariant_ok());

    return is_eof ? traits_type::not_eof(ch) : ch;
  }

private:
  friend server_Istream;

  /**
   * @brief A result of process_header().
   */
  enum class Process_header_result {
    /** A request is rejected. */
    request_rejected,
    /** A management record processed. */
    management_processed,
    /** A content from a client must be consumed. */
    content_must_be_consumed,
    /** A content from a client must be discarded. */
    content_must_be_discarded
  };

  Type type_{};
  bool is_content_must_be_discarded_{};
  bool is_end_of_stream_{};
  bool is_end_records_must_be_transmitted_{};
  bool is_put_area_at_least_once_consumed_{};
  char_type* buffer_{};
  char_type* buffer_end_{}; // Used by underflow() to mark the actual end of get area. (buffer_end_ <= buffer_ + buffer_size_).
  std::streamsize buffer_size_{}; // The available size of the area pointed by buffer_.
  std::streamsize unread_content_length_{};
  std::streamsize unread_padding_length_{};
  iServer_connection* const connection_{};

  // ===========================================================================

  bool is_invariant_ok() const
  {
    const bool connection_ok = connection_;
    const bool buffer_ok = buffer_ &&
      (!is_reader() || (buffer_end_ && (buffer_end_ <= buffer_ + buffer_size_)));
    const bool buffer_size_ok = (buffer_size_ >= 2048) &&
      (buffer_size_ <= 65528) && (buffer_size_ % 8 == 0);
    const bool unread_content_length_ok = (unread_content_length_ <= buffer_size_ &&
      unread_content_length_ <= static_cast<std::streamsize>(
        detail::Header::max_content_length));
    const bool unread_padding_length_ok = (unread_padding_length_ <= buffer_size_ &&
      unread_padding_length_ <= static_cast<std::streamsize>(
        detail::Header::max_padding_length));
    const bool reader_ok = (!is_reader() ||
      (type_ == Type::params) ||
      (connection_->role() == Role{0}) || // unread yet
      (connection_->role() == Role::responder &&
        type_ == Type::in) ||
      (connection_->role() == Role::filter &&
        (type_ == Type::in || type_ == Type::data)));
    const bool closed_ok = (is_closed() && (is_reader() || is_end_of_stream_) &&
      (!eback() && !gptr() && !egptr()) && (!pbase() && !pptr() && !epptr()))
      ||
      (is_reader() &&
        !(!eback() && !gptr() && !egptr()) && (!pbase() && !pptr() && !epptr()))
      ||
      (!is_reader() &&
        (!eback() && !gptr() && !egptr()) && !(!pbase() && !pptr() && !epptr()));
    const bool put_area_ok = is_reader() ||
      (is_closed() ||
        ((pbase() <= pptr() && pptr() <= epptr()) &&
          (is_end_of_stream_ || (pbase() == buffer_ + sizeof(detail::Header)))));
    const bool get_area_ok = !is_reader() ||
      (is_closed() ||
        (eback() <= gptr() && gptr() <= egptr() && egptr() <= buffer_end_));

    const bool result = connection_ok && buffer_ok && buffer_size_ok &&
      unread_content_length_ok && unread_padding_length_ok &&
      reader_ok & closed_ok && put_area_ok && get_area_ok;

    return result;
  }

  // ===========================================================================

  /**
   * @brief Processes a record by the header info.
   *
   *   - (1) If `header` is the header of begin-request record then rejecting
   *     the request;
   *   - (2) If `header` is the header of management record, then responds with
   *     get-values-result or unknown-type record;
   *   - (3) If `header` is the header of stream record then do nothing.
   *
   * @returns
   * In case (1): Process_header_result::request_rejected.
   * In case (2): Process_header_result::management_processed.
   * In case (3):
   *   a) Process_header_result::content_must_be_discarded
   *      if `(header.request_id() != connection_->request_id())`;
   *   b) Process_header_result::content_must_be_consumed
   *      if and only if `(header.record_type() == type_)`.
   *
   * @throws Exception in case of (1) or on protocol violation.
   *
   * @par Effects
   * In all cases: `(unread_content_length_ == header.content_length()) &&
   * (unread_padding_length_ == header.padding_length())`.
   * In case (2): the effects of underflow().
   */
  Process_header_result process_header(const detail::Header header)
  {
    header.check_validity();
    unread_content_length_ = static_cast<decltype(unread_content_length_)>(
      header.content_length());
    unread_padding_length_ = static_cast<decltype(unread_content_length_)>(
      header.padding_length());

    const auto end_request = [&](const detail::Protocol_status protocol_status)
    {
      const detail::End_request_record record{header.request_id(), 0, protocol_status};
      const auto count = connection_->io_->write(
        reinterpret_cast<const char*>(&record), sizeof(record));
      DMITIGR_ASSERT(count == sizeof(record));
    };

    // Called in cases of protocol violation.
    const auto end_request_protocol_violation = [&]()
    {
      end_request(detail::Protocol_status::cant_mpx_conn);
      throw Exception{"FastCGI protocol violation"};
    };

    const auto process_management_record = [&]()
    {
      namespace math = dmitigr::math;
      if (header.record_type() == detail::Record_type::get_values) {
        // The length of "FCGI_MPXS_CONNS" == 15.
        constexpr std::size_t max_variable_name_length{15};
        constexpr std::size_t max_body_length{3 * (sizeof(char) + sizeof(char)
          + max_variable_name_length + sizeof(char))};
        constexpr std::streamsize max_record_length{sizeof(detail::Header)
          + max_body_length};
        std::array<unsigned char,
          math::aligned<std::size_t>(max_record_length, 8)> record;
        static_assert((record.size() % 8 == 0));

        // Reading the requested variables.
        const auto variables = [&]()
        {
          std::istream stream{this};
          return detail::Names_values{stream, 3};
        }();
        if (unread_content_length_ > 0)
          end_request_protocol_violation();

        // Filling up the content of get-values-result.
        const auto content_offset = std::begin(record) + sizeof(detail::Header);
        auto p = content_offset;
        const auto variables_count = variables.pair_count();
        for (std::size_t i = 0; i < variables_count; ++i) {
          const auto name = variables.pair(i).name();
          char value{};
          if (name == "FCGI_MAX_CONNS")
            value = '1';
          else if (name == "FCGI_MAX_REQS")
            value = '1';
          else if (name == "FCGI_MPXS_CONNS")
            value = '0';
          else
            continue; // Ignoring other variables specified in the get-values record.

          const auto name_size = name.size();
          if (name_size > std::numeric_limits<unsigned char>::max())
            end_request_protocol_violation();

          // Note: name_size + 3 - is a space required to store: {char(name_size), char(1), name.data(), char(value)} string.
          DMITIGR_ASSERT(name_size + 3 <= static_cast<std::size_t>(
              std::cend(record) - p));
          *p++ = static_cast<unsigned char>(name_size); // name_size
          *p++ = static_cast<unsigned char>(1);         // 1
          p = std::copy(cbegin(name), cend(name), p);   // name.data()
          *p++ = static_cast<unsigned char>(value);     // value
        }

        using detail::Header;
        const auto content_length = p - content_offset;
        const auto padding_length = math::padding<std::streamsize>(content_length, 8);
        const auto record_length = static_cast<int>(sizeof(Header)) +
          content_length + padding_length;
        auto* const h = reinterpret_cast<Header*>(record.data());
        *h = detail::Header{detail::Record_type::get_values_result,
          Header::null_request_id, static_cast<std::size_t>(content_length),
          static_cast<std::size_t>(padding_length)};
        const auto count = connection_->io_->write(
          reinterpret_cast<const char*>(record.data()), record_length);
        DMITIGR_ASSERT(count == record_length);
      } else {
        const detail::Unknown_type_record r{header.record_type()};
        const std::streamsize record_length = sizeof(r);
        const auto count = connection_->io_->write(
          reinterpret_cast<const char*>(&r), record_length);
        DMITIGR_ASSERT(count == record_length);
      }

      return Process_header_result::management_processed;
    };

    Process_header_result result{};
    if (header.record_type() == detail::Record_type::begin_request) {
      end_request(detail::Protocol_status::cant_mpx_conn);
      result = Process_header_result::content_must_be_discarded;
    } else if (header.is_management_record())
      result = process_management_record();
    else if (header.request_id() != connection_->request_id())
      result = Process_header_result::content_must_be_discarded;
    else if (header.record_type() == static_cast<detail::Record_type>(type_))
      result = Process_header_result::content_must_be_consumed;
    else
      end_request_protocol_violation();

    DMITIGR_ASSERT(is_invariant_ok());

    return result;
  }

  /**
   * @brief Resets the input stream to read the data of the specified type.
   *
   * @par Requires
   * `is_reader()`.
   */
  void reset_reader(const Type type)
  {
    DMITIGR_ASSERT(is_reader() && !is_closed());
    type_ = type;
    is_end_of_stream_ = false;
    is_content_must_be_discarded_ = false;
    unread_content_length_ = 0;
    unread_padding_length_ = 0;
    DMITIGR_ASSERT(is_invariant_ok());
  }

#ifdef DMITIGR_FCGI_DEBUG
  template<typename ... Types>
  void print(Types&& ... msgs) const
  {
    if (type_ == Stream_type::out) {
      ((std::cerr << std::forward<Types>(msgs) << " "), ...);
      std::cerr << std::endl;
    }
  }
#endif
};

} // namespace dmitigr::fcgi::detail
