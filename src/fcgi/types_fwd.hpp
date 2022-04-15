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

#ifndef DMITIGR_FCGI_TYPES_FWD_HPP
#define DMITIGR_FCGI_TYPES_FWD_HPP

/// The API.
namespace dmitigr::fcgi {

enum class Role;
enum class Stream_type;

class Exception;

class Listener;
class Listener_options;

class Connection_parameter;
class Connection;
class Server_connection;

class Streambuf;

class Stream;
class Istream;
class Ostream;

/// The implementation details.
namespace detail {
enum class Record_type : unsigned char;
enum class Protocol_status : unsigned char;
struct Header;
struct Begin_request_body;
struct End_request_body;
struct End_request_record;
struct Unknown_type_body;
struct Unknown_type_record;
class Name_value;
class Names_values;

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
