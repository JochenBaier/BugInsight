// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <array>
#include <cassert>

#include <Windows.h>

#include <utest.h>

extern size_t buginsight_internal_allocated_bytes(const char* p_function);

struct heapalloc_tests
{
};

UTEST_F_SETUP(heapalloc_tests)
{
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

UTEST_F_TEARDOWN(heapalloc_tests)
{
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

UTEST_F(heapalloc_tests, HeapAlloc_0_exit_0)
{
  HANDLE heap=HeapCreate(0, 0, 0);
  ASSERT_TRUE(heap);

  void* p=HeapAlloc(heap, 0, 0);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("HeapAlloc"));
  ASSERT_EQ(0, HeapSize(heap, 0, p));

  BOOL ok=HeapFree(heap, 0, p);
  ASSERT_TRUE(ok);

  ASSERT_EQ(0, buginsight_internal_allocated_bytes("HeapAlloc"));
}

UTEST_F(heapalloc_tests, HeapAlloc_100_exit_0)
{
  HANDLE heap=HeapCreate(0, 0, 0);
  ASSERT_TRUE(heap);

  void* p=HeapAlloc(heap, 0, 100);
  ASSERT_EQ(100, buginsight_internal_allocated_bytes("HeapAlloc"));
  ASSERT_EQ(100, HeapSize(heap, 0, p));

  BOOL ok=HeapFree(heap, 0, p);
  ASSERT_TRUE(ok);

  ASSERT_EQ(0, buginsight_internal_allocated_bytes("HeapAlloc"));
  ok=HeapDestroy(heap);
  ASSERT_TRUE(ok);
}

UTEST_F(heapalloc_tests, HeapAlloc_ProcessHeap_exit_0)
{
  HANDLE heap=GetProcessHeap();
  ASSERT_TRUE(heap);

  void* p=HeapAlloc(heap, 0, 100);
  ASSERT_EQ(100, buginsight_internal_allocated_bytes("HeapAlloc"));
  ASSERT_EQ(100, HeapSize(heap, 0, p));

  BOOL ok=HeapFree(heap, 0, p);
  ASSERT_TRUE(ok);

  ASSERT_EQ(0, buginsight_internal_allocated_bytes("HeapAlloc"));
}

UTEST_F(heapalloc_tests, HeapAlloc_10000_exit_0)
{
  HANDLE heap=HeapCreate(0, 0, 0);
  ASSERT_TRUE(heap);

  void* p=HeapAlloc(heap, 0, 10000);
  ASSERT_EQ(10000, buginsight_internal_allocated_bytes("HeapAlloc"));
  ASSERT_EQ(10000, HeapSize(heap, 0, p));

  BOOL ok=HeapFree(heap, HEAP_GENERATE_EXCEPTIONS, p);
  ASSERT_TRUE(ok);

  ASSERT_EQ(0, buginsight_internal_allocated_bytes("HeapAlloc"));

  ok=HeapDestroy(heap);
  ASSERT_TRUE(ok);
}

UTEST_F(heapalloc_tests, HeapAlloc_two_heaps_exit_0)
{
  HANDLE heapA=HeapCreate(0, 0, 0);
  ASSERT_TRUE(heapA);

  HANDLE heapB=HeapCreate(0, 0, 0);
  ASSERT_TRUE(heapB);

  void* pA1=HeapAlloc(heapA, 0, 100);
  ASSERT_EQ(100, HeapSize(heapA, 0, pA1));
  ASSERT_EQ(100, buginsight_internal_allocated_bytes("HeapAlloc"));

  void* pA2=HeapAlloc(heapA, 0, 200);
  ASSERT_EQ(200, HeapSize(heapA, 0, pA2));
  ASSERT_EQ(300, buginsight_internal_allocated_bytes("HeapAlloc"));


  void* pB1=HeapAlloc(heapB, 0, 300);
  ASSERT_EQ(300, HeapSize(heapB, 0, pB1));
  ASSERT_EQ(600, buginsight_internal_allocated_bytes("HeapAlloc"));

  void* pB2=HeapAlloc(heapB, 0, 400);
  ASSERT_EQ(400, HeapSize(heapB, 0, pB2));
  ASSERT_EQ(1000, buginsight_internal_allocated_bytes("HeapAlloc"));

  BOOL ok=HeapFree(heapA, 0, pA1);
  ASSERT_TRUE(ok);
  ASSERT_EQ(900, buginsight_internal_allocated_bytes("HeapAlloc"));

  ok=HeapFree(heapA, 0, pA2);
  ASSERT_TRUE(ok);
  ASSERT_EQ(700, buginsight_internal_allocated_bytes("HeapAlloc"));

  ok=HeapFree(heapB, 0, pB1);
  ASSERT_TRUE(ok);
  ASSERT_EQ(400, buginsight_internal_allocated_bytes("HeapAlloc"));

  ok=HeapFree(heapB, 0, pB2);
  ASSERT_TRUE(ok);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("HeapAlloc"));

  ok=HeapDestroy(heapA);
  ASSERT_TRUE(ok);

  ok=HeapDestroy(heapB);
  ASSERT_TRUE(ok);
}

UTEST_F(heapalloc_tests, HeapReAlloc_1000_1010_exit_0)
{
  HANDLE heap=HeapCreate(0, 0, 0);
  ASSERT_TRUE(heap);

  void* p=HeapAlloc(heap, 0, 1000);
  ASSERT_EQ(1000, buginsight_internal_allocated_bytes("HeapAlloc"));
  ASSERT_EQ(1000, HeapSize(heap, 0, p));

  void* p2=HeapReAlloc(heap, 0, p, 1010);
  ASSERT_EQ(1010, buginsight_internal_allocated_bytes("HeapReAlloc"));
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("HeapAlloc"));
  ASSERT_EQ(1010, HeapSize(heap, 0, p2));

  BOOL ok=HeapFree(heap, 0, p2);
  ASSERT_TRUE(ok);

  ASSERT_EQ(0, buginsight_internal_allocated_bytes("HeapAlloc"));
  ok=HeapDestroy(heap);
  ASSERT_TRUE(ok);
}

typedef LPVOID(__stdcall* heap_alloc_fptr)(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes);
typedef BOOL(__stdcall* heap_free_ftpr) (HANDLE hHeap, DWORD dwFlags, LPVOID lpMem);

UTEST_F(heapalloc_tests, HeapAlloc_GetProcAddress_exit_0)
{
  UTEST_SKIP("GetProcAddress not yet supported");

  HANDLE heap=HeapCreate(0, 0, 0);
  ASSERT_TRUE(heap);

  const HMODULE kernel32=GetModuleHandleW(L"kernel32.dll");

  heap_alloc_fptr heap_alloc_p=(heap_alloc_fptr)GetProcAddress(kernel32, "HeapAlloc");
  ASSERT_EQ(heap_alloc_p, HeapAlloc);

  void* p=heap_alloc_p(heap, 0, 42);
  ASSERT_TRUE(p);
  ASSERT_EQ(42, buginsight_internal_allocated_bytes("HeapAlloc"));
  ASSERT_EQ(42, HeapSize(heap, 0, p));

  heap_free_ftpr heap_free_p=(heap_free_ftpr)GetProcAddress(kernel32, "HeapFree");
  heap_free_p(heap, 0, p);

  ASSERT_EQ(0, buginsight_internal_allocated_bytes("HeapAlloc"));

  bool ok=HeapDestroy(heap);
  ASSERT_TRUE(ok);
}

