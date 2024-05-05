// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <array>
#include <cassert>

#include <Windows.h>

#include <utest.h>

extern size_t buginsight_internal_allocated_bytes(const char* p_function);


struct global_alloc_bad_tests
{
};

UTEST_F_SETUP(global_alloc_bad_tests)
{
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

UTEST_F_TEARDOWN(global_alloc_bad_tests)
{
}

UTEST_F(global_alloc_bad_tests, GlobalAlloc_free_local_alloc_exit_66)
{
  void* p=GlobalAlloc(0, 500);
  ASSERT_TRUE(p);

  LocalFree(p);
}

UTEST_F(global_alloc_bad_tests, GlobalAlloc_double_free_exit_66)
{
  void* p=GlobalAlloc(0, 500);
  ASSERT_TRUE(p);

  GlobalFree(p);
  GlobalFree(p);
}

UTEST_F(global_alloc_bad_tests, GlobalReAlloc_freed_pointer_exit_66)
{
  void* p=GlobalAlloc(LMEM_MOVEABLE, 1000);
  ASSERT_TRUE(p);
  GlobalFree(p);
  void* p2=GlobalReAlloc(p, 1010, 0);
}

UTEST_F(global_alloc_bad_tests, IGlobalAllocI1I500I_leak_66)
{
  void* p=GlobalAlloc(0, 500);
  ASSERT_TRUE(p);
}

UTEST_F(global_alloc_bad_tests, IGlobalAllocI2I1000I_leak_66)
{
  void* p=GlobalAlloc(0, 500);
  ASSERT_TRUE(p);
  void* p2=GlobalAlloc(0, 500);
  ASSERT_TRUE(p2);
}

UTEST_F(global_alloc_bad_tests, IGlobalAllocI1I0I_leak_66)
{
  void* p=GlobalAlloc(0, 0);
  ASSERT_TRUE(p);
}

UTEST_F(global_alloc_bad_tests, IGlobalReAllocI1I1010I_leak_66)
{
  void* p=GlobalAlloc(LMEM_MOVEABLE, 1000);
  ASSERT_TRUE(p);
  void* p2=GlobalReAlloc(p, 1010, 0);
  ASSERT_TRUE(p2);
}

