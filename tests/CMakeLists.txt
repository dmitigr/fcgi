# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see files LICENSE.txt or fcgi.hpp

if(NOT DMITIGR_FCGI_BUILD_TESTS)
  return()
endif()

remove_definitions(-DDMITIGR_FCGI_DLL_BUILDING)

foreach(t hello hellomt unit1)
  add_executable(${t} ${t}.cpp)
  target_link_libraries(${t} ${dmitigr_fcgi_target} ${dmitigr_common_library})
  if (UNIX)
    target_link_libraries(${t} pthread)
  endif()
  dmitigr_target_compile_options(${t})
  add_test(NAME ${t} COMMAND ${t})
endforeach()

if (WIN32)
  set(dmitigr_fcgi_resource_destination_dir "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>")
elseif (UNIX)
  set(dmitigr_fcgi_resource_destination_dir "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endif()
