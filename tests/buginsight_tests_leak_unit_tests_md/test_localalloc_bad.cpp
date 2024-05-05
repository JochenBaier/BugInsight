// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <array>
#include <cassert>

#include <Windows.h>

#include <utest.h>

extern size_t buginsight_internal_allocated_bytes(const char* p_function);

struct localalloc_bad_tests
{
};

UTEST_F_SETUP(localalloc_bad_tests)
{
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

UTEST_F_TEARDOWN(localalloc_bad_tests)
{
}

UTEST_F(localalloc_bad_tests, LocalAlloc_global_free_exit_66)
{
  void* p=LocalAlloc(0, 500);
  ASSERT_TRUE(p);

  GlobalFree(p);
}

UTEST_F(localalloc_bad_tests, LocalAlloc_double_free_exit_66)
{
  void* p=LocalAlloc(0, 500);

  LocalFree(p);
  LocalFree(p);
}

UTEST_F(localalloc_bad_tests, LocalReAlloc_freed_pointer_exit_66)
{
  void* p=LocalAlloc(LMEM_MOVEABLE, 1000);

  LocalFree(p);

  void* p2=LocalReAlloc(p, 1010, 0);
}

UTEST_F(localalloc_bad_tests, ILocalAllocI1I500I_leak_66)
{
  void* p=LocalAlloc(0, 500);
  ASSERT_TRUE(p);
}

UTEST_F(localalloc_bad_tests, ILocalAllocI2I1000I_leak_66)
{
  void* p=LocalAlloc(0, 500);
  ASSERT_TRUE(p);
  void* p2=LocalAlloc(0, 500);
  ASSERT_TRUE(p2);
}

UTEST_F(localalloc_bad_tests, ILocalAllocI1I0I_leak_66)
{
  void* p=LocalAlloc(0, 0);
  ASSERT_TRUE(p);
}

UTEST_F(localalloc_bad_tests, ILocalReAllocI1I1010I_leak_66)
{
  void* p=LocalAlloc(LMEM_MOVEABLE, 1000);
  void* p2=LocalReAlloc(p, 1010, 0);
  ASSERT_TRUE(p2);
}


