// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

//Licensed to the Apache Software Foundation (ASF) under one
//or more contributor license agreements.  See the NOTICE file
//distributed with this work for additional information
//regarding copyright ownership.  The ASF licenses this file
//to you under the Apache License, Version 2.0 (the
//"License"); you may not use this file except in compliance
//with the License.  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing,
//software distributed under the License is distributed on an
//"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
//KIND, either express or implied.  See the License for the
//specific language governing permissions and limitations
//under the License.  

#include <cassert>
#include <thread>


#define DOCTEST_CONFIG_IMPLEMENT
#define DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
#include "doctest.h"

#include <cs.hpp>
#include <werapi.h>

#include "signal.hpp"

void test_ok_cs_100t_10cs_bulk_thread_function2(std::vector<CRITICAL_SECTION>& p_critical_sections, std::atomic_uint32_t& p_counter)
{
  for(size_t i=0; i < 10; ++i)
  {
    for(auto& cs : p_critical_sections)
    {
      EnterCriticalSection(&cs);
    }

    ++p_counter;

    for(auto& cs : p_critical_sections)
    {
      LeaveCriticalSection(&cs);
    }
  }
}

//FIXME not used?
struct static_test_before_main_deadlock_t
{
  static_test_before_main_deadlock_t()
  {
    if(std::getenv("BUGINSIGHT_STATIC_TEST_BEFORE_MAIN") == nullptr)
    {
      return;
    }

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

  ~static_test_before_main_deadlock_t()
  {
  }

};

static static_test_before_main_deadlock_t static_test;

int main(int argc, char* argv[])
{
  doctest::Context context;
  context.applyCommandLine(argc, argv);

  //FIXME should be not used
  //context.addFilter("test-case", "malloc_tests.basic_exit_0");

  const int res_run=context.run();

  if(context.shouldExit())
  {
    return res_run;
  }

  return res_run;
}