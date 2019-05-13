// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or fcgi.hpp

#ifndef DMITIGR_FCGI_INLINE
  #if defined(DMITIGR_FCGI_HEADER_ONLY) && !defined(DMITIGR_FCGI_BUILDING)
    #define DMITIGR_FCGI_INLINE inline
  #else
    #define DMITIGR_FCGI_INLINE
  #endif
#endif  // DMITIGR_FCGI_INLINE

#ifndef DMITIGR_FCGI_NOMINMAX
  #ifdef _WIN32
    #ifndef NOMINMAX
      #define NOMINMAX
      #define DMITIGR_FCGI_NOMINMAX
    #endif
  #endif
#endif  // DMITIGR_FCGI_NOMINMAX
