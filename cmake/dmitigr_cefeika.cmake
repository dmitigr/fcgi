# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

set(dmitigr_cefeika_libraries
  base

  math
  os
  testo

  net

  fcgi
  )

set(dmitigr_cefeika_base_deps)

set(dmitigr_cefeika_math_deps base)
set(dmitigr_cefeika_os_deps base)
set(dmitigr_cefeika_testo_deps base)

set(dmitigr_cefeika_net_deps os base)

set(dmitigr_cefeika_fcgi_deps net math base)
