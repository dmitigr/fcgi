# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

set(dmitigr_fcgi_tests hello hellomt overload)
if(UNIX)
  set(dmitigr_fcgi_tests_target_link_libraries pthread)
endif()
dmitigr_propagate_tests_settings(fcgi)
