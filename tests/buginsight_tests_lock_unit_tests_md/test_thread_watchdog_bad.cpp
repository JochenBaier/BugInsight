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
#include "utils/cs.hpp"
#include "utils/signal.hpp"

#if _WIN32
#include <Windows.h>
#endif

TEST_CASE("test_cs_t1_duration_6_sec_bad_exit_66")
{
  cs_maker_t cs_maker;
  CRITICAL_SECTION* CriticalSectionA=cs_maker.make_scoped();

  EnterCriticalSection(CriticalSectionA);

  std::this_thread::sleep_for(std::chrono::seconds(6));

  LeaveCriticalSection(CriticalSectionA);
}

TEST_CASE("test_cs_t1_missing_leave_bad_exit_66")
{
  cs_maker_t cs_maker;

  CRITICAL_SECTION* CriticalSectionA=cs_maker.make_scoped();
  EnterCriticalSection(CriticalSectionA);

  std::this_thread::sleep_for(std::chrono::hours(2));
}

TEST_CASE("test_cs_2t_misssing_leave_bad_exit_66")
{
  cs_maker_t cs_maker;
  CRITICAL_SECTION* CriticalSectionA=cs_maker.make_scoped();

  EnterCriticalSection(CriticalSectionA);

  //leave in other thread
  std::thread t([&]()
    {
      EnterCriticalSection(CriticalSectionA);
    });

  t.join();
}

TEST_CASE("test_ok_cs_t1_40_bad_exit_66")
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

  std::this_thread::sleep_for(std::chrono::seconds(100));
  for(auto& cs : critical_sections)
  {
    DeleteCriticalSection(&cs);
  }

}

TEST_CASE("test_cs_t1_too_long_duration_exit_66")
{
  cs_maker_t cs_maker;
  CRITICAL_SECTION* CriticalSectionA=cs_maker.make_scoped();

  EnterCriticalSection(CriticalSectionA);

  std::this_thread::sleep_for(std::chrono::hours(2));

}

TEST_CASE("test_cs_misssing_leave_n_exit_66")
{
  cs_maker_t cs_maker;

  CRITICAL_SECTION* CriticalSectionA=cs_maker.make_scoped();
  CRITICAL_SECTION* CriticalSectionB=cs_maker.make_scoped();
  CRITICAL_SECTION* CriticalSectionC=cs_maker.make_scoped();
  CRITICAL_SECTION* CriticalSectionD=cs_maker.make_scoped();
  CRITICAL_SECTION* CriticalSectionE=cs_maker.make_scoped();

  utils::event_t signal_t1_cs_a;

  EnterCriticalSection(CriticalSectionA);

  //leave in other thread
  std::thread t([&]()
    {
      signal_t1_cs_a.notify_one();
      EnterCriticalSection(CriticalSectionA);
    });

  signal_t1_cs_a.wait();


  EnterCriticalSection(CriticalSectionB);
  EnterCriticalSection(CriticalSectionC);
  EnterCriticalSection(CriticalSectionD);
  EnterCriticalSection(CriticalSectionE);

  LeaveCriticalSection(CriticalSectionE);
  LeaveCriticalSection(CriticalSectionD);
  LeaveCriticalSection(CriticalSectionC);
  LeaveCriticalSection(CriticalSectionB);
  //missing: LeaveCriticalSection(&CriticalSectionB);

  t.join();
}

void test_thread_watchdog_cs_100t_10cs_bulk_thread_function_bad(std::vector<CRITICAL_SECTION*>& p_critical_sections, std::atomic_uint32_t& p_counter)
{
  for(size_t i=0; i < 10; ++i)
  {
    for(auto*& cs : p_critical_sections)
    {
      EnterCriticalSection(cs);
    }

    //std::this_thread::sleep_for(std::chrono::milliseconds(1));

    p_counter++;
    if(p_counter == 900)
    {
      std::this_thread::sleep_for(std::chrono::seconds(20));
    }

    for(auto*& cs : p_critical_sections)
    {
      LeaveCriticalSection(cs);
    }
  }
}

TEST_CASE("test_cs_multi_thread_bad_exit_66")
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
    threads.emplace_back(test_thread_watchdog_cs_100t_10cs_bulk_thread_function_bad, std::ref(critical_sections), std::ref(counter));
  }

  for(auto& t : threads)
  {
    t.join();
  }

  for(auto*& cs : critical_sections)
  {
    DeleteCriticalSection(cs);
  }

}

TEST_CASE("test_mutex_t1_duration_6_sec_bad_exit_66")
{
  std::mutex m;

  m.lock();
  std::this_thread::sleep_for(std::chrono::seconds(6));
  m.unlock();
}

TEST_CASE("test_lock_guard_mutex_t1_duration_6_sec_ok_exit_66")
{
  std::mutex m;
  const std::lock_guard<std::mutex> lock(m);
  std::this_thread::sleep_for(std::chrono::seconds(6));
}


TEST_CASE("test_std_condition_variable_signal_blocked_too_long_exit_66")
{
  std::mutex m_signal_mutex;
  std::condition_variable m_signal_condition;
  bool m_notified=false;

  utils::event_t signal;

  std::thread t1([&]()
    {
      std::this_thread::sleep_for(std::chrono::seconds(2));

      //waits too long in mutex
      std::unique_lock<std::mutex> lock(m_signal_mutex);
      std::this_thread::sleep_for(std::chrono::seconds(8));

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

void test_cs_too_many_waiters_func(CRITICAL_SECTION& p_critical_sections, std::atomic_uint32_t& p_counter)
{
  EnterCriticalSection(&p_critical_sections);

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  p_counter++;

  LeaveCriticalSection(&p_critical_sections);
}

TEST_CASE("test_cs_too_many_waiters_exit_66")
{
  const size_t thread_count=1000;
  std::atomic_uint32_t counter{0};

  CRITICAL_SECTION cs;
  InitializeCriticalSection(&cs);


  std::vector<std::thread> threads;

  for(size_t i=0; i < thread_count; ++i)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    threads.emplace_back(test_cs_too_many_waiters_func, std::ref(cs), std::ref(counter));
  }

  for(auto& t : threads)
  {
    t.join();
  }

  DeleteCriticalSection(&cs);

}
