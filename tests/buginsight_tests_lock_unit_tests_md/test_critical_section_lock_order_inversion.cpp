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

#if _WIN32
#include <Windows.h>
#endif

#include <atomic>
#include <thread>
#include <vector>
#include "utils/signal.hpp"
#include <random>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include "utils/cs.hpp"

#include "test_c_file.h"

#include "../shared/test_helper.hpp"


TEST_CASE("test_cs_t1_double_leave_exit_66")
{
  cs_maker_t cs_maker;

  CRITICAL_SECTION* CriticalSectionA=cs_maker.make_scoped();

  EnterCriticalSection(CriticalSectionA);

  //Double leave
  LeaveCriticalSection(CriticalSectionA);
  LeaveCriticalSection(CriticalSectionA);
}

TEST_CASE("test_cs_leave_other_thread_exit_66")
{
  cs_maker_t cs_maker;

  CRITICAL_SECTION* CriticalSectionA=cs_maker.make_scoped();

  EnterCriticalSection(CriticalSectionA);

  //leave in other thread
  std::thread t([&]()
    {
      LeaveCriticalSection(CriticalSectionA);
    });

  t.join();
}

TEST_CASE("test_bad_thread_sanitzer_example_exit_66")
{
  cs_maker_t cs_maker;

  CRITICAL_SECTION* mu1=cs_maker.make_scoped();
  CRITICAL_SECTION* mu2=cs_maker.make_scoped();

  // mu1 => mu2
  EnterCriticalSection(mu1);
  EnterCriticalSection(mu2);
  LeaveCriticalSection(mu2);
  LeaveCriticalSection(mu1);

  // mu2 => mu1
  EnterCriticalSection(mu2);
  EnterCriticalSection(mu1);  // <<<<<<< OOPS

  //REQUIRE(false);

  LeaveCriticalSection(mu1);
  LeaveCriticalSection(mu2);
}

TEST_CASE("test_bad_thread_sanitzer_example_cs_with_spin_count_exit_66")
{
  cs_maker_t cs_maker;

  CRITICAL_SECTION mu1;
  CRITICAL_SECTION mu2;

  InitializeCriticalSectionAndSpinCount(&mu1, 4000);
  InitializeCriticalSectionAndSpinCount(&mu2, 4000);

  // mu1 => mu2
  EnterCriticalSection(&mu1);
  EnterCriticalSection(&mu2);
  LeaveCriticalSection(&mu2);
  LeaveCriticalSection(&mu1);

  // mu2 => mu1
  EnterCriticalSection(&mu2);
  EnterCriticalSection(&mu1);  // <<<<<<< OOPS

  //REQUIRE(false);

  LeaveCriticalSection(&mu1);
  LeaveCriticalSection(&mu2);

  DeleteCriticalSection(&mu1);
  DeleteCriticalSection(&mu2);
}

TEST_CASE("test_cs_t1_a_b_t2_b_a_overlap_exit_66")
{
  cs_maker_t cs_maker;

  CRITICAL_SECTION* CriticalSectionA=cs_maker.make_scoped();
  CRITICAL_SECTION* CriticalSectionB=cs_maker.make_scoped();

  utils::event_t signal_t1_cs_a;
  utils::event_t signal_t2_cs_b;

  std::thread t([&]()
    {
      EnterCriticalSection(CriticalSectionA);
      signal_t1_cs_a.notify_one();

      signal_t2_cs_b.wait();
      EnterCriticalSection(CriticalSectionB);


      LeaveCriticalSection(CriticalSectionB);
      LeaveCriticalSection(CriticalSectionA);
    });

  std::thread t2([&]()
    {
      signal_t1_cs_a.wait();

      EnterCriticalSection(CriticalSectionB);
      signal_t2_cs_b.notify_one();

      EnterCriticalSection(CriticalSectionA);

      LeaveCriticalSection(CriticalSectionA);
      LeaveCriticalSection(CriticalSectionB);


    });

  t.join();
  t2.join();
}

TEST_CASE("test_cs_t1_a_b_t2_b_a_no_overlap_exit_66")
{
  cs_maker_t cs_maker;

  CRITICAL_SECTION* CriticalSectionA=cs_maker.make_scoped();
  CRITICAL_SECTION* CriticalSectionB=cs_maker.make_scoped();

  utils::event_t signal_t1;

  std::thread t([&]()
    {
      EnterCriticalSection(CriticalSectionA);
      EnterCriticalSection(CriticalSectionB);

      LeaveCriticalSection(CriticalSectionB);
      LeaveCriticalSection(CriticalSectionA);

      signal_t1.notify_one();

    });

  std::thread t2([&]()
    {
      signal_t1.wait();


      EnterCriticalSection(CriticalSectionB);
      EnterCriticalSection(CriticalSectionA);

      LeaveCriticalSection(CriticalSectionA);
      LeaveCriticalSection(CriticalSectionB);

    });

  t.join();
  t2.join();
}

