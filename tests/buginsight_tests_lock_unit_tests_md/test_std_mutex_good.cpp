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

//test with many includes...
#include <mutex>
#include <algorithm>
#include <thread>
#include  <chrono>
#include <atomic>
#include <cctype>
#include <cinttypes>
#include <climits>
#include <clocale>
#include <cstddef>
#include <cstdio>
#include <cstdio> // snprintf()
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <deque>
#include <exception>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <istream>
#include <iterator>
#include <limits>
#include <list>
#include <locale>
#include <map>
#include <memory>
#include <mutex>
#include <new>
#include <numeric>
#include <ostream>
#include <random>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <utility>
#include <vector>

#include "utils/signal.hpp"
#include <Windows.h>

TEST_CASE("test_std_mutex_t1_a_exit_0")
{
  std::mutex ma;

  {
    const bool got_lock=ma.try_lock();
    REQUIRE(got_lock);
    ma.unlock();
  }

  ma.lock();

  {
    const bool got_lock=ma.try_lock();
    REQUIRE_FALSE(got_lock);
  }

  ma.unlock();

  {
    const bool got_lock=ma.try_lock();
    REQUIRE(got_lock);
    ma.unlock();
  }

}

TEST_CASE("test_std_mutex_t1_a_b_exit_0")
{
  std::mutex ma;
  std::mutex mb;

  ma.lock();
  mb.lock();

  {
    const bool got_lock=ma.try_lock();
    REQUIRE_FALSE(got_lock);
  }

  {
    const bool got_lock=mb.try_lock();
    REQUIRE_FALSE(got_lock);
  }

  mb.unlock();
  ma.unlock();
}

TEST_CASE("test_condition_variable_exit_0")
{
  utils::event_t signal;

  std::atomic_uint16_t counter{0};

  std::thread t([&]()
    {
      std::this_thread::sleep_for(std::chrono::seconds(2));
      counter++;

      signal.notify_one();

    });

  signal.wait();

  REQUIRE(counter.load() == 1);

  t.join();
}

TEST_CASE("test_lock_guard_std_mutex_t1_a_exit_0")
{
  std::mutex ma;

  {
    const std::lock_guard lock(ma);

    {
      const bool got_lock=ma.try_lock();
      REQUIRE_FALSE(got_lock);
    }

  }
}

TEST_CASE("test_std_lock_guard_std_mutex_t1_a_b_exit_0")
{
  std::mutex ma;
  std::mutex mb;

  {
    const std::lock_guard lock_a(ma);

    {
      const bool got_lock=ma.try_lock();
      REQUIRE_FALSE(got_lock);
    }

    const std::lock_guard lock_b(mb);

    {
      const bool got_lock=mb.try_lock();
      REQUIRE_FALSE(got_lock);
    }

  }

}

TEST_CASE("test_unique_lock_std_mutex_t1_a_exit_0")
{
  std::mutex ma;

  {
    const std::unique_lock lock(ma);

    {
      const bool got_lock=ma.try_lock();
      REQUIRE_FALSE(got_lock);
    }

  }
}

TEST_CASE("test_std_unique_std_mutex_t1_a_b_exit_0")
{
  std::mutex ma;
  std::mutex mb;

  {
    const std::unique_lock lock_a(ma);

    {
      const bool got_lock=ma.try_lock();
      REQUIRE_FALSE(got_lock);
    }

    const std::unique_lock lock_b(mb);

    {
      const bool got_lock=mb.try_lock();
      REQUIRE_FALSE(got_lock);
    }

  }

}

TEST_CASE("test_std_mutex_try_lock_t1_a_b_t2_b_a_overlap_try_on_second_exit_0")
{
  std::mutex m_a;
  std::mutex m_b;

  utils::event_t signal_t1_cs_a;
  utils::event_t signal_t2_cs_b;
  utils::event_t signal_3;
  utils::event_t signal_4;

  std::thread t([&]()
    {
      const bool got=m_a.try_lock();
      REQUIRE(got);
      signal_t1_cs_a.notify_one();

      signal_t2_cs_b.wait();
      const bool got2=m_b.try_lock();

      signal_3.notify_one();
      REQUIRE_FALSE(got2);
      signal_4.wait();

      m_a.unlock();
    });

  std::thread t2([&]()
    {
      signal_t1_cs_a.wait();
      const bool got=m_b.try_lock();
      REQUIRE(got);

      signal_t2_cs_b.notify_one();

      signal_3.wait();

      const bool got2=m_a.try_lock();
      REQUIRE_FALSE(got2);

      signal_4.notify_one();

      m_b.unlock();

    });

  t.join();
  t2.join();
}

