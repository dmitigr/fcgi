# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

set(dmitigr_cefeika_libraries
  # Abstraction level 1
  util
  # Abstraction level 2
  fs
  math
  # Abstraction level 3
  net
  # Abstraction level 4
  fcgi
  )

set(dmitigr_cefeika_util_deps)
set(dmitigr_cefeika_fs_deps util)
set(dmitigr_cefeika_math_deps util)
set(dmitigr_cefeika_net_deps fs util)
set(dmitigr_cefeika_fcgi_deps net math fs util)
