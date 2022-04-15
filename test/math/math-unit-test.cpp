// -*- C++ -*-
//
// Copyright 2022 Dmitry Igrishin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "../../src/base/assert.hpp"
#include "../../src/math/math.hpp"

#include <array>
#include <vector>

int main()
{
  try {
    namespace math = dmitigr::math;

    // avg
    {
      constexpr auto a1 = math::avg(std::array<int, 5>{1,2,3,4,5});
      static_assert(static_cast<int>(a1) == static_cast<int>(3));
      const auto a2 = math::avg(std::vector<int>{1,2,3,4,5});
      DMITIGR_ASSERT(a2 == static_cast<int>(3));
    }

    // variance 1
    {
      constexpr auto d1 = math::variance(std::array<int, 5>{1,2,3,4,5});
      static_assert(static_cast<int>(d1) == static_cast<int>(2));
      const auto d2 = math::variance(std::vector<int>{1,2,3,4,5});
      DMITIGR_ASSERT(d2 == static_cast<int>(2));
      constexpr auto d3 = math::variance(std::array<int, 5>{600,470,170,430,300});
      static_assert(static_cast<int>(d3) == static_cast<int>(21704));
    }

    // variance 2
    {
      constexpr auto d1 = math::variance(std::array<int, 5>{1,2,3,4,5}, false);
      static_assert(static_cast<int>(d1) == static_cast<int>(2));
      const auto d2 = math::variance(std::vector<int>{1,2,3,4,5});
      DMITIGR_ASSERT(d2 == static_cast<int>(2));
      constexpr auto d3 = math::variance(std::array<int, 5>{600,470,170,430,300});
      static_assert(static_cast<int>(d3) == static_cast<int>(21704));
    }

    // -------------------------------------------------------------------------
    // Interval
    // -------------------------------------------------------------------------

    using math::Interval;
    using math::Interval_type;

    {
      Interval<int> i;
      DMITIGR_ASSERT(i.type() == Interval_type::closed);
      DMITIGR_ASSERT(i.min() == 0);
      DMITIGR_ASSERT(i.max() == 0);
    }
    {
      Interval<char> i{Interval_type::ropen, 0, 3};
      DMITIGR_ASSERT(i.type() == Interval_type::ropen);
      DMITIGR_ASSERT(i.min() == 0);
      DMITIGR_ASSERT(i.max() == 3);
      DMITIGR_ASSERT(!i.has(-1));
      DMITIGR_ASSERT(i.has(0));
      DMITIGR_ASSERT(i.has(1));
      DMITIGR_ASSERT(i.has(2));
      DMITIGR_ASSERT(!i.has(3));
    }
    {
      Interval<int> i{Interval_type::lopen, 0, 3};
      DMITIGR_ASSERT(i.type() == Interval_type::lopen);
      DMITIGR_ASSERT(i.min() == 0);
      DMITIGR_ASSERT(i.max() == 3);
      DMITIGR_ASSERT(!i.has(-1));
      DMITIGR_ASSERT(!i.has(0));
      DMITIGR_ASSERT(i.has(1));
      DMITIGR_ASSERT(i.has(2));
      DMITIGR_ASSERT(i.has(3));
      DMITIGR_ASSERT(!i.has(4));
      const auto [min, max] = i.release();
      DMITIGR_ASSERT(min == 0);
      DMITIGR_ASSERT(max == 3);
      DMITIGR_ASSERT(i.type() == Interval_type::closed);
      DMITIGR_ASSERT(i.min() == 0);
      DMITIGR_ASSERT(i.max() == 0);
    }
    {
      Interval i{Interval_type::open, .0f, 1.0f};
      DMITIGR_ASSERT(i.type() == Interval_type::open);
      DMITIGR_ASSERT(!i.has(-.3f));
      DMITIGR_ASSERT(i.has(.3f));
      DMITIGR_ASSERT(!i.has(1.3f));
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "unknown error" << std::endl;
    return 2;
  }
}