TEST_CASE("test_cs_t1_a_b_t2_b_and_more_exit_66")
{
  cs_maker_t cs_maker;
  auto CriticalSectionA=cs_maker.make_scoped();
  auto CriticalSectionB=cs_maker.make_scoped();
  auto CriticalSectionC=cs_maker.make_scoped();
  auto CriticalSectionD=cs_maker.make_scoped();
  auto CriticalSectionE=cs_maker.make_scoped();

  utils::event_t block_thread;

  std::thread t1([&]()
    {
      std::thread::id id=std::this_thread::get_id();
      EnterCriticalSection(CriticalSectionA);
      EnterCriticalSection(CriticalSectionB);

      LeaveCriticalSection(CriticalSectionB);
      LeaveCriticalSection(CriticalSectionA);

      block_thread.wait();

    });

  std::thread t2([&]()
    {
      std::thread::id id=std::this_thread::get_id();

      EnterCriticalSection(CriticalSectionB);
      EnterCriticalSection(CriticalSectionC);

      LeaveCriticalSection(CriticalSectionC);
      LeaveCriticalSection(CriticalSectionB);

      block_thread.wait();

    });

  std::thread t3([&]()
    {
      std::thread::id id=std::this_thread::get_id();
      EnterCriticalSection(CriticalSectionC);
      EnterCriticalSection(CriticalSectionD);

      LeaveCriticalSection(CriticalSectionD);
      LeaveCriticalSection(CriticalSectionC);

      block_thread.wait();

    });

  std::thread t4([&]()
    {
      std::thread::id id=std::this_thread::get_id();
      EnterCriticalSection(CriticalSectionD);
      EnterCriticalSection(CriticalSectionE);

      LeaveCriticalSection(CriticalSectionE);
      LeaveCriticalSection(CriticalSectionD);

      block_thread.wait();

    });

  std::thread t5([&]()
    {
      std::thread::id id=std::this_thread::get_id();
      EnterCriticalSection(CriticalSectionE);
      EnterCriticalSection(CriticalSectionA);

      LeaveCriticalSection(CriticalSectionA);
      LeaveCriticalSection(CriticalSectionE);

      block_thread.wait();

    });


  t1.join();
  t2.join();
  t3.join();
  t4.join();
  t5.join();

}

void test_bad_cs_100t_10cs_bulk_thread_function(std::vector<CRITICAL_SECTION*>& p_critical_sections, std::atomic_uint32_t& p_counter)
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

TEST_CASE("test_bad_cs_100t_10cs_bulk_exit_66")
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

  std::vector<CRITICAL_SECTION*> critical_sections_wrong_order=critical_sections;

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

  for(auto*& cs : critical_sections)
  {
    DeleteCriticalSection(cs);
    delete cs;
  }

}

TEST_CASE("test_cs_t1_a_b_t2_b_a_no_overlap_halt_on_error_false_exit_66")
{
  cs_maker_t cs_maker;

  CRITICAL_SECTION* CriticalSectionA=cs_maker.make_scoped();
  CRITICAL_SECTION* CriticalSectionB=cs_maker.make_scoped();

  for(size_t i=0; i < 100; ++i)
  {
    utils::event_t signal_t1;

    std::thread t([&]()
      {
        EnterCriticalSection(CriticalSectionA);
        EnterCriticalSection(CriticalSectionB);

        LeaveCriticalSection(CriticalSectionB);
        LeaveCriticalSection(CriticalSectionA);

        signal_t1.notify_one();

      });

    std::thread t2([&]()
      {
        signal_t1.wait();

        EnterCriticalSection(CriticalSectionB);
        EnterCriticalSection(CriticalSectionA);

        LeaveCriticalSection(CriticalSectionA);
        LeaveCriticalSection(CriticalSectionB);

      });

    t.join();
    t2.join();
  }
}

