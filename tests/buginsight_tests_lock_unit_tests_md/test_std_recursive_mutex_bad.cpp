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
#include <vector>
#include "utils/signal.hpp"
#include <random>

TEST_CASE("test_std_recursive_mutex_t1_a_unlock_double_bad_exit_66")
{
  std::recursive_mutex ma;
  ma.lock();
  ma.unlock();
  ma.unlock();
}

TEST_CASE("test_std_recursive_mutex_unlock_other_thread_exit_66")
{
  std::recursive_mutex ma;

  ma.lock();

  //leave in other thread
  std::thread t([&]()
    {
      ma.unlock();
    });

  t.join();
}

TEST_CASE("test_std_recursive_mutex_t1_a_b_t2_b_a_overlap_exit_66")
{
  std::recursive_mutex m_a;
  std::recursive_mutex m_b;

  utils::event_t signal_t1_cs_a;
  utils::event_t signal_t2_cs_b;

  std::thread t([&]()
    {
      m_a.lock();
      signal_t1_cs_a.notify_one();

      signal_t2_cs_b.wait();
      m_b.lock();


      m_b.unlock();
      m_a.unlock();
    });

  std::thread t2([&]()
    {
      signal_t1_cs_a.wait();

      m_b.lock();
      signal_t2_cs_b.notify_one();

      m_a.lock();

      m_a.unlock();
      m_b.unlock();

    });

  t.join();
  t2.join();
}

TEST_CASE("test_std_recursive_mutex_t1_a_b_t2_b_a_maybe_overlap_exit_66")
{
  std::recursive_mutex m_a;
  std::recursive_mutex m_b;

  std::thread t([&]()
    {
      m_a.lock();
      m_b.lock();

      m_b.unlock();
      m_a.unlock();
    });

  std::thread t2([&]()
    {
      m_b.lock();
      m_a.lock();

      m_a.unlock();
      m_b.unlock();

    });

  t.join();
  t2.join();
}

TEST_CASE("test_std_recursive_mutex_t1_a_b_t2_b_a_no_overlap_exit_66")
{
  std::recursive_mutex m_a;
  std::recursive_mutex m_b;

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

      m_b.lock();
      m_a.lock();

      m_a.unlock();
      m_b.unlock();
    });

  t.join();
  t2.join();
}

TEST_CASE("test_std_recursive_mutex_t1_a_b_t2_b_and_more_exit_66")
{
  std::recursive_mutex m_a;
  std::recursive_mutex m_b;
  std::recursive_mutex m_c;
  std::recursive_mutex m_d;
  std::recursive_mutex m_e;

  utils::event_t block_thread;

  std::thread t1([&]()
    {
      m_a.lock();
      m_b.lock();

      m_b.unlock();
      m_a.unlock();
    });

  std::thread t2([&]()
    {
      m_b.lock();
      m_c.lock();

      m_c.unlock();
      m_b.unlock();
    });

  std::thread t3([&]()
    {
      m_c.lock();
      m_d.lock();

      m_d.unlock();
      m_c.unlock();
    });

  std::thread t4([&]()
    {
      m_d.lock();
      m_e.lock();

      m_e.unlock();
      m_d.unlock();
    });

  std::thread t5([&]()
    {
      m_e.lock();
      m_a.lock();

      m_a.unlock();
      m_e.unlock();
    });

  t1.join();
  t2.join();
  t3.join();
  t4.join();
  t5.join();
}

void test_bad_cs_100t_10cs_bulk_thread_function(std::vector<std::recursive_mutex*>& p_critical_sections, std::atomic_uint32_t& p_counter)
{
  for(size_t i=0; i < 10; ++i)
  {
    for(auto*& cs : p_critical_sections)
    {
      cs->lock();
    }

    ++p_counter;

    for(auto*& cs : p_critical_sections)
    {
      cs->unlock();
    }
  }
}

TEST_CASE("test_std_recursive_mutex_100t_10_m_bulk_exit_66")
{
  const size_t cs_count=20;
  const size_t thread_count=1000;
  std::atomic_uint32_t counter{0};

  std::vector<std::recursive_mutex*> critical_sections;
  critical_sections.resize(cs_count);

  for(auto*& cs : critical_sections)
  {
    cs=new std::recursive_mutex(); //i know..
  }

  std::vector<std::recursive_mutex*> critical_sections_wrong_order=critical_sections;

  std::random_device rd;
  std::mt19937 g(rd());

  std::shuffle(critical_sections_wrong_order.begin(), critical_sections_wrong_order.end(), g);

  std::vector<std::thread> threads;

  for(size_t i=0; i < thread_count; ++i)
  {
    if(i == thread_count / 2)
    {
      threads.emplace_back(test_bad_cs_100t_10cs_bulk_thread_function, std::ref(critical_sections_wrong_order), std::ref(counter));
    } else
    {
      threads.emplace_back(test_bad_cs_100t_10cs_bulk_thread_function, std::ref(critical_sections), std::ref(counter));
    }
  }

  for(auto& t : threads)
  {
    t.join();
  }

}

TEST_CASE("test_lock_guard_std_recursive_mutex_t1_a_b_t2_b_a_maybe_overlap_exit_66")
{
  std::recursive_mutex m_a;
  std::recursive_mutex m_b;

  std::thread t1([&]()
    {
      const std::lock_guard < std::recursive_mutex> lock_a(m_a);
      const std::lock_guard < std::recursive_mutex> lock_b(m_b);
    });

  std::thread t2([&]()
    {
      const std::lock_guard < std::recursive_mutex> lock_b(m_b);
      const std::lock_guard < std::recursive_mutex> lock_a(m_a);
    });

  t1.join();
  t2.join();
}

TEST_CASE("test_lock_guard_recursive__t1_a_b_t2_b_a_maybe_overlap_exit_66")
{
  std::recursive_mutex m_a;
  std::recursive_mutex m_b;

  std::thread t1([&]()
    {
      const std::lock_guard < std::recursive_mutex> lock_a(m_a);
      const std::lock_guard < std::recursive_mutex> lock_b(m_b);
    });

  std::thread t2([&]()
    {
      const std::lock_guard < std::recursive_mutex> lock_b(m_b);
      const std::lock_guard < std::recursive_mutex> lock_a(m_a);
    });

  t1.join();
  t2.join();
}

TEST_CASE("test_std_recursive_mutex_delete_before_unlock_exit_66")
{
  {
    std::recursive_mutex m;
    m.lock();
  }
}

