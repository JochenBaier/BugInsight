// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "utest.h"

extern size_t buginsight_internal_allocated_bytes(const char* p_function);

struct malloc_bad_tests
{
};

UTEST_F_SETUP(malloc_bad_tests)
{
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

UTEST_F_TEARDOWN(malloc_bad_tests)
{
}

UTEST_F(malloc_bad_tests, malloc_double_free_exit_66)
{
  void* p=malloc(5000);
  ASSERT_TRUE(p);

  free(p);
  free(p);
}

UTEST_F(malloc_bad_tests, _malloc_dbg_double_free_exit_66)
{
  void* p=_malloc_dbg(100, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p);

  _free_dbg(p, _NORMAL_BLOCK);
  _free_dbg(p, _NORMAL_BLOCK);
}

UTEST_F(malloc_bad_tests, realloc_double_free_exit_66)
{
  void* p=realloc(nullptr, 100);
  ASSERT_TRUE(p);

  realloc(p, 0);
  realloc(p, 0);
}

UTEST_F(malloc_bad_tests, _realloc_dgb_double_free_exit_66)
{
  void* p=_realloc_dbg(nullptr, 100, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p);

  _realloc_dbg(p, 0, _NORMAL_BLOCK, __FILE__, __LINE__);
  _realloc_dbg(p, 0, _NORMAL_BLOCK, __FILE__, __LINE__);
}

UTEST_F(malloc_bad_tests, realloc_with_freed_address_exit_66)
{
  void* p=realloc(nullptr, 100);
  ASSERT_TRUE(p);

  realloc(p, 0); //like free

  void* p2=realloc(p, 100);
}

UTEST_F(malloc_bad_tests, _realloc_dgb_with_freed_address_exit_66)
{
  void* p=_realloc_dbg(nullptr, 100, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p);

  _realloc_dbg(p, 0, _NORMAL_BLOCK, __FILE__, __LINE__); //like free

  void* p2=_realloc_dbg(p, 100, _NORMAL_BLOCK, __FILE__, __LINE__);
}

UTEST_F(malloc_bad_tests, _recalloc_double_free_exit_66)
{
  void* p=_recalloc(nullptr, 100, 2);
  ASSERT_TRUE(p);

  _recalloc(p, 2, 0);
  _recalloc(p, 2, 0);
}

UTEST_F(malloc_bad_tests, _recalloc_dbg_double_free_exit_66)
{
  void* p=_recalloc_dbg(nullptr, 100, 2, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p);

  _recalloc_dbg(p, 2, 0, _NORMAL_BLOCK, __FILE__, __LINE__);
  _recalloc_dbg(p, 2, 0, _NORMAL_BLOCK, __FILE__, __LINE__);
}

UTEST_F(malloc_bad_tests, _recalloc_with_freed_address_exit_66)
{
  void* p=_recalloc(nullptr, 100, 2);
  ASSERT_TRUE(p);

  _recalloc(p, 0, 0); //like free

  void* p2=_recalloc(p, 400, 4);
}

UTEST_F(malloc_bad_tests, _recalloc_dbg_with_freed_address_exit_66)
{
  void* p=_recalloc_dbg(nullptr, 100, 2, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p);

  _recalloc_dbg(p, 0, 0, _NORMAL_BLOCK, __FILE__, __LINE__); //like free

  void* p2=_recalloc_dbg(p, 400, 4, _NORMAL_BLOCK, __FILE__, __LINE__);
}

UTEST_F(malloc_bad_tests, _expand_with_freed_address_exit_66)
{
  void* p=malloc(1000);
  ASSERT_TRUE(p);

  free(p);

  _expand(p, 2000);
}

UTEST_F(malloc_bad_tests, _expand_dbg_with_freed_address_exit_66)
{
  void* p=_malloc_dbg(1000, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p);

  _free_dbg(p, _NORMAL_BLOCK);

  _expand_dbg(p, 2000, _NORMAL_BLOCK, __FILE__, __LINE__);
}

UTEST_F(malloc_bad_tests, ImallocI1I5000I_leak_66)
{
  void* p=malloc(5000);
  ASSERT_TRUE(p);
}

UTEST_F(malloc_bad_tests, ImallocI2I10000I_leak_66)
{
  void* p=malloc(5000);
  ASSERT_TRUE(p);
  void* p2=malloc(5000);
  ASSERT_TRUE(p2);
}

UTEST_F(malloc_bad_tests, ImallocI1I0I_leak_66)
{
  void* p=malloc(0);
  ASSERT_TRUE(p);
}

UTEST_F(malloc_bad_tests, I_malloc_dbgI1I5000I_leak_66)
{
  void* p=_malloc_dbg(5000, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p);
}

UTEST_F(malloc_bad_tests, I_malloc_dbgI1I0I_leak_66)
{
  void* p=_malloc_dbg(0, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p);
}

UTEST_F(malloc_bad_tests, IcallocI1I8I_leak_66)
{
  void* p=calloc(2, 4);
  ASSERT_TRUE(p);
}

UTEST_F(malloc_bad_tests, I_calloc_dbgI1I8I_leak_66)
{
  void* p=_calloc_dbg(2, 4, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p);
}

UTEST_F(malloc_bad_tests, IreallocI1I5000I_leak_66)
{
  void* p=realloc(nullptr, 5000);
  ASSERT_TRUE(p);
}

UTEST_F(malloc_bad_tests, I_realloc_dbgI1I5000I_leak_66)
{
  void* p=_realloc_dbg(nullptr, 5000, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p);
}

UTEST_F(malloc_bad_tests, I_recallocI1I20000I_leak_66)
{
  void* p=_recalloc(nullptr, 100, 200);
  ASSERT_TRUE(p);
}

UTEST_F(malloc_bad_tests, I_recalloc_dbgI1I20000I_leak_66)
{
  void* p=_recalloc_dbg(nullptr, 100, 200, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p);
}

UTEST_F(malloc_bad_tests, I_expandI1I5010I_leak_66)
{
  void* p=malloc(5000);
  ASSERT_TRUE(p);

  void* p2=_expand(p, 5010);
  ASSERT_TRUE(p);
}

UTEST_F(malloc_bad_tests, I_expand_dbgI1I5010I_leak_66)
{
  void* p=malloc(5000);
  ASSERT_TRUE(p);

  void* p2=_expand_dbg(p, 5010, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p);
}

