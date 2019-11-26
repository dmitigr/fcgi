// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or fcgi.hpp

#ifndef DMITIGR_FCGI_TYPES_FWD_HPP
#define DMITIGR_FCGI_TYPES_FWD_HPP

/**
 * @brief The API.
 */
namespace dmitigr::fcgi {

enum class Role;
enum class Stream_type;

class Listener;
class Listener_options;

class Connection_parameter;
class Connection;
class Server_connection;

class Streambuf;

class Stream;
class Istream;
class Ostream;

/**
 * @brief The implementation details.
 */
namespace detail {
class iListener;
class iListener_options;
class iServer_connection;
class iStreambuf;
class server_Streambuf;
class iIstream;
class server_Istream;
class iOstream;
class server_Ostream;

class Name_value;
class Names_values;
} // namespace detail

} // namespace dmitigr::fcgi

#endif  // DMITIGR_FCGI_TYPES_FWD_HPP