TEST_CASE("test_cs_t1_a_b_t2_b_a_overlap_recurse_exit_66")
{
  cs_maker_t cs_maker;

  CRITICAL_SECTION* CriticalSectionA=cs_maker.make_scoped();
  CRITICAL_SECTION* CriticalSectionB=cs_maker.make_scoped();

  utils::event_t signal_t1_cs_a;
  utils::event_t signal_t2_cs_b;

  std::thread t([&]()
    {
      EnterCriticalSection(CriticalSectionA);
      EnterCriticalSection(CriticalSectionA);
      signal_t1_cs_a.notify_one();

      signal_t2_cs_b.wait();
      EnterCriticalSection(CriticalSectionB);
      EnterCriticalSection(CriticalSectionB);


      LeaveCriticalSection(CriticalSectionB);
      LeaveCriticalSection(CriticalSectionB);

      LeaveCriticalSection(CriticalSectionA);
      LeaveCriticalSection(CriticalSectionA);

    });

  std::thread t2([&]()
    {
      signal_t1_cs_a.wait();

      EnterCriticalSection(CriticalSectionB);
      EnterCriticalSection(CriticalSectionB);
      signal_t2_cs_b.notify_one();

      EnterCriticalSection(CriticalSectionA);
      EnterCriticalSection(CriticalSectionA);

      LeaveCriticalSection(CriticalSectionA);
      LeaveCriticalSection(CriticalSectionA);

      LeaveCriticalSection(CriticalSectionB);
      LeaveCriticalSection(CriticalSectionB);



    });

  t.join();
  t2.join();
}

TEST_CASE("test_cs_try_enter_t1_a_b_t2_b_a_overlap_exit_66")
{
  cs_maker_t cs_maker;

  CRITICAL_SECTION* CriticalSectionA=cs_maker.make_scoped();
  CRITICAL_SECTION* CriticalSectionB=cs_maker.make_scoped();

  utils::event_t signal_t1_cs_a;
  utils::event_t signal_t2_cs_b;

  std::thread t([&]()
    {
      const bool got=TryEnterCriticalSection(CriticalSectionA);
      REQUIRE(got);
      signal_t1_cs_a.notify_one();

      signal_t2_cs_b.wait();
      EnterCriticalSection(CriticalSectionB);


      LeaveCriticalSection(CriticalSectionB);
      LeaveCriticalSection(CriticalSectionA);
    });

  std::thread t2([&]()
    {
      signal_t1_cs_a.wait();

      const bool got=TryEnterCriticalSection(CriticalSectionB);
      REQUIRE(got);
      signal_t2_cs_b.notify_one();

      EnterCriticalSection(CriticalSectionA);

      LeaveCriticalSection(CriticalSectionA);
      LeaveCriticalSection(CriticalSectionB);


    });

  t.join();
  t2.join();
}

