// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <thread>
#include <Windows.h>

#include <utest.h>

extern size_t buginsight_internal_allocated_bytes(const char* p_function);

struct loadlibrary_tests
{
};

UTEST_F_SETUP(loadlibrary_tests)
{
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

UTEST_F_TEARDOWN(loadlibrary_tests)
{
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

UTEST_F(loadlibrary_tests, LoadLibraryTest_exit_0)
{
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));

  HMODULE library=LoadLibraryA("buginsight_tests_leak_dll_mini.dll");
  ASSERT_TRUE(library);

  ASSERT_EQ(100, buginsight_internal_allocated_bytes("malloc"));

  bool ok=FreeLibrary(library);
  ASSERT_TRUE(ok);

  GetEnvironmentVariableA("buginsight/set/mem/reset_all", nullptr, 0);
}

UTEST_F(loadlibrary_tests, LoadLibrarySameTwice_exit_0)
{
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("malloc"));

#if 1

  std::thread t1([&]()
    {
      for(size_t j=0; j < 100000; ++j)
      {
        void* p=malloc(j);
        free(p);
      }
      int brk=1;

    });

  std::thread t2([&]()
    {
      for(size_t j=0; j < 100000; ++j)
      {
        void* p=malloc(j);
        free(p);
      }
      int brk=1;
    });

#endif

#if 1

  std::thread t3([&]()
    {
      for(size_t j=0; j < 10; ++j)
      {
        HMODULE library=LoadLibraryA("buginsight_tests_leak_dll_mini.dll");
        ASSERT_TRUE(library);
        bool ok=FreeLibrary(library);
        ASSERT_TRUE(ok);
      }
      int brk=1;
    });

  std::thread t4([&]()
    {
      for(size_t j=0; j < 10; ++j)
      {
        HMODULE library=LoadLibraryA("buginsight_tests_leak_dll_mini2.dll");
        ASSERT_TRUE(library);
        bool ok=FreeLibrary(library);
        ASSERT_TRUE(ok);
      }
      int brk=1;
    });

#endif

  t1.join();
  t2.join();
  t3.join();
  t4.join();

  ASSERT_EQ(2000, buginsight_internal_allocated_bytes("malloc"));
  GetEnvironmentVariableA("buginsight/set/mem/reset_all", nullptr, 0);
}

UTEST_F(loadlibrary_tests, LoadLibraryMassTest_exit_0)
{
  std::thread t1([&]()
    {
      for(size_t j=0; j < 1000; ++j)
      {
        for(size_t i=0; i < 10000; ++i)
        {
          void* p=malloc(i);
          free(p);
        }
      }
      int brk=1;

    });

  for(size_t i=0; i < 500; i++)
  {
    HMODULE library=LoadLibraryA("buginsight_tests_leak_dll_mini.dll");
    ASSERT_TRUE(library);
    bool ok=FreeLibrary(library);
    ASSERT_TRUE(ok);
  }

  t1.join();

  GetEnvironmentVariableA("buginsight/set/mem/reset_all", nullptr, 0);

}
