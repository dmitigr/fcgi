// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or fcgi.hpp

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// This file is generated automatically. Edit dll.hpp.in instead!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#ifndef DMITIGR_FCGI_DLL_HPP
#define DMITIGR_FCGI_DLL_HPP

#ifdef _WIN32
  #ifdef DMITIGR_FCGI_DLL_BUILDING
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

#ifndef DMITIGR_FCGI_INLINE
  #if defined(DMITIGR_FCGI_HEADER_ONLY) && !defined(DMITIGR_FCGI_BUILDING)
    #define DMITIGR_FCGI_INLINE inline
  #else
    #define DMITIGR_FCGI_INLINE
  #endif
#endif  // DMITIGR_FCGI_INLINE

#endif // DMITIGR_FCGI_DLL_HPP
