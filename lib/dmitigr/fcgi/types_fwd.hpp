// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or fcgi.hpp

#ifndef DMITIGR_FCGI_TYPES_FWD_HPP
#define DMITIGR_FCGI_TYPES_FWD_HPP

/**
 * @brief Public API.
 *
 * @warning The nested namespaces `detail` contains implementation details
 * which should not be used in the client code.
 */
namespace dmitigr::fcgi {

enum class Role;
enum class Stream_type;

class Listener;
class iListener;

class Listener_options;
class iListener_options;

class Connection;
class Server_connection;
class iServer_connection;

class Streambuf;
class iStreambuf;
class server_Streambuf;

class Stream;
class Istream;
class iIstream;
class server_Istream;
class Ostream;
class iOstream;
class server_Ostream;

/**
 * @brief The namespace dmitigr::fcgi::detail contains an implementation details.
 */
namespace detail {
} // namespace detail

} // namespace dmitigr::fcgi

namespace dmitigr::net {
class Endpoint_id;
} // namespace dmitigr::net

#endif  // DMITIGR_FCGI_TYPES_FWD_HPP
