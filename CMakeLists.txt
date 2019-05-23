# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see files LICENSE.txt or fcgi.hpp

project(dmitigr_fcgi)

cmake_minimum_required(VERSION 3.13)
cmake_policy(VERSION 3.13)

if (NOT (UNIX OR WIN32))
  message(FATAL_ERROR "unsupported target platform")
endif()

list(APPEND CMAKE_MODULE_PATH ${dmitigr_fcgi_SOURCE_DIR}/cmake)
include(dmitigr)

set(dmitigr_fcgi_version_part1 0)
set(dmitigr_fcgi_version_part2 1)
add_definitions(-DDMITIGR_FCGI_VERSION_PART1=${dmitigr_fcgi_version_part1})
add_definitions(-DDMITIGR_FCGI_VERSION_PART2=${dmitigr_fcgi_version_part2})

# ------------------------------------------------------------------------------
# Build options
# ------------------------------------------------------------------------------

option(BUILD_SHARED_LIBS "Build shared library?" ON)
option(DMITIGR_FCGI_HEADER_ONLY "Header-only library?" OFF)
option(DMITIGR_FCGI_BUILD_TESTS "Build test?" ON)

if(NOT DMITIGR_FCGI_HEADER_ONLY)
  if(BUILD_SHARED_LIBS)
    message("Building of shared library is enabled.")
    add_definitions(-DDMITIGR_FCGI_DLL_BUILDING)
  else()
    message("Building of static library is enabled.")
  endif()

  if(NOT DEFINED CMAKE_BUILD_TYPE OR NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING
      "Build type: Debug Release RelWithDebInfo MinSizeRel." FORCE)
  endif()
  message("Build type is ${CMAKE_BUILD_TYPE}")
else()
  message("Header-only mode is enabled")
endif()

if(DMITIGR_FCGI_BUILD_TESTS)
  enable_testing()
  message("Building of tests is enabled")
endif()

# ------------------------------------------------------------------------------
# Installation options
# ------------------------------------------------------------------------------

if(UNIX)
  set(DMITIGR_FCGI_CMAKE_INSTALL_DIR "share/dmitigr_fcgi/cmake" CACHE
    STRING "Name of the installation directory for the CMake stuff relative to ${CMAKE_INSTALL_PREFIX}")
  set(DMITIGR_FCGI_DOC_INSTALL_DIR "share/dmitigr_fcgi/doc" CACHE
    STRING "Name of the installation directory for the documentation relative to ${CMAKE_INSTALL_PREFIX}")
  set(DMITIGR_FCGI_LIB_INSTALL_DIR "lib" CACHE
    STRING "Name of the installation directory for the libraries relative to ${CMAKE_INSTALL_PREFIX}")
  set(DMITIGR_FCGI_INCLUDE_INSTALL_DIR "include" CACHE
    STRING "Name of the installation directory for the includes relative to ${CMAKE_INSTALL_PREFIX}")
elseif(WIN32)
  # On Windows, CMAKE_INSTALL_PREFIX is $ENV{ProgramFiles}\\${CMAKE_PROJECT_NAME} by default. In turn:
  #   - on AMD64: ProgramFiles=%ProgramFiles%
  #   - on   x86: ProgramFiles=%ProgramFiles(x86)%
  # See: https://msdn.microsoft.com/en-us/library/aa384274.aspx
  set(DMITIGR_FCGI_CMAKE_INSTALL_DIR "cmake" CACHE
    STRING "Name of share directory relative to ${CMAKE_INSTALL_PREFIX}")
  set(DMITIGR_FCGI_DOC_INSTALL_DIR "doc" CACHE
    STRING "Name of the documentation directory relative to ${CMAKE_INSTALL_PREFIX}")
  set(DMITIGR_FCGI_LIB_INSTALL_DIR "lib" CACHE
    STRING "Name of the library directory relative to ${CMAKE_INSTALL_PREFIX}")
  set(DMITIGR_FCGI_INCLUDE_INSTALL_DIR "include" CACHE
    STRING "Name of includes directory relative to ${CMAKE_INSTALL_PREFIX}")
endif()

# ------------------------------------------------------------------------------
# Languages
# ------------------------------------------------------------------------------

enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)

include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/lib)
include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR}/lib)

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_fcgi_root_headers
  lib/dmitigr/fcgi.hpp
  )

set(dmitigr_fcgi_header_only_headers
  lib/dmitigr/fcgi/implementation_footer.hpp
  lib/dmitigr/fcgi/implementation_header.hpp
  )

set(dmitigr_fcgi_headers
  lib/dmitigr/fcgi/basics.hpp
  lib/dmitigr/fcgi/connection.hpp
  lib/dmitigr/fcgi/dll.hpp
  lib/dmitigr/fcgi/listener.hpp
  lib/dmitigr/fcgi/listener_options.hpp
  lib/dmitigr/fcgi/server_connection.hpp
  lib/dmitigr/fcgi/streambuf.hpp
  lib/dmitigr/fcgi/streams.hpp
  lib/dmitigr/fcgi/types_fwd.hpp
  )

set(dmitigr_fcgi_implementations
  lib/dmitigr/fcgi/basics.cpp
  lib/dmitigr/fcgi/listener.cpp
  lib/dmitigr/fcgi/listener_options.cpp
  lib/dmitigr/fcgi/server_connection.cpp
  lib/dmitigr/fcgi/streambuf.cpp
  lib/dmitigr/fcgi/streams.cpp
  )

set(dmitigr_fcgi_transunits
  lib/dmitigr/fcgi.cpp
  )

set(dmitigr_fcgi_sources
  ${dmitigr_fcgi_root_headers}
  ${dmitigr_fcgi_header_only_headers}
  ${dmitigr_fcgi_headers}
  ${dmitigr_fcgi_implementations}
  ${dmitigr_fcgi_transunits}
  )

