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

#include <thread>
#include <vector>
#include <atomic>
#include <mutex>

#ifdef _WIN32
#include <Windows.h>
#endif

#include "random_numbers.hpp"
#include "utils/cs.hpp"
#include "utils/signal.hpp"

TEST_CASE("test_cs_t1_duration_1_sec_ok_exit_0")
{
  cs_maker_t cs_maker;
  CRITICAL_SECTION* CriticalSectionA=cs_maker.make_scoped();

  EnterCriticalSection(CriticalSectionA);

  std::this_thread::sleep_for(std::chrono::seconds(1));

  LeaveCriticalSection(CriticalSectionA);
}

TEST_CASE("test_cs_t1_duration_4_sec_ok_exit_0")
{
  cs_maker_t cs_maker;

  CRITICAL_SECTION* CriticalSectionA=cs_maker.make_scoped();

  EnterCriticalSection(CriticalSectionA);

  std::this_thread::sleep_for(std::chrono::seconds(4));

  LeaveCriticalSection(CriticalSectionA);
}

TEST_CASE("test_x_times_cs_t1_duration_1_sec_ok_exit_0")
{
  cs_maker_t cs_maker;
  CRITICAL_SECTION* CriticalSectionA=cs_maker.make_scoped();

  for(size_t i=0; i < 1000; ++i)
  {
    std::thread t1([&]()
      {
        EnterCriticalSection(CriticalSectionA);
        LeaveCriticalSection(CriticalSectionA);
      });
    t1.join();
  }
}

TEST_CASE("test_ok_cs_t1_bulk2_ok_exit_0")
{
  const size_t cs_count=20;

  std::vector<CRITICAL_SECTION> critical_sections;
  critical_sections.resize(cs_count);

  for(auto& cs : critical_sections)
  {
    InitializeCriticalSection(&cs);
  }

  for(auto& cs : critical_sections)
  {
    EnterCriticalSection(&cs);
  }

  for(auto& cs : critical_sections)
  {
    LeaveCriticalSection(&cs);
  }

  for(auto& cs : critical_sections)
  {
    DeleteCriticalSection(&cs);
  }

}

void test_thread_watchdog_cs_100t_10cs_bulk_thread_function(std::vector<CRITICAL_SECTION*>& p_critical_sections, std::atomic_uint32_t& p_counter)
{
  for(size_t i=0; i < 10; ++i)
  {
    for(auto*& cs : p_critical_sections)
    {
      EnterCriticalSection(cs);
    }

    p_counter++;

    for(auto*& cs : p_critical_sections)
    {
      LeaveCriticalSection(cs);
    }
  }
}

TEST_CASE("test_good_cs_multi_thread_ok_exit_0")
{
  const size_t cs_count=20;
  const size_t thread_count=1000;
  std::atomic_uint32_t counter{0};

  std::vector<CRITICAL_SECTION*> critical_sections;
  critical_sections.resize(cs_count);

  for(auto*& cs : critical_sections)
  {
    cs=new CRITICAL_SECTION;
    InitializeCriticalSection(cs);
  }

  std::vector<std::thread> threads;

  for(size_t i=0; i < thread_count; ++i)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    threads.emplace_back(test_thread_watchdog_cs_100t_10cs_bulk_thread_function, std::ref(critical_sections), std::ref(counter));
  }

  for(auto& t : threads)
  {
    t.join();
  }

  for(auto*& cs : critical_sections)
  {
    DeleteCriticalSection(cs);
    delete cs;
  }
}

TEST_CASE("test_mutex_t1_duration_1_sec_ok_exit_0")
{
  std::mutex m;

  m.lock();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  m.unlock();
}

TEST_CASE("test_mutex_t1_duration_4_sec_ok_exit_0")
{
  std::mutex m;

  m.lock();
  std::this_thread::sleep_for(std::chrono::seconds(4));
  m.unlock();
}

TEST_CASE("test_lock_guard_mutex_t1_duration_4_sec_ok_exit_0")
{
  std::mutex m;
  const std::lock_guard<std::mutex> lock(m);
  std::this_thread::sleep_for(std::chrono::seconds(4));
}

TEST_CASE("test_std_condition_variable_duration_10_sec_good_exit_0")
{
  utils::event_t t;
  //should not trigger watchdog
  t.wait_with_timeout(10 * 1000);
}

TEST_CASE("test_std_condition_variable_duration_10_sec_with_signal_exit_0")
{
  utils::event_t t;

  std::thread t1([&]()
    {
      std::this_thread::sleep_for(std::chrono::seconds(10));
      t.notify_one();
    });


  //should not trigger watchdog
  t.wait();

  t1.join();
}

TEST_CASE("test_std_condition_variable_signal_short_blocked_exit_0")
{
  std::mutex m_signal_mutex;
  std::condition_variable m_signal_condition;
  bool m_notified=false;

  utils::event_t signal;

  std::thread t1([&]()
    {
      std::this_thread::sleep_for(std::chrono::seconds(2));

      std::unique_lock<std::mutex> lock(m_signal_mutex);
      std::this_thread::sleep_for(std::chrono::seconds(2));

      m_notified=true;
      m_signal_condition.notify_one();
    });


  std::unique_lock<std::mutex> lock(m_signal_mutex);
  m_signal_condition.wait(lock, [&]
    {
      return m_notified;
    });


  t1.join();
}

void test_thread_watchdog_many_too_long_durations_but_ignore_erros_func(std::atomic_uint32_t& p_counter)
{
  cs_maker_t cs_maker;
  CRITICAL_SECTION* CriticalSectionA=cs_maker.make_scoped();

  EnterCriticalSection(CriticalSectionA);

  const int random_sleep_time=random_helper::thread_safe_uniform_int(0, 4);
  std::this_thread::sleep_for(std::chrono::seconds(random_sleep_time));
  p_counter+=random_sleep_time;

  LeaveCriticalSection(CriticalSectionA);
}

TEST_CASE("test_thread_watchdog_many_too_long_durations_but_ignore_errors_exit_0")
{
#ifdef _WIN64
  const size_t thread_count=10000;
#else
  const size_t thread_count=1000;
#endif
  std::atomic_uint32_t counter{0};

  std::vector<std::thread> threads;

  for(size_t i=0; i < thread_count; ++i)
  {
    threads.emplace_back(test_thread_watchdog_many_too_long_durations_but_ignore_erros_func, std::ref(counter));
  }

  for(auto& t : threads)
  {
    t.join();
  }


}
