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


#pragma optimize( "", off )

#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#include <iostream>
#include <mutex>
#include <string>
#include <direct.h>

#if defined(_MSC_VER)
#define UBENCH_NOINLINE __declspec(noinline)
#else
#define UBENCH_NOINLINE UBENCH_ATTRIBUTE(noinline)
#endif


UBENCH_NOINLINE void ubench_do_nothing(void* const);

#define UBENCH_DO_NOTHING(x) ubench_do_nothing(x)

#if defined(_MSC_VER)
void _ReadWriteBarrier(void);

#define UBENCH_DECLARE_DO_NOTHING()                                            \
  void ubench_do_nothing(void *ptr) {                                          \
    (void)ptr;                                                                 \
    _ReadWriteBarrier();                                                       \
  }
#elif defined(__clang__)
#define UBENCH_DECLARE_DO_NOTHING()                                            \
  void ubench_do_nothing(void *ptr) {                                          \
    _Pragma("clang diagnostic push")                                           \
        _Pragma("clang diagnostic ignored \"-Wlanguage-extension-token\"");    \
    asm volatile("" : : "r"(ptr), "m"(ptr) : "memory");                        \
    _Pragma("clang diagnostic pop");                                           \
  }
#else
#define UBENCH_DECLARE_DO_NOTHING()                                            \
  void ubench_do_nothing(void *ptr) {                                          \
    asm volatile("" : : "r"(ptr), "m"(ptr) : "memory");                        \
  }
#endif

UBENCH_DECLARE_DO_NOTHING()


std::mutex g_mut;
static void handler(const doctest::AssertData& ad)
{
  using namespace doctest;

  // uncomment if asserts will be used in a multi-threaded context
  std::lock_guard<std::mutex> lock(g_mut);

  // here we can choose what to do:
  // - log the failed assert
  // - throw an exception
  // - call std::abort() or std::terminate()

  std::cout << Color::LightGrey << skipPathFromFilename(ad.m_file) << "(" << ad.m_line << "): ";
  std::cout << Color::Red << failureString(ad.m_at) << ": ";

  // handling only normal (comparison and unary) asserts - exceptions-related asserts have been skipped
  if(ad.m_at & assertType::is_normal) {
    std::cout << Color::Cyan << assertString(ad.m_at) << "( " << ad.m_expr << " ) ";
    std::cout << Color::None << (ad.m_threw?"THREW exception: ":"is NOT correct!\n");
    if(ad.m_threw)
      std::cout << ad.m_exception;
    else
      std::cout << "  values: " << assertString(ad.m_at) << "( " << ad.m_decomp << " )";
  } else {
    std::cout << Color::None << "an assert dealing with exceptions has failed!";
  }

  std::cout << std::endl;
}


#define CAT_I(a, b) a ## b
#define CAT(a, b) CAT_I(a, b)
#define SCOPED_NAME(name) CAT(name, ALLOC_PREFIX_)


class static_struct_file_global_t {
public:
  static_struct_file_global_t()
  {
#define ALLOC_PREFIX_ _in_struct_ctor
#include "leaks.hpp"

  }

};


static_struct_file_global_t g_struct_leak_a;
static_struct_file_global_t g_struct_leak_b;
static static_struct_file_global_t g_struct_leak_c;

void leak_in_function()
{
#define ALLOC_PREFIX_ _in_function
#include "leaks.hpp"

  //pointer override
  void* p=malloc(100);
  p=0;


  //reassing
  int* arr1=new int[10];
  arr1=new int[20];
  delete[] arr1;

}


#define ALLOC_PREFIX_ _file_global
#include "leaks.hpp"
#include "vld_bugs.hpp"


static std::string g_std_string_no_leak_empty;
static std::string g_std_string_no_leak_empty_small="small";
static std::string g_std_string_no_leak_empty_large="large large large large large large large large large large";

static void* g_malloc_static_leak=malloc(33);

class MemoryLeak {
public:
  MemoryLeak(size_t n) { l=malloc(n); memset(l, 0x30 + ((int)n / 10), n); } // 4,5
  ~MemoryLeak() { free(l); }
private:
  void* l;
};

static MemoryLeak* pml=new MemoryLeak(70); // 3: leaks a new pointer and malloc(70)
static MemoryLeak ml{80}; // *should* be freed and not report as a memory leak

void thread_function()
{
#define ALLOC_PREFIX_ _in_thread
#include "leaks.hpp"

}

int main(int argc, char* argv[])
{
  doctest::Context context(argc, argv);
  context.setAsDefaultForAssertsOutOfTestCases();
  context.setAssertHandler(handler);

  leak_in_function();

#define ALLOC_PREFIX_ _in_main
#include "leaks.hpp"

  std::thread t(thread_function);
  t.join();

  return 0;
}

#pragma optimize( "", on )