set_source_files_properties(
  ${dmitigr_fcgi_implementations}

  PROPERTIES
  HEADER_FILE_ONLY ON)

# ------------------------------------------------------------------------------
# Targets
# ------------------------------------------------------------------------------

# Note: Multi-configuration generators (VS, Xcode) appends a per-configuration
# subdirectory to CMAKE_RUNTIME_OUTPUT_DIRECTORY unless a generator expression
# is used.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

if(NOT DMITIGR_FCGI_HEADER_ONLY)
  if(BUILD_SHARED_LIBS)
    set(dmitigr_fcgi_target "dmitigr_fcgi")
  else()
    set(dmitigr_fcgi_target "dmitigr_fcgi_static")
  endif()

  add_library(${dmitigr_fcgi_target} ${dmitigr_fcgi_sources})

  if(BUILD_SHARED_LIBS)
    target_compile_definitions(dmitigr_fcgi PUBLIC -DDMITIGR_FCGI_DLL)
  endif()

  set_target_properties(${dmitigr_fcgi_target}
    PROPERTIES
    LINKER_LANGUAGE "CXX"
    POSITION_INDEPENDENT_CODE True
    VERSION ${dmitigr_fcgi_version_part1}.${dmitigr_fcgi_version_part2}
    DEBUG_POSTFIX "d")

  dmitigr_target_compile_options(${dmitigr_fcgi_target})
else() # for header-only
  set(dmitigr_fcgi_target "dmitigr_fcgi_interface")

  add_library(dmitigr_fcgi_interface INTERFACE)

  target_compile_definitions(dmitigr_fcgi_interface INTERFACE -DDMITIGR_FCGI_HEADER_ONLY)
endif()

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

#
# dmitigr_common
#

find_package(dmitigr_common REQUIRED)
message("Using ${dmitigr_common_library} library")

# --------------------------------------

if(NOT DMITIGR_FCGI_HEADER_ONLY)
  target_link_libraries(${dmitigr_fcgi_target} PRIVATE ${dmitigr_common_library})
else()
  target_link_libraries(dmitigr_fcgi_interface INTERFACE ${dmitigr_common_library})
endif()

# ------------------------------------------------------------------------------
# Installing
# ------------------------------------------------------------------------------

install(FILES ${dmitigr_fcgi_root_headers}
  DESTINATION ${DMITIGR_FCGI_INCLUDE_INSTALL_DIR}/dmitigr)

if(DMITIGR_FCGI_HEADER_ONLY)
  install(FILES ${dmitigr_fcgi_header_only_headers}
    DESTINATION ${DMITIGR_FCGI_INCLUDE_INSTALL_DIR}/dmitigr/fcgi)
endif()

install(FILES ${dmitigr_fcgi_headers}
  DESTINATION ${DMITIGR_FCGI_INCLUDE_INSTALL_DIR}/dmitigr/fcgi)

if(NOT DMITIGR_FCGI_HEADER_ONLY)
  install(TARGETS ${dmitigr_fcgi_target}
    EXPORT ${dmitigr_fcgi_target}_export
    ARCHIVE  DESTINATION ${DMITIGR_FCGI_LIB_INSTALL_DIR}
    LIBRARY  DESTINATION ${DMITIGR_FCGI_LIB_INSTALL_DIR}
    RUNTIME  DESTINATION ${DMITIGR_FCGI_LIB_INSTALL_DIR}
    INCLUDES DESTINATION ${DMITIGR_FCGI_INCLUDE_INSTALL_DIR})
else()
  install(TARGETS dmitigr_fcgi_interface
    EXPORT dmitigr_fcgi_interface_export
    INCLUDES DESTINATION ${DMITIGR_FCGI_INCLUDE_INSTALL_DIR})

  install(FILES ${dmitigr_fcgi_implementations}
    DESTINATION ${DMITIGR_FCGI_INCLUDE_INSTALL_DIR}/dmitigr/fcgi)
endif()

install(EXPORT ${dmitigr_fcgi_target}_export
  DESTINATION ${DMITIGR_FCGI_CMAKE_INSTALL_DIR}
  FILE ${dmitigr_fcgi_target}-config.cmake)

# ------------------------------------------------------------------------------
# Uninstalling
# ------------------------------------------------------------------------------

add_custom_target(uninstall)

add_custom_command(TARGET uninstall PRE_BUILD
  COMMAND cmake -E remove -f ${CMAKE_INSTALL_PREFIX}/${DMITIGR_FCGI_CMAKE_INSTALL_DIR}/dmitigr_fcgi*
  COMMAND cmake -E remove -f ${CMAKE_INSTALL_PREFIX}/${DMITIGR_FCGI_DOC_INSTALL_DIR}/dmitigr_fcgi*
  COMMAND cmake -E remove -f ${CMAKE_INSTALL_PREFIX}/${DMITIGR_FCGI_LIB_INSTALL_DIR}/dmitigr_fcgi*
  COMMAND cmake -E remove -f ${CMAKE_INSTALL_PREFIX}/${DMITIGR_FCGI_LIB_INSTALL_DIR}/libdmitigr_fcgi*
  COMMAND cmake -E remove -f ${CMAKE_INSTALL_PREFIX}/${DMITIGR_FCGI_INCLUDE_INSTALL_DIR}/dmitigr/fcgi*
  COMMAND cmake -E remove_directory ${CMAKE_INSTALL_PREFIX}/${DMITIGR_FCGI_INCLUDE_INSTALL_DIR}/dmitigr/fcgi
  )

# ------------------------------------------------------------------------------

if(DMITIGR_FCGI_BUILD_TESTS)
  add_subdirectory(tests)
endif()
