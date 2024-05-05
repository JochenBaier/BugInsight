// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <cassert>


#include <cstdlib>
#include <vector>
#include <array>
#include <allocator.hpp>

#include <utest.h>

extern size_t buginsight_internal_allocated_bytes(const char* p_function);

struct aligned_malloc_bad_tests
{
};

UTEST_F_SETUP(aligned_malloc_bad_tests)
{
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

UTEST_F_TEARDOWN(aligned_malloc_bad_tests)
{
}

UTEST_F(aligned_malloc_bad_tests, _aligned_malloc_double_free_exit_66)
{
  void* p=_aligned_malloc(500, 32);
  ASSERT_TRUE(p);

  _aligned_free(p);
  _aligned_free(p);
}

UTEST_F(aligned_malloc_bad_tests, _aligned_malloc_dbg_double_free_exit_66)
{
  void* p=_aligned_malloc_dbg(500, 32, __FILE__, __LINE__);
  ASSERT_TRUE(p);

  _aligned_free_dbg(p);
  _aligned_free_dbg(p);
}

UTEST_F(aligned_malloc_bad_tests, _aligned_offset_malloc_double_free_exit_66)
{
  void* p=_aligned_offset_malloc(500, 32, 400);
  ASSERT_TRUE(p);

  _aligned_free(p);
  _aligned_free(p);
}

UTEST_F(aligned_malloc_bad_tests, _aligned_offset_malloc_dbg_double_free_exit_66)
{
  void* p=_aligned_offset_malloc_dbg(500, 32, 400, __FILE__, __LINE__);
  ASSERT_TRUE(p);

  _aligned_free_dbg(p);
  _aligned_free_dbg(p);
}

UTEST_F(aligned_malloc_bad_tests, _aligned_recalloc_deleted_pointer_exit_66)
{
  void* p=_aligned_recalloc(nullptr, 500, 10, 32);
  ASSERT_TRUE(p);

  _aligned_free(p);

  void* p2=_aligned_recalloc(p, 10, 20, 32);
}

UTEST_F(aligned_malloc_bad_tests, _aligned_recalloc_dbg_deleted_pointer_exit_66)
{
  void* p=_aligned_recalloc_dbg(nullptr, 500, 10, 32, __FILE__, __LINE__);
  ASSERT_TRUE(p);

  _aligned_free_dbg(p);

  void* p2=_aligned_recalloc_dbg(p, 10, 20, 32, __FILE__, __LINE__);
}

UTEST_F(aligned_malloc_bad_tests, _aligned_offset_recalloc_deleted_pointer_exit_66)
{
  void* p=_aligned_offset_recalloc(nullptr, 500, 10, 32, 400);
  ASSERT_TRUE(p);

  _aligned_free(p);

  void* p2=_aligned_offset_recalloc(p, 10, 20, 32, 400);
}

UTEST_F(aligned_malloc_bad_tests, _aligned_offset_realloc_dbg_deleted_pointer_exit_66)
{
  void* p=_aligned_offset_recalloc_dbg(nullptr, 500, 10, 32, 400, __FILE__, __LINE__);
  ASSERT_TRUE(p);

  _aligned_free_dbg(p);

  void* p2=_aligned_offset_recalloc_dbg(p, 10, 20, 32, 400, __FILE__, __LINE__);
}

UTEST_F(aligned_malloc_bad_tests, _aligned_realloc_deleted_pointer_exit_66)
{
  void* p=_aligned_realloc(nullptr, 500, 32);
  ASSERT_TRUE(p);

  _aligned_free(p);

  void* p2=_aligned_realloc(p, 600, 32);
}

UTEST_F(aligned_malloc_bad_tests, _aligned_realloc_dbg_deleted_pointer_exit_66)
{
  void* p=_aligned_realloc_dbg(nullptr, 500, 32, __FILE__, __LINE__);
  ASSERT_TRUE(p);

  _aligned_free_dbg(p);

  void* p2=_aligned_realloc_dbg(p, 600, 32, __FILE__, __LINE__);
}

UTEST_F(aligned_malloc_bad_tests, I_aligned_mallocI1I500I_leak_66)
{
  void* p=_aligned_malloc(500, 32);
  ASSERT_TRUE(p);
}

UTEST_F(aligned_malloc_bad_tests, I_aligned_mallocI2I1000I_leak_66)
{
  void* p=_aligned_malloc(500, 32);
  ASSERT_TRUE(p);
  void* p2=_aligned_malloc(500, 32);
  ASSERT_TRUE(p2);
}

UTEST_F(aligned_malloc_bad_tests, I_aligned_malloc_dbgI1I500I_leak_66)
{
  void* p=_aligned_malloc_dbg(500, 32, __FILE__, __LINE__);
  ASSERT_TRUE(p);
}

UTEST_F(aligned_malloc_bad_tests, I_aligned_offset_mallocI1I500I_leak_66)
{
  void* p=_aligned_offset_malloc(500, 32, 400);
  ASSERT_TRUE(p);
}

UTEST_F(aligned_malloc_bad_tests, I_aligned_offset_malloc_dbgI1I500I_leak_66)
{
  void* p=_aligned_offset_malloc_dbg(500, 32, 400, __FILE__, __LINE__);
  ASSERT_TRUE(p);
}

UTEST_F(aligned_malloc_bad_tests, I_aligned_recallocI1I200I_leak_66)
{
  void* p=_aligned_recalloc(nullptr, 500, 10, 32);
  ASSERT_TRUE(p);

  void* p2=_aligned_recalloc(p, 10, 20, 32);
  ASSERT_TRUE(p2);
}

UTEST_F(aligned_malloc_bad_tests, I_aligned_recalloc_dbgI1I200I_leak_66)
{
  void* p=_aligned_recalloc_dbg(nullptr, 500, 10, 32, __FILE__, __LINE__);
  ASSERT_TRUE(p);

  void* p2=_aligned_recalloc_dbg(p, 10, 20, 32, __FILE__, __LINE__);
  ASSERT_TRUE(p2);
}

UTEST_F(aligned_malloc_bad_tests, I_aligned_offset_recallocI1I20000I_leak_66)
{
  void* p=_aligned_offset_recalloc(nullptr, 500, 10, 32, 400);
  ASSERT_TRUE(p);

  void* p2=_aligned_offset_recalloc(p, 1000, 20, 32, 400);
  ASSERT_TRUE(p2);
}

UTEST_F(aligned_malloc_bad_tests, I_aligned_offset_recalloc_dbgI1I20000I_leak_66)
{
  void* p=_aligned_offset_recalloc_dbg(nullptr, 500, 10, 32, 400, __FILE__, __LINE__);
  ASSERT_TRUE(p);

  void* p2=_aligned_offset_recalloc_dbg(p, 1000, 20, 32, 400, __FILE__, __LINE__);
  ASSERT_TRUE(p2);
}