TEST_CASE("test_std_mutex_t1_a_b_t2_b_a_no_overlap_ignored_exit_0")
{
  std::mutex m_a;
  std::mutex m_b;

  utils::event_t signal;


  std::thread t([&]()
    {
      GetEnvironmentVariableA("buginsight/set/lock++", nullptr, 0);
      GetEnvironmentVariableA("buginsight/set/lock++", nullptr, 0);
      GetEnvironmentVariableA("buginsight/set/unlock++", nullptr, 0);
      GetEnvironmentVariableA("buginsight/set/unlock++", nullptr, 0);

      m_a.lock();
      m_b.lock();

      m_b.unlock();
      m_a.unlock();

      signal.notify_one();

    });

  std::thread t2([&]()
    {
      signal.wait();

      GetEnvironmentVariableA("buginsight/set/lock++", nullptr, 0);
      GetEnvironmentVariableA("buginsight/set/lock++", nullptr, 0);
      GetEnvironmentVariableA("buginsight/set/unlock++", nullptr, 0);
      GetEnvironmentVariableA("buginsight/set/unlock++", nullptr, 0);

      m_b.lock();
      m_a.lock();

      m_a.unlock();
      m_b.unlock();
    });

  t.join();
  t2.join();
}

TEST_CASE("test_lock_guard_std_mutex_t1_a_b_t2_b_a_no_overlap_ignore_exit_0")
{
  std::mutex m_a;
  std::mutex m_b;

  utils::event_t signal;

  std::thread t([&]()
    {
      const std::lock_guard lock_a(m_a);
      const std::lock_guard lock_b(m_b);
      signal.notify_one();

    });

  std::thread t2([&]()
    {
      signal.wait();

      GetEnvironmentVariableA("buginsight/set/lock++", nullptr, 0);
      GetEnvironmentVariableA("buginsight/set/lock++", nullptr, 0);
      GetEnvironmentVariableA("buginsight/set/unlock++", nullptr, 0);
      GetEnvironmentVariableA("buginsight/set/unlock++", nullptr, 0);

      const std::lock_guard lock_b(m_b);
      const std::lock_guard lock_a(m_a);
    });

  t.join();
  t2.join();
}

TEST_CASE("test_std_mutex_try_lock_second_time_exit_0")
{
  std::mutex ma;

  {
    const bool got_lock=ma.try_lock();
    REQUIRE(got_lock);
  }

  {
    const bool got_lock=ma.try_lock();
    REQUIRE_FALSE(got_lock);
  }

  ma.unlock();

}

TEST_CASE("test_std_mutex_try_lock_t1_a_b_t2_b_a_no_overlap_exit_0")
{
  std::mutex m_a;
  std::mutex m_b;

  utils::event_t signal;

  std::thread t([&]()
    {
      m_a.lock();
      m_b.lock();

      m_b.unlock();
      m_a.unlock();

      signal.notify_one();

    });

  std::thread t2([&]()
    {
      signal.wait();

      const bool gotb=m_b.try_lock();
      const bool gota=m_a.try_lock();

      REQUIRE(gotb);
      REQUIRE(gota);

      m_a.unlock();
      m_b.unlock();
    });

  t.join();
  t2.join();
}

TEST_CASE("test_from_abseil_source_test_large_exit_0")
{
  const int n_locks=1 << 17;
  auto array_of_locks=std::make_unique<std::mutex[]>(n_locks);
  for(int i=0; i < n_locks; i++)
  {
    int end=(std::min)(n_locks, i + 5);

    // acquire and then release locks i, i+1, ..., i+4
    for(int j=i; j < end; j++) {
      array_of_locks[j].lock();
    }

    for(int j=i; j < end; j++) {
      array_of_locks[j].unlock();
    }
  }
}


TEST_CASE("test_std_mutex_delete_without_use_exit_0")
{
  {
    std::mutex m;
  }
}

TEST_CASE("test_std_mutex_delete_with_use_exit_0")
{
  {
    std::mutex m;
    m.lock();
    m.unlock();
  }
}


TEST_CASE("test_std_mutex_mass_delete_exit_0")
{
  for(size_t i=0; i < 10000; ++i)
  {
    auto array_of_locks=std::make_unique<std::mutex[]>(1000);

    for(size_t j=0; j != 1000; j++)
    {
      array_of_locks[j].lock();
      array_of_locks[j].unlock();
    }

  }
}
