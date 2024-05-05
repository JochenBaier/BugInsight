// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <array>
#include <cassert>

#include <Windows.h>

#include <utest.h>

extern size_t buginsight_internal_allocated_bytes(const char* p_function);


struct globalalloc_tests
{
};

UTEST_F_SETUP(globalalloc_tests)
{
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

UTEST_F_TEARDOWN(globalalloc_tests)
{
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

UTEST_F(globalalloc_tests, GlobalAlloc_0_exit_0)
{
  void* p=GlobalAlloc(0, 0);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("GlobalAlloc"));
  const size_t expected_size=GlobalSize(p);
  ASSERT_EQ(1, expected_size);

  GlobalFree(p);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("GlobalAlloc"));
}

UTEST_F(globalalloc_tests, GlobalAlloc_100_exit_0)
{
  void* p=GlobalAlloc(0, 100);
  ASSERT_EQ(100, buginsight_internal_allocated_bytes("GlobalAlloc"));
  ASSERT_EQ(100, GlobalSize(p));

  GlobalFree(p);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("GlobalAlloc"));
}

UTEST_F(globalalloc_tests, GlobalAlloc_LMEM_ZEROINIT_100_exit_0)
{
  void* p=GlobalAlloc(LMEM_ZEROINIT, 100);
  ASSERT_EQ(100, buginsight_internal_allocated_bytes("GlobalAlloc"));
  ASSERT_EQ(100, GlobalSize(p));

  std::array<uint8_t, 100> zeros{0};

  ASSERT_TRUE(memcmp(zeros.data(), p, 100) == 0);

  GlobalFree(p);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("GlobalAlloc"));
}

UTEST_F(globalalloc_tests, GlobalAlloc_10000_exit_0)
{
  void* p=GlobalAlloc(0, 10000);
  ASSERT_EQ(10000, buginsight_internal_allocated_bytes("GlobalAlloc"));
  ASSERT_EQ(10000, GlobalSize(p));

  GlobalFree(p);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("GlobalAlloc"));
}

UTEST_F(globalalloc_tests, GlobalReAlloc_1000_1010_exit_0)
{
  void* p=GlobalAlloc(LMEM_MOVEABLE, 1000);
  ASSERT_EQ(1000, buginsight_internal_allocated_bytes("GlobalAlloc"));
  ASSERT_EQ(1000, GlobalSize(p));

  void* p2=GlobalReAlloc(p, 1010, 0);
  ASSERT_TRUE(p2);

  ASSERT_EQ(1010, buginsight_internal_allocated_bytes("GlobalReAlloc"));
  ASSERT_EQ(1010, GlobalSize(p2));
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("GlobalAlloc"));

  GlobalFree(p2);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("GlobalAlloc"));
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("GlobalReAlloc"));
}