TEST_CASE("test_bad_cs_2t_10cs_bulk_exit_66")
{
  const size_t cs_count=20;
  const size_t thread_count=100;
  std::atomic_uint32_t counter{0};

  std::vector<CRITICAL_SECTION*> critical_sections;
  critical_sections.resize(cs_count);

  for(auto*& cs : critical_sections)
  {
    cs=new CRITICAL_SECTION;
    InitializeCriticalSection(cs);
  }

  std::vector<CRITICAL_SECTION*> critical_sections_wrong_order=critical_sections;

  std::random_device rd;
  std::mt19937 g(rd());

  std::shuffle(critical_sections_wrong_order.begin(), critical_sections_wrong_order.end(), g);

  std::vector<std::thread> threads;

  for(size_t i=0; i < thread_count; ++i)
  {
    if(i == thread_count - 2)
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

  for(auto*& cs : critical_sections)
  {
    DeleteCriticalSection(cs);
  }
}

TEST_CASE("test_c_file_thread_sanitzer_example_exit_66")
{
  test_c_file_thread_sanitzer_example();
}

TEST_CASE("test_deadlock_in_header_exit_66")
{
  test_in_header();
}

TEST_CASE("test_1t_2t2_exit_66")
{
  cs_maker_t cs_maker;

  CRITICAL_SECTION* CriticalSectionA=cs_maker.make_scoped();
  CRITICAL_SECTION* CriticalSectionB=cs_maker.make_scoped();
  CRITICAL_SECTION* CriticalSectionC=cs_maker.make_scoped();
  CRITICAL_SECTION* CriticalSectionD=cs_maker.make_scoped();
  CRITICAL_SECTION* CriticalSectionE=cs_maker.make_scoped();

  utils::event_t signal;

  std::thread t1([&]()
    {
      std::thread::id id=std::this_thread::get_id();
      EnterCriticalSection(CriticalSectionA);
      EnterCriticalSection(CriticalSectionB);
      EnterCriticalSection(CriticalSectionC);

      LeaveCriticalSection(CriticalSectionC);
      LeaveCriticalSection(CriticalSectionB);
      LeaveCriticalSection(CriticalSectionA);


      EnterCriticalSection(CriticalSectionA);
      EnterCriticalSection(CriticalSectionB);
      EnterCriticalSection(CriticalSectionC);
      EnterCriticalSection(CriticalSectionE);
      EnterCriticalSection(CriticalSectionD);


      LeaveCriticalSection(CriticalSectionE);
      LeaveCriticalSection(CriticalSectionD);
      LeaveCriticalSection(CriticalSectionC);
      LeaveCriticalSection(CriticalSectionB);
      LeaveCriticalSection(CriticalSectionA);


      signal.notify_one();

    });

  std::thread t2([&]()
    {
      std::thread::id id=std::this_thread::get_id();

      signal.wait();

      EnterCriticalSection(CriticalSectionD);
      EnterCriticalSection(CriticalSectionE);

      LeaveCriticalSection(CriticalSectionE);
      LeaveCriticalSection(CriticalSectionD);

    });


  t1.join();
  t2.join();

}

TEST_CASE("test_cs_too_many_locks_perh_thread_exit_66")
{
  const size_t cs_count=21;

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

TEST_CASE("test_cs_too_many_locks_perh_thread_1000_exit_66")
{
  const size_t cs_count=1000;

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

TEST_CASE("test_cs_too_many_locks_perh_thread_200")
{
  printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  const size_t cs_count=200;

  std::vector<CRITICAL_SECTION> critical_sections;
  critical_sections.resize(cs_count);

  for(auto& cs : critical_sections)
  {
    InitializeCriticalSection(&cs);
    test_helper::no_optimize(&cs);

  }

  for(auto& cs : critical_sections)
  {
    EnterCriticalSection(&cs);
    test_helper::no_optimize(&cs);
  }

  for(auto& cs : critical_sections)
  {
    LeaveCriticalSection(&cs);
    test_helper::no_optimize(&cs);
  }

  for(auto& cs : critical_sections)
  {
    DeleteCriticalSection(&cs);
    test_helper::no_optimize(&cs);
  }

}


CRITICAL_SECTION* m_a=nullptr;
CRITICAL_SECTION* m_b=nullptr;

#if defined(_WIN32)
#define ATTRIBUTE_NOINLINE __declspec(noinline)
#else
#define ATTRIBUTE_NOINLINE __attribute__((noinline))
#endif


ATTRIBUTE_NOINLINE void ce_funce()
{
  EnterCriticalSection(m_b);
  EnterCriticalSection(m_a);
}

ATTRIBUTE_NOINLINE void ce_funcd()
{
  ce_funce();
}

ATTRIBUTE_NOINLINE void ce_funcc()
{
  ce_funcd();
}

ATTRIBUTE_NOINLINE void ce_funcb()
{
  ce_funcc();
}

ATTRIBUTE_NOINLINE void ce_funca()
{
  ce_funcb();
}

TEST_CASE("test_cs_t1_a_b_t2_b_a_large_callstack_exit_66")
{
  cs_maker_t cs_maker;

  m_a=cs_maker.make_scoped();
  m_b=cs_maker.make_scoped();


  utils::event_t signal;

  std::thread t([&]()
    {
      EnterCriticalSection(m_a);
      EnterCriticalSection(m_b);

      LeaveCriticalSection(m_b);
      LeaveCriticalSection(m_a);


      signal.notify_one();
    });

  std::thread t2([&]()
    {
      signal.wait();

      ce_funca();
    });

  t.join();
  t2.join();

  DeleteCriticalSection(m_b);
  DeleteCriticalSection(m_a);
}

TEST_CASE("test_abseil_test_1_exit_66")
{
  std::vector<CRITICAL_SECTION> critical_sections;
  critical_sections.resize(100);

  for(auto& cs : critical_sections)
  {
    InitializeCriticalSection(&cs);
  }

  for(size_t i=0; i != critical_sections.size(); i++)
  {
    EnterCriticalSection(&critical_sections[i]);
    EnterCriticalSection(&critical_sections[(i + 1) % critical_sections.size()]);
    LeaveCriticalSection(&critical_sections[i]);
    LeaveCriticalSection(&critical_sections[(i + 1) % critical_sections.size()]);
  }

  for(auto& cs : critical_sections)
  {
    DeleteCriticalSection(&cs);
  }

}

TEST_CASE("test_abseil_test_1000_large_exit_66")
{
  std::vector<CRITICAL_SECTION> critical_sections;
  critical_sections.resize(1000);

  for(auto& cs : critical_sections)
  {
    InitializeCriticalSection(&cs);
  }

  for(size_t i=0; i != critical_sections.size(); i++)
  {
    EnterCriticalSection(&critical_sections[i]);
    EnterCriticalSection(&critical_sections[(i + 1) % critical_sections.size()]);
    LeaveCriticalSection(&critical_sections[i]);
    LeaveCriticalSection(&critical_sections[(i + 1) % critical_sections.size()]);
  }

  for(auto& cs : critical_sections)
  {
    DeleteCriticalSection(&cs);
  }


}


#define PHILOS 5
#define FOOD 50

void grab_chopstick(int,
  int,
  const char*);
void down_chopsticks(int,
  int);
int food_on_table();

CRITICAL_SECTION g_chopstick[PHILOS];
std::vector<std::thread>  g_philosopher;

CRITICAL_SECTION food_lock;
int sleep_seconds=0;


void philosopher(int num)
{
  int id;
  int left_chopstick, right_chopstick, f;

  id=(int)num;
  printf("Philosopher %d is done thinking and now ready to eat.\n", id);
  right_chopstick=id;
  left_chopstick=id + 1;

  /* Wrap around the chopsticks. */
  if(left_chopstick == PHILOS)
    left_chopstick=0;

  while(f=food_on_table()) {

    /* Thanks to philosophers #1 who would like to take a nap
    * before picking up the chopsticks, the other philosophers
    * may be able to eat their dishes and not deadlock.
    */
    if(id == 1)
      std::this_thread::sleep_for(std::chrono::seconds(sleep_seconds));

    grab_chopstick(id, right_chopstick, "right ");
    grab_chopstick(id, left_chopstick, "left");

    printf("Philosopher %d: eating.\n", id);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    down_chopsticks(left_chopstick, right_chopstick);
  }

  printf("Philosopher %d is done eating.\n", id);
}

int
food_on_table()
{
  static int food=FOOD;
  int myfood;

  EnterCriticalSection(&food_lock);
  if(food > 0) {
    food--;
  }
  myfood=food;
  LeaveCriticalSection(&food_lock);
  return myfood;
}

void
grab_chopstick(int phil,
  int c,
  const char* hand)
{
  EnterCriticalSection(&g_chopstick[c]);
  printf("Philosopher %d: got %s g_chopstick %d\n", phil, hand, c);
}

void
down_chopsticks(int c1,
  int c2)
{
  LeaveCriticalSection(&g_chopstick[c1]);
  LeaveCriticalSection(&g_chopstick[c2]);
}

//from https://docs.oracle.com/cd/E19205-01/820-0619/820-0619.pdf
TEST_CASE("test_dining_philosophers_exit_66")
{
  InitializeCriticalSection(&food_lock);
  for(auto& i : g_chopstick)
  {
    InitializeCriticalSection(&i);
  }

  for(int i=0; i < PHILOS; i++)
  {
    g_philosopher.emplace_back(philosopher, i);
  }

  for(int i=0; i < PHILOS; i++)
  {
    g_philosopher[i].join();
  }

  for(auto& i : g_chopstick)
  {
    DeleteCriticalSection(&i);
  }

  DeleteCriticalSection(&food_lock);
}


TEST_CASE("test_delete_cs_before_leave_exit_66")
{
  CRITICAL_SECTION mu1;
  InitializeCriticalSection(&mu1);

  EnterCriticalSection(&mu1);

  DeleteCriticalSection(&mu1);

  LeaveCriticalSection(&mu1);
}

TEST_CASE("test_delete_cs_in_other_thread_before_leave_exit_66")
{
  CRITICAL_SECTION mu1;
  InitializeCriticalSection(&mu1);

  EnterCriticalSection(&mu1);

  std::thread t([&]()
    {
      DeleteCriticalSection(&mu1);
    });
  t.join();

  LeaveCriticalSection(&mu1);
}

TEST_CASE("test_cs_leave_other_thread_cs_only_leave_disabled_exit_66")
{
  cs_maker_t cs_maker;

  CRITICAL_SECTION* CriticalSectionA=cs_maker.make_scoped();

  EnterCriticalSection(CriticalSectionA);

  //leave in other thread
  std::thread t([&]()
    {
      GetEnvironmentVariableA("buginsight/set/unlock++", nullptr, 0);
      LeaveCriticalSection(CriticalSectionA);
    });

  t.join();
}

TEST_CASE("test_cs_leave_cs_which_the_thread_does_not_hold_exit_66")
{
  cs_maker_t cs_maker;

  CRITICAL_SECTION* CriticalSectionA=cs_maker.make_scoped();

  EnterCriticalSection(CriticalSectionA);

  //leave in other thread
  std::thread t([&]()
    {
      LeaveCriticalSection(CriticalSectionA);
    });

  t.join();
}





