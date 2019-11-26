// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or fcgi.hpp

#ifndef DMITIGR_FCGI_DLL_HPP
#define DMITIGR_FCGI_DLL_HPP

#ifdef _WIN32
  #ifdef DMITIGR_CEFEIKA_DLL_BUILDING
    #define DMITIGR_FCGI_API __declspec(dllexport)
  #else
    #if DMITIGR_FCGI_DLL
      #define DMITIGR_FCGI_API __declspec(dllimport)
    #else /* static or header-only library on Windows */
      #define DMITIGR_FCGI_API
    #endif
  #endif
#else /* Unix */
  #define DMITIGR_FCGI_API
#endif

#endif // DMITIGR_FCGI_DLL_HPP
