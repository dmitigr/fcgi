// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or dt.hpp

#include <dmitigr/math.hpp>
#include <dmitigr/testo.hpp>

int main(int, char* argv[])
{
  namespace math = dmitigr::math;
  using namespace dmitigr::testo;
  using math::Interval;
  using math::Interval_type;

  try {
    {
      Interval<int> i;
      ASSERT(i.type() == Interval_type::closed);
      ASSERT(i.min() == 0);
      ASSERT(i.max() == 0);
    }
    {
      Interval<char> i{Interval_type::ropen, 0, 3};
      ASSERT(i.type() == Interval_type::ropen);
      ASSERT(i.min() == 0);
      ASSERT(i.max() == 3);
      ASSERT(!i.has(-1));
      ASSERT(i.has(0));
      ASSERT(i.has(1));
      ASSERT(i.has(2));
      ASSERT(!i.has(3));
    }
    {
      Interval<unsigned> i{Interval_type::lopen, 0, 3};
      ASSERT(i.type() == Interval_type::lopen);
      ASSERT(i.min() == 0);
      ASSERT(i.max() == 3);
      ASSERT(!i.has(-1));
      ASSERT(!i.has(0));
      ASSERT(i.has(1));
      ASSERT(i.has(2));
      ASSERT(i.has(3));
      ASSERT(!i.has(4));
      const auto [min, max] = i.release();
      ASSERT(min == 0);
      ASSERT(max == 3);
      ASSERT(i.type() == Interval_type::closed);
      ASSERT(i.min() == 0);
      ASSERT(i.max() == 0);
    }
    {
      Interval i{Interval_type::open, .0f, 1.0f};
      ASSERT(i.type() == Interval_type::open);
      ASSERT(!i.has(-.3f));
      ASSERT(i.has(.3f));
      ASSERT(!i.has(1.3f));
    }
  } catch (const std::exception& e) {
    report_failure(argv[0], e);
    return 1;
  } catch (...) {
    report_failure(argv[0]);
    return 2;
  }

  return 0;
}
