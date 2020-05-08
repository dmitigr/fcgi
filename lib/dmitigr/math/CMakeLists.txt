# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_set_library_info(math 0 1 "Mathematical stuff")

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_math_headers
  math.hpp
  )

set(dmitigr_math_implementations
  )

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

set(dmitigr_math_target_link_libraries_public dmitigr::base)
set(dmitigr_math_target_link_libraries_interface dmitigr::base)

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(math)
