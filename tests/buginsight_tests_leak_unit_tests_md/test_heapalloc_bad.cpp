// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <array>
#include <cassert>

#include <Windows.h>

#include <utest.h>

extern size_t buginsight_internal_allocated_bytes(const char* p_function);


struct heapalloc_bad_tests
{
};

UTEST_F_SETUP(heapalloc_bad_tests)
{
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

UTEST_F_TEARDOWN(heapalloc_bad_tests)
{
}

UTEST_F(heapalloc_bad_tests, HeapAlloc_double_free_exit_66)
{
  HANDLE heap=HeapCreate(0, 0, 0);
  ASSERT_TRUE(heap);

  void* p=HeapAlloc(heap, 0, 1000);

  BOOL ok=HeapFree(heap, 0, p);
  ASSERT_TRUE(ok);

  HeapFree(heap, 0, p);

  HeapDestroy(heap);
}

UTEST_F(heapalloc_bad_tests, HeapReAlloc_free_freed_pointer_exit_66)
{
  HANDLE heap=HeapCreate(0, 0, 0);
  ASSERT_TRUE(heap);

  void* p=HeapAlloc(heap, 0, 1000);

  BOOL ok=HeapFree(heap, 0, p);
  ASSERT_TRUE(ok);

  HeapReAlloc(heap, 0, p, 1000);
  HeapDestroy(heap);
}

UTEST_F(heapalloc_bad_tests, IHeapAllocI1I1000I_leak_66)
{
  HANDLE heap=HeapCreate(0, 0, 0);
  ASSERT_TRUE(heap);

  void* p=HeapAlloc(heap, 0, 1000);
  ASSERT_TRUE(p);
}

UTEST_F(heapalloc_bad_tests, IHeapAllocI2I2000I_leak_66)
{
  HANDLE heap=HeapCreate(0, 0, 0);
  ASSERT_TRUE(heap);

  void* p=HeapAlloc(heap, 0, 1000);
  ASSERT_TRUE(p);

  void* p2=HeapAlloc(heap, 0, 1000);
  ASSERT_TRUE(p2);
}


UTEST_F(heapalloc_bad_tests, IHeapReAllocI1I1010I_leak_66)
{
  HANDLE heap=HeapCreate(0, 0, 0);
  ASSERT_TRUE(heap);

  void* p=HeapAlloc(heap, 0, 1000);
  ASSERT_TRUE(p);

  void* p2=HeapReAlloc(heap, 0, p, 1010);
  ASSERT_TRUE(p2);
}

