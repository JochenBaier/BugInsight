// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

//This program is free software:you can redistribute it and /or modify it under the terms of the GNU
//General Public License as published by the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
//even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//See the GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License along with this program.If not, see
//<https://www.gnu.org/licenses/>. 

#include "doctest.h"

#include <mutex>
#include "utils/signal.hpp"

TEST_CASE("test_std_recursive_mutex_t1_a_exit_0")
{
  std::recursive_mutex ma;
  ma.lock();
  ma.unlock();
}

TEST_CASE("test_std_recursive_mutex_t1_a_b_exit_0")
{
  std::recursive_mutex ma;
  std::recursive_mutex mb;

  ma.lock();
  mb.lock();

  mb.unlock();
  ma.unlock();
}

TEST_CASE("test_std_mutex_t1_a_lock_recurse_good_exit_0")
{
  std::recursive_mutex ma;
  ma.lock();
  ma.lock();
  ma.unlock();
  ma.unlock();
}

TEST_CASE("test_lock_guard_std_recursive_mutex_t1_a_exit_0")
{
  std::recursive_mutex ma;

  {
    const std::lock_guard <std::recursive_mutex> lock(ma);

    {
      const bool got_lock=ma.try_lock();
      REQUIRE(got_lock);
    }

  }

  ma.unlock();
}

TEST_CASE("test_std_lock_guard_std_recursive_mutex_t1_a_b_exit_0")
{
  std::recursive_mutex ma;
  std::recursive_mutex mb;

  {
    const std::lock_guard < std::recursive_mutex> lock_a(ma);

    {
      const bool got_lock=ma.try_lock();
      REQUIRE(got_lock);
    }

    const std::lock_guard < std::recursive_mutex> lock_b(mb);

    {
      const bool got_lock=mb.try_lock();
      REQUIRE(got_lock);
    }
  }

  ma.unlock();
  mb.unlock();

}

TEST_CASE("test_lock_guard_std_mutex_t1_a_lock_recurse_exit_0")
{
  std::recursive_mutex ma;
  {
    const std::lock_guard < std::recursive_mutex> lock_a(ma);
    const std::lock_guard < std::recursive_mutex> lock_b(ma);
  }

}

TEST_CASE("test_std_recursive_mutex_delete_without_use_exit_0")
{
  {
    std::recursive_mutex m;
  }
}

TEST_CASE("test_std_recursive_mutex_delete_with_use_exit_0")
{
  {
    std::recursive_mutex m;
    m.lock();
    m.unlock();
  }
}

TEST_CASE("test_std_recursive_mutex_mass_delete_exit_0")
{
  for(size_t i=0; i < 10000; ++i)
  {
    auto array_of_locks=std::make_unique<std::recursive_mutex[]>(1000);

    for(size_t j=0; j < 1000; j++)
    {
      array_of_locks[j].lock();
      array_of_locks[j].unlock();
    }

  }
}

