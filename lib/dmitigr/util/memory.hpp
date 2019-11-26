// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or util.hpp

#ifndef DMITIGR_UTIL_MEMORY_HPP
#define DMITIGR_UTIL_MEMORY_HPP

#include <memory>

namespace dmitigr::memory {

/**
 * @brief A custom deleter for smart pointers.
 */
template<typename T>
class Conditional_delete final {
public:
  /**
   * @brief The default constructor.
   *
   * @par Effects
   * `condition()`.
   */
  constexpr Conditional_delete() noexcept = default;

  /**
   * @brief The constructor.
   *
   * @par Effects
   * `condition()`.
   */
  explicit constexpr Conditional_delete(const bool condition) noexcept
    : condition_(condition)
  {}

  /**
   * @returns The value of condition.
   */
  constexpr bool condition() const noexcept
  {
    return condition_;
  }

  /**
   * @brief Applies `std::default_delete::operator()` to the
   * pointer `o` if and only if the `(condition() == true)`.
   */
  void operator()(T* const o) const noexcept
  {
    if (condition())
      std::default_delete<T>{}(o);
  }

private:
  bool condition_{true};
};

} // namespace dmitigr::memory

#endif  // DMITIGR_UTIL_MEMORY_HPP
