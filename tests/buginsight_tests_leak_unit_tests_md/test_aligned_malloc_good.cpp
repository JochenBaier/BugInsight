
// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <cassert>


#include <cstdlib>
#include <vector>
#include <array>
#include <allocator.hpp>

#include <utest.h>

extern size_t buginsight_internal_allocated_bytes(const char* p_function);


struct aligned_malloc_tests
{
};

struct allocated_t
{
  allocated_t(void* p_pointer, const char* p_function, size_t p_expected_alloc_after_free) :
    m_pointer(p_pointer),
    m_expected_alloc_after_free(p_expected_alloc_after_free)
  {
    strcpy_s(m_function.data(), m_function.size(), p_function);
  }

  void* m_pointer;
  std::array<char, 256> m_function{0};
  size_t m_expected_alloc_after_free;
};

UTEST_F_SETUP(aligned_malloc_tests)
{
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

UTEST_F_TEARDOWN(aligned_malloc_tests)
{
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

UTEST_F(aligned_malloc_tests, basic_exit_0)
{
  const size_t sizes[]={0, 1, 10, 1000, 1073741824llu};

  std::vector<allocated_t, meta_allocator<allocated_t>> allocations;
  std::vector<void*, meta_allocator<void*>> pointer_dbg;

  size_t expected_allocations=0;

  for(size_t i=0; i < _countof(sizes); ++i)
  {
    const size_t size=sizes[i];
    size_t expected_allocations_after_free=expected_allocations;
    expected_allocations+=size;

    {
      void* p=_aligned_malloc(size, 32);
      ASSERT_TRUE(p);
      ASSERT_TRUE(((size_t)p % 32) == 0);
#ifdef _DEBUG
      ASSERT_EQ(_aligned_msize(p, 32, 0), size);
#endif
      ASSERT_EQ(expected_allocations, buginsight_internal_allocated_bytes("_aligned_malloc"));
      allocations.emplace_back(p, "_aligned_malloc", expected_allocations_after_free);
    }

    {
      void* p=_aligned_offset_malloc(size, 32, size?size - 1:0);
      ASSERT_TRUE(p);
#ifdef _DEBUG
      ASSERT_EQ(_aligned_msize(p, 32, size?size - 1:0), size);
#endif
      ASSERT_EQ(expected_allocations, buginsight_internal_allocated_bytes("_aligned_offset_malloc"));
      allocations.emplace_back(p, "_aligned_offset_malloc", expected_allocations_after_free);
    }

    {
      void* p=_aligned_realloc(nullptr, size, 32);
      ASSERT_TRUE(p);
      ASSERT_TRUE(((size_t)p % 32) == 0);
#ifdef _DEBUG
      ASSERT_EQ(_aligned_msize(p, 32, 0), size);
#endif
      ASSERT_EQ(expected_allocations, buginsight_internal_allocated_bytes("_aligned_realloc"));
      allocations.emplace_back(p, "_aligned_realloc", expected_allocations_after_free);
    }

    {
      void* p=_aligned_offset_realloc(nullptr, size, 32, size?size - 1:0);
      ASSERT_TRUE(p);
#ifdef _DEBUG
      ASSERT_EQ(_aligned_msize(p, 32, size?size - 1:0), size);
#endif
      ASSERT_EQ(expected_allocations, buginsight_internal_allocated_bytes("_aligned_offset_realloc"));
      allocations.emplace_back(p, "_aligned_offset_realloc", expected_allocations_after_free);
    }

    {
      void* p=_aligned_recalloc(nullptr, size, sizeof(uint8_t), 32);
      ASSERT_TRUE(p);
      ASSERT_TRUE(((size_t)p % 32) == 0);
#ifdef _DEBUG
      ASSERT_EQ(_aligned_msize(p, 32, 0), size);
#endif
      ASSERT_EQ(expected_allocations, buginsight_internal_allocated_bytes("_aligned_recalloc"));
      allocations.emplace_back(p, "_aligned_recalloc", expected_allocations_after_free);
    }

    {
      void* p=_aligned_offset_recalloc(nullptr, size, sizeof(uint8_t), 32, size?size - 1:0);
      ASSERT_TRUE(p);
#ifdef _DEBUG
      //ASSERT_EQ(_aligned_msize(p, 32, size?size-1:0), size);
#endif
      ASSERT_EQ(expected_allocations, buginsight_internal_allocated_bytes("_aligned_offset_recalloc"));
      allocations.emplace_back(p, "_aligned_offset_recalloc", expected_allocations_after_free);
    }

  }

  std::reverse(allocations.begin(), allocations.end());

  for(const allocated_t& allocted : allocations)
  {
    _aligned_free(allocted.m_pointer);
    ASSERT_EQ(allocted.m_expected_alloc_after_free, buginsight_internal_allocated_bytes(allocted.m_function.data()));
  }

}


UTEST_F(aligned_malloc_tests, basic_dbg_exit_0)
{
  const size_t sizes[]={0, 1, 10, 1000, 1073741824llu};

  std::vector<allocated_t, meta_allocator<allocated_t>> allocations_dbg;
  std::vector<void*, meta_allocator<void*>> pointer_dbg;

  size_t expected_allocations=0;

  for(size_t i=0; i < _countof(sizes); ++i)
  {
    const size_t size=sizes[i];
    size_t expected_allocations_after_free=expected_allocations;
    expected_allocations+=size;

    {
      void* p=_aligned_malloc_dbg(size, 32, __FILE__, __LINE__);
      ASSERT_TRUE(p);
      ASSERT_TRUE(((size_t)p % 32) == 0);
#ifdef _DEBUG
      ASSERT_EQ(_aligned_msize_dbg(p, 32, 0), size);
#endif

#ifdef _DEBUG
      ASSERT_EQ(expected_allocations, buginsight_internal_allocated_bytes("_aligned_malloc_dbg"));
      allocations_dbg.emplace_back(p, "_aligned_malloc_dbg", expected_allocations_after_free);
#else
      ASSERT_EQ(expected_allocations, buginsight_internal_allocated_bytes("_aligned_malloc"));
      allocations_dbg.emplace_back(p, "_aligned_malloc", expected_allocations_after_free);
#endif
    }

    {
      void* p=_aligned_offset_malloc_dbg(size, 32, size?size - 1:0, __FILE__, __LINE__);
      ASSERT_TRUE(p);
#ifdef _DEBUG
      ASSERT_EQ(_aligned_msize_dbg(p, 32, size?size - 1:0), size);
#endif

#ifdef _DEBUG
      ASSERT_EQ(expected_allocations, buginsight_internal_allocated_bytes("_aligned_offset_malloc_dbg"));
      allocations_dbg.emplace_back(p, "_aligned_offset_malloc_dbg", expected_allocations_after_free);
#else
      ASSERT_EQ(expected_allocations, buginsight_internal_allocated_bytes("_aligned_offset_malloc"));
      allocations_dbg.emplace_back(p, "_aligned_offset_malloc", expected_allocations_after_free);
#endif
    }

    {
      void* p=_aligned_realloc_dbg(nullptr, size, 32, __FILE__, __LINE__);
      ASSERT_TRUE(p);
      ASSERT_TRUE(((size_t)p % 32) == 0);
#ifdef _DEBUG
      ASSERT_EQ(_aligned_msize_dbg(p, 32, 0), size);
#endif

#ifdef _DEBUG
      ASSERT_EQ(expected_allocations, buginsight_internal_allocated_bytes("_aligned_realloc_dbg"));
      allocations_dbg.emplace_back(p, "_aligned_realloc_dbg", expected_allocations_after_free);
#else
      ASSERT_EQ(expected_allocations, buginsight_internal_allocated_bytes("_aligned_realloc"));
      allocations_dbg.emplace_back(p, "_aligned_realloc", expected_allocations_after_free);
#endif
    }

    {
      void* p=_aligned_offset_realloc_dbg(nullptr, size, 32, size?size - 1:0, __FILE__, __LINE__);
      ASSERT_TRUE(p);
#ifdef _DEBUG
      ASSERT_EQ(_aligned_msize_dbg(p, 32, size?size - 1:0), size);
#endif

#ifdef _DEBUG
      ASSERT_EQ(expected_allocations, buginsight_internal_allocated_bytes("_aligned_offset_realloc_dbg"));
      allocations_dbg.emplace_back(p, "_aligned_offset_realloc_dbg", expected_allocations_after_free);
#else
      ASSERT_EQ(expected_allocations, buginsight_internal_allocated_bytes("_aligned_offset_realloc"));
      allocations_dbg.emplace_back(p, "_aligned_offset_realloc", expected_allocations_after_free);
#endif
    }

    {
      void* p=_aligned_recalloc_dbg(nullptr, size, sizeof(uint8_t), 32, __FILE__, __LINE__);
      ASSERT_TRUE(p);
      ASSERT_TRUE(((size_t)p % 32) == 0);
#ifdef _DEBUG
      //ASSERT_EQ(_aligned_msize_dbg(p, 32, size?size-1:0), size);
#endif

#ifdef _DEBUG
      ASSERT_EQ(expected_allocations, buginsight_internal_allocated_bytes("_aligned_recalloc_dbg"));
      allocations_dbg.emplace_back(p, "_aligned_recalloc_dbg", expected_allocations_after_free);
#else
      ASSERT_EQ(expected_allocations, buginsight_internal_allocated_bytes("_aligned_recalloc"));
      allocations_dbg.emplace_back(p, "_aligned_recalloc", expected_allocations_after_free);
#endif
    }

    {
      void* p=_aligned_offset_recalloc_dbg(nullptr, size, sizeof(uint8_t), 32, size?size - 1:0, __FILE__, __LINE__);
      ASSERT_TRUE(p);
#ifdef _DEBUG
      //ASSERT_EQ(_aligned_msize_dbg(p, 32, size?size-1:0), size);
#endif

#ifdef _DEBUG
      ASSERT_EQ(expected_allocations, buginsight_internal_allocated_bytes("_aligned_offset_recalloc_dbg"));
      allocations_dbg.emplace_back(p, "_aligned_offset_recalloc_dbg", expected_allocations_after_free);
#else
      ASSERT_EQ(expected_allocations, buginsight_internal_allocated_bytes("_aligned_offset_recalloc"));
      allocations_dbg.emplace_back(p, "_aligned_offset_recalloc", expected_allocations_after_free);
#endif
    }

  }

  std::reverse(allocations_dbg.begin(), allocations_dbg.end());

  for(const allocated_t& allocted : allocations_dbg)
  {
    _aligned_free_dbg(allocted.m_pointer);
    ASSERT_EQ(allocted.m_expected_alloc_after_free, buginsight_internal_allocated_bytes(allocted.m_function.data()));
  }
}


UTEST_F(aligned_malloc_tests, size_too_large_exit_0)
{
  void* p=_aligned_malloc(_HEAP_MAXREQ - 2 + 1, 32);
  ASSERT_TRUE(p == nullptr);
  p=_aligned_malloc_dbg(_HEAP_MAXREQ - 2 + 1, 32, __FILE__, __LINE__);
  ASSERT_TRUE(p == nullptr);
  p=_aligned_offset_malloc(_HEAP_MAXREQ - 2 + 1, 32, 100);
  ASSERT_TRUE(p == nullptr);
  p=_aligned_offset_malloc_dbg(_HEAP_MAXREQ - 2 + 1, 32, 100, __FILE__, __LINE__);
  ASSERT_TRUE(p == nullptr);

  p=_aligned_realloc(0, _HEAP_MAXREQ - 2 + 1, 32);
  ASSERT_TRUE(p == nullptr);
  p=_aligned_realloc_dbg(0, _HEAP_MAXREQ - 2 + 1, 32, __FILE__, __LINE__);
  ASSERT_TRUE(p == nullptr);
  p=_aligned_offset_realloc(0, _HEAP_MAXREQ - 2 + 1, 32, 100);
  ASSERT_TRUE(p == nullptr);
  p=_aligned_offset_realloc_dbg(0, _HEAP_MAXREQ - 2 + 1, 32, 100, __FILE__, __LINE__);
  ASSERT_TRUE(p == nullptr);

  p=_aligned_recalloc(nullptr, _HEAP_MAXREQ - 2 + 1, 1, 32);
  ASSERT_TRUE(p == nullptr);
  p=_aligned_recalloc_dbg(0, _HEAP_MAXREQ - 2 + 1, 1, 32, __FILE__, __LINE__);
  ASSERT_TRUE(p == nullptr);
  p=_aligned_offset_recalloc(nullptr, _HEAP_MAXREQ - 2 + 1, 1, 32, 100);
  ASSERT_TRUE(p == nullptr);
  p=_aligned_offset_recalloc_dbg(0, _HEAP_MAXREQ - 2 + 1, 1, 32, 100, __FILE__, __LINE__);
  ASSERT_TRUE(p == nullptr);
}

UTEST_F(aligned_malloc_tests, test_api_aligned_recalloc_uint64_t_exit_0)
{
  {
    void* p=_aligned_recalloc(nullptr, 10, sizeof(uint64_t), 32);
    ASSERT_TRUE(p);
    ASSERT_EQ(10 * sizeof(uint64_t), buginsight_internal_allocated_bytes("_aligned_recalloc"));

    void* p2=_aligned_recalloc(p, 15, sizeof(uint64_t), 32);
    ASSERT_TRUE(p2);
    ASSERT_EQ(15 * sizeof(uint64_t), buginsight_internal_allocated_bytes("_aligned_recalloc"));
    _aligned_free(p2);
  }

  {
    void* p=_aligned_recalloc_dbg(nullptr, 10, sizeof(uint64_t), 32, __FILE__, __LINE__);
    ASSERT_TRUE(p);
#ifdef _DEBUG
    ASSERT_EQ(10 * sizeof(uint64_t), buginsight_internal_allocated_bytes("_aligned_recalloc_dbg"));
#else
    ASSERT_EQ(10 * sizeof(uint64_t), buginsight_internal_allocated_bytes("_aligned_recalloc"));
#endif

    void* p2=_aligned_recalloc_dbg(p, 15, sizeof(uint64_t), 32, __FILE__, __LINE__);
    ASSERT_TRUE(p2);

#ifdef _DEBUG
    ASSERT_EQ(15 * sizeof(uint64_t), buginsight_internal_allocated_bytes("_aligned_recalloc_dbg"));
#else
    ASSERT_EQ(15 * sizeof(uint64_t), buginsight_internal_allocated_bytes("_aligned_recalloc"));
#endif
    _aligned_free_dbg(p2);
  }
}

UTEST_F(aligned_malloc_tests, test_api_aligned_offset_recalloc_uint64_t_exit_0)
{
  {
    void* p=_aligned_offset_recalloc(nullptr, 10, sizeof(uint64_t), 32, 5);
    ASSERT_TRUE(p);
    ASSERT_EQ(10 * sizeof(uint64_t), buginsight_internal_allocated_bytes("_aligned_offset_recalloc"));

    void* p2=_aligned_offset_recalloc(p, 15, sizeof(uint64_t), 32, 100);
    ASSERT_TRUE(p2);
    ASSERT_EQ(15 * sizeof(uint64_t), buginsight_internal_allocated_bytes("_aligned_offset_recalloc"));
    _aligned_free(p2);
  }

  {
    void* p=_aligned_offset_recalloc_dbg(nullptr, 10, sizeof(uint64_t), 32, 5, __FILE__, __LINE__);
    ASSERT_TRUE(p);

#ifdef _DEBUG
    ASSERT_EQ(10 * sizeof(uint64_t), buginsight_internal_allocated_bytes("_aligned_offset_recalloc_dbg"));
#else
    ASSERT_EQ(10 * sizeof(uint64_t), buginsight_internal_allocated_bytes("_aligned_offset_recalloc"));
#endif

    void* p2=_aligned_offset_recalloc_dbg(p, 15, sizeof(uint64_t), 32, 5, __FILE__, __LINE__);
    ASSERT_TRUE(p2);

#ifdef _DEBUG
    ASSERT_EQ(15 * sizeof(uint64_t), buginsight_internal_allocated_bytes("_aligned_offset_recalloc_dbg"));
#else
    ASSERT_EQ(15 * sizeof(uint64_t), buginsight_internal_allocated_bytes("_aligned_offset_recalloc"));
#endif
    _aligned_free_dbg(p2);
  }
}

UTEST_F(aligned_malloc_tests, test_api__aligned_realloc_exit_0)
{
  void* p=_aligned_malloc(10, 32);
  ASSERT_TRUE(p);
  ASSERT_EQ(10, buginsight_internal_allocated_bytes("_aligned_malloc"));


  void* p2=_aligned_realloc(p, 20, 32);
  ASSERT_TRUE(p2);

#ifdef _DEBUG
  ASSERT_EQ(_aligned_msize(p2, 32, 0), 20);
#endif

  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_malloc"));
  ASSERT_EQ(20, buginsight_internal_allocated_bytes("_aligned_realloc"));

  _aligned_free(p2);
}

UTEST_F(aligned_malloc_tests, test_api__aligned_offset_realloc_resize_10_20_exit_0)
{
  void* p=_aligned_offset_malloc(10, 32, 5);
  ASSERT_TRUE(p);
  ASSERT_EQ(10, buginsight_internal_allocated_bytes("_aligned_offset_malloc"));


  void* p2=_aligned_offset_realloc(p, 20, 32, 5);
  ASSERT_TRUE(p2);
#ifdef _DEBUG
  size_t actuell_size=_aligned_msize(p2, 32, 5);
  ASSERT_EQ(actuell_size, 20);
#endif

  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_offset_malloc"));
  ASSERT_EQ(20, buginsight_internal_allocated_bytes("_aligned_offset_realloc"));

  _aligned_free(p2);
}

UTEST_F(aligned_malloc_tests, test_api__aligned_offset_realloc_dbg_resize_10_20_exit_0)
{
  void* p=_aligned_offset_malloc_dbg(10, 32, 5, __FILE__, __LINE__);
  ASSERT_TRUE(p);
#ifdef _DEBUG
  ASSERT_EQ(10, buginsight_internal_allocated_bytes("_aligned_offset_malloc_dbg"));
#else
  ASSERT_EQ(10, buginsight_internal_allocated_bytes("_aligned_offset_malloc"));
#endif

  void* p2=_aligned_offset_realloc_dbg(p, 20, 32, 5, __FILE__, __LINE__);
  ASSERT_TRUE(p2);

#ifdef _DEBUG
  size_t actuell_size=_aligned_msize(p2, 32, 5);
  ASSERT_EQ(actuell_size, 20);
#endif

#ifdef _DEBUG
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_offset_malloc_dbg"));
  ASSERT_EQ(20, buginsight_internal_allocated_bytes("_aligned_offset_realloc_dbg"));
#else
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_offset_malloc"));
  ASSERT_EQ(20, buginsight_internal_allocated_bytes("_aligned_offset_realloc"));
#endif

  _aligned_free(p2);
}


UTEST_F(aligned_malloc_tests, test_api_realloc_dbg_resize_10_to_20_exit_0)
{
  void* p=_aligned_malloc_dbg(10, 32, __FILE__, __LINE__);
  ASSERT_TRUE(p);

#ifdef _DEBUG
  ASSERT_EQ(10, buginsight_internal_allocated_bytes("_aligned_malloc_dbg"));
#else
  ASSERT_EQ(10, buginsight_internal_allocated_bytes("_aligned_malloc"));
#endif



  void* p2=_aligned_realloc_dbg(p, 20, 32, __FILE__, __LINE__);
  ASSERT_TRUE(p2);

#ifdef _DEBUG
  ASSERT_EQ(_aligned_msize_dbg(p2, 32, 0), 20);
#endif

#ifdef _DEBUG
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_malloc_dbg"));
  ASSERT_EQ(20, buginsight_internal_allocated_bytes("_aligned_realloc_dbg"));
#else
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_malloc"));
  ASSERT_EQ(20, buginsight_internal_allocated_bytes("_aligned_realloc"));
#endif

  _aligned_free_dbg(p2);
}

UTEST_F(aligned_malloc_tests, test_api_realloc_resize_10_to_5_exit_0)
{
  void* p=_aligned_malloc(10, 32);
  ASSERT_TRUE(p);
  ASSERT_EQ(10, buginsight_internal_allocated_bytes("_aligned_malloc"));

  void* p2=_aligned_realloc(p, 5, 32);
  ASSERT_TRUE(p2);
#ifdef _DEBUG
  ASSERT_EQ(_aligned_msize(p2, 32, 0), 5);
#endif

  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_malloc"));
  ASSERT_EQ(5, buginsight_internal_allocated_bytes("_aligned_realloc"));

  _aligned_free(p2);
}

UTEST_F(aligned_malloc_tests, test_api_realloc_dbg_resize_10_to_5_exit_0)
{
  void* p=_aligned_malloc_dbg(10, 32, __FILE__, __LINE__);
  ASSERT_TRUE(p);

#ifdef _DEBUG
  ASSERT_EQ(10, buginsight_internal_allocated_bytes("_aligned_malloc_dbg"));
#else
  ASSERT_EQ(10, buginsight_internal_allocated_bytes("_aligned_malloc"));
#endif

  void* p2=_aligned_realloc_dbg(p, 5, 32, __FILE__, __LINE__);
  ASSERT_TRUE(p2);
#ifdef _DEBUG
  ASSERT_EQ(_aligned_msize_dbg(p2, 32, 0), 5);
#endif

#ifdef _DEBUG
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_malloc_dbg"));
  ASSERT_EQ(5, buginsight_internal_allocated_bytes("_aligned_realloc_dbg"));
#else
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_malloc"));
  ASSERT_EQ(5, buginsight_internal_allocated_bytes("_aligned_realloc"));
#endif

  _aligned_free_dbg(p2);
}

UTEST_F(aligned_malloc_tests, test_api_realloc_size_zero_exit_0)
{
  void* p=_aligned_malloc(10000, 32);
  ASSERT_TRUE(p);
  ASSERT_EQ(10000, buginsight_internal_allocated_bytes("_aligned_malloc"));

  void* p2=_aligned_realloc(p, 0, 32);
  ASSERT_TRUE(p2 == nullptr);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_malloc"));
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_realloc"));
}

UTEST_F(aligned_malloc_tests, test_api_realloc_dbg_size_zero_exit_0)
{
  void* p=_aligned_malloc_dbg(10000, 32, __FILE__, __LINE__);
  ASSERT_TRUE(p);

#ifdef _DEBUG
  ASSERT_EQ(10000, buginsight_internal_allocated_bytes("_aligned_malloc_dbg"));
#else
  ASSERT_EQ(10000, buginsight_internal_allocated_bytes("_aligned_malloc"));
#endif

  void* p2=_aligned_realloc_dbg(p, 0, 32, __FILE__, __LINE__);
  ASSERT_TRUE(p2 == nullptr);

#ifdef _DEBUG
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_malloc_dbg"));
#else
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_malloc"));
#endif
}

UTEST_F(aligned_malloc_tests, test_api_offset_realloc_size_zero_exit_0)
{
  void* p=_aligned_offset_malloc(10000, 32, 100);
  ASSERT_TRUE(p);
  ASSERT_EQ(10000, buginsight_internal_allocated_bytes("_aligned_offset_malloc"));

  void* p2=_aligned_offset_realloc(p, 0, 32, 100);
  ASSERT_TRUE(p2 == nullptr);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_offset_malloc"));
}

UTEST_F(aligned_malloc_tests, test_api_offset_realloc_dbg_size_zero_exit_0)
{
  void* p=_aligned_offset_malloc_dbg(10000, 32, 100, __FILE__, __LINE__);
  ASSERT_TRUE(p);

#ifdef _DEBUG
  ASSERT_EQ(10000, buginsight_internal_allocated_bytes("_aligned_offset_malloc_dbg"));
#else
  ASSERT_EQ(10000, buginsight_internal_allocated_bytes("_aligned_offset_malloc"));
#endif

  void* p2=_aligned_offset_realloc_dbg(p, 0, 32, 100, __FILE__, __LINE__);
  ASSERT_TRUE(p2 == nullptr);

#ifdef _DEBUG
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_offset_realloc_dbg"));
#else
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_offset_realloc"));
#endif
}

UTEST_F(aligned_malloc_tests, test_api_recalloc_size_zero_exit_0)
{
  void* p=_aligned_recalloc(0, 10, 20, 32);
  ASSERT_TRUE(p);
  ASSERT_EQ(10 * 20, buginsight_internal_allocated_bytes("_aligned_recalloc"));

  void* p2=_aligned_recalloc(p, 0, 0, 32);
  ASSERT_TRUE(p2 == nullptr);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_recalloc"));
}

UTEST_F(aligned_malloc_tests, test_api_recalloc_dgb_size_zero_exit_0)
{
  void* p=_aligned_recalloc_dbg(0, 10, 20, 32, __FILE__, __LINE__);
  ASSERT_TRUE(p);

#ifdef _DEBUG
  ASSERT_EQ(10 * 20, buginsight_internal_allocated_bytes("_aligned_recalloc_dbg"));
#else
  ASSERT_EQ(10 * 20, buginsight_internal_allocated_bytes("_aligned_recalloc"));
#endif

  void* p2=_aligned_recalloc_dbg(p, 0, 0, 32, __FILE__, __LINE__);
  ASSERT_TRUE(p2 == nullptr);

#ifdef _DEBUG
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_recalloc_dbg"));
#else
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_recalloc"));
#endif
}

UTEST_F(aligned_malloc_tests, test_api_offset_recalloc_size_zero_exit_0)
{
  void* p=_aligned_offset_recalloc(0, 10, 20, 32, 5);
  ASSERT_TRUE(p);
  ASSERT_EQ(10 * 20, buginsight_internal_allocated_bytes("_aligned_offset_recalloc"));

  void* p2=_aligned_offset_recalloc(p, 0, 0, 32, 5);
  ASSERT_TRUE(p2 == nullptr);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_offset_recalloc"));
}

UTEST_F(aligned_malloc_tests, test_api_offset_recalloc_dgb_size_zero_exit_0)
{
  void* p=_aligned_offset_recalloc_dbg(0, 10, 20, 32, 5, __FILE__, __LINE__);
  ASSERT_TRUE(p);

#ifdef _DEBUG
  ASSERT_EQ(10 * 20, buginsight_internal_allocated_bytes("_aligned_offset_recalloc_dbg"));
#else
  ASSERT_EQ(10 * 20, buginsight_internal_allocated_bytes("_aligned_offset_recalloc"));
#endif

  void* p2=_aligned_offset_recalloc_dbg(p, 0, 0, 32, 5, __FILE__, __LINE__);
  ASSERT_TRUE(p2 == nullptr);

#ifdef _DEBUG
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_offset_recalloc_dbg"));
#else
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_offset_recalloc"));
#endif
}

UTEST_F(aligned_malloc_tests, test_api_realloc_block_and_size_zero_exit_0)
{
  void* p2=_aligned_realloc(0, 0, 32);
  ASSERT_TRUE(p2);

#ifdef _DEBUG
  ASSERT_EQ(_aligned_msize(p2, 32, 0), 0);
#endif

  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_realloc"));

  _aligned_free(p2);
}

UTEST_F(aligned_malloc_tests, test_api_realloc_dbg_block_and_size_zero_exit_0)
{
  void* p2=_aligned_realloc_dbg(0, 0, 32, __FILE__, __LINE__);
  ASSERT_TRUE(p2);

#ifdef _DEBUG
  ASSERT_EQ(_aligned_msize_dbg(p2, 32, 0), 0);
#endif

#ifdef _DEBUG
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_realloc_dbg"));
#else
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_realloc"));
#endif
  _aligned_free_dbg(p2);
}

UTEST_F(aligned_malloc_tests, test_api_recalloc_block_and_size_zero_exit_0)
{
  void* p2=_aligned_recalloc(0, 0, 0, 32);
  ASSERT_TRUE(p2);

#ifdef _DEBUG
  ASSERT_EQ(_aligned_msize(p2, 32, 0), 0);
#endif
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_recalloc"));
  _aligned_free(p2);
}

UTEST_F(aligned_malloc_tests, test_api_recalloc_dbg_block_and_size_zero_exit_0)
{
  void* p2=_aligned_recalloc_dbg(0, 0, 0, 32, __FILE__, __LINE__);
  ASSERT_TRUE(p2);

#ifdef _DEBUG
  ASSERT_EQ(_aligned_msize_dbg(p2, 32, 0), 0);
#endif

#ifdef _DEBUG
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_recalloc_dbg"));
#else
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_recalloc"));
#endif
  _aligned_free_dbg(p2);
}

UTEST_F(aligned_malloc_tests, test_api_realloc_offset_block_and_size_zero_exit_0)
{
  void* p2=_aligned_offset_realloc(0, 0, 32, 0);
  ASSERT_TRUE(p2);

#ifdef _DEBUG
  ASSERT_EQ(_aligned_msize(p2, 32, 0), 0);
#endif

  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_offset_realloc"));
  _aligned_free(p2);
}

UTEST_F(aligned_malloc_tests, test_api_realloc_offse_dbg_block_and_size_zero_exit_0)
{
  void* p2=_aligned_offset_realloc_dbg(0, 0, 32, 0, __FILE__, __LINE__);
  ASSERT_TRUE(p2);

#ifdef _DEBUG
  ASSERT_EQ(_aligned_msize_dbg(p2, 32, 0), 0);
#endif

#ifdef _DEBUG
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_offset_realloc_dbg"));
#else
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_offset_realloc"));
#endif
  _aligned_free_dbg(p2);
}

UTEST_F(aligned_malloc_tests, test_api_recalloc_offset_block_and_size_zero_exit_0)
{
  void* p2=_aligned_offset_recalloc(0, 0, 0, 32, 0);
  ASSERT_TRUE(p2);

#ifdef _DEBUG
  ASSERT_EQ(_aligned_msize(p2, 32, 0), 0);
#endif
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_offset_recalloc"));
  _aligned_free(p2);
}

UTEST_F(aligned_malloc_tests, test_api_recalloc_offset_dbg_block_and_size_zero_exit_0)
{
  void* p2=_aligned_offset_recalloc_dbg(0, 0, 0, 32, 0, __FILE__, __LINE__);
  ASSERT_TRUE(p2);
#ifdef _DEBUG
  ASSERT_EQ(_aligned_msize_dbg(p2, 32, 0), 0);
#endif

#ifdef _DEBUG
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_offset_recalloc_dbg"));
#else
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_offset_recalloc"));
#endif
  _aligned_free_dbg(p2);
}

UTEST_F(aligned_malloc_tests, test_api__recalloc_recalloc_10_to_20_exit_0)
{
  void* p=_aligned_malloc(10, 32);
  ASSERT_TRUE(p);
  ASSERT_EQ(10, buginsight_internal_allocated_bytes("_aligned_malloc"));

  void* p2=_aligned_recalloc(p, 20, 1, 32);
  ASSERT_TRUE(p2);
#ifdef _DEBUG
  ASSERT_EQ(_aligned_msize(p2, 32, 0), 20);
#endif

  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_malloc"));
  ASSERT_EQ(20, buginsight_internal_allocated_bytes("_aligned_recalloc"));

  _aligned_free(p2);
}

UTEST_F(aligned_malloc_tests, test_api_recalloc_dbg_resize_10_to_20_exit_0)
{
  void* p=_aligned_malloc_dbg(10, 32, __FILE__, __LINE__);
  ASSERT_TRUE(p);

#ifdef _DEBUG
  ASSERT_EQ(10, buginsight_internal_allocated_bytes("_aligned_malloc_dbg"));
#else
  ASSERT_EQ(10, buginsight_internal_allocated_bytes("_aligned_malloc"));
#endif

  void* p2=_aligned_recalloc_dbg(p, 20, 1, 32, __FILE__, __LINE__);
  ASSERT_TRUE(p2);
#ifdef _DEBUG
  ASSERT_EQ(_aligned_msize_dbg(p2, 32, 0), 20);
#endif

#ifdef _DEBUG
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_malloc_dbg"));
  ASSERT_EQ(20, buginsight_internal_allocated_bytes("_aligned_recalloc_dbg"));
#else
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_malloc"));
  ASSERT_EQ(20, buginsight_internal_allocated_bytes("_aligned_recalloc"));
#endif

  _aligned_free_dbg(p2);
}

UTEST_F(aligned_malloc_tests, test_api__recalloc_recalloc_10_to_5_exit_0)
{
  void* p=_aligned_malloc(10, 32);
  ASSERT_TRUE(p);
  ASSERT_EQ(10, buginsight_internal_allocated_bytes("_aligned_malloc"));

  void* p2=_aligned_recalloc(p, 5, 1, 32);
  ASSERT_TRUE(p2);

#ifdef _DEBUG
  ASSERT_EQ(_aligned_msize(p2, 32, 0), 5);
#endif

  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_malloc"));
  ASSERT_EQ(5, buginsight_internal_allocated_bytes("_aligned_recalloc"));

  _aligned_free(p2);
}

UTEST_F(aligned_malloc_tests, test_api_recalloc_dbg_resize_10_to_5_exit_0)
{
  void* p=_aligned_malloc_dbg(10, 32, __FILE__, __LINE__);
  ASSERT_TRUE(p);

#ifdef _DEBUG
  ASSERT_EQ(10, buginsight_internal_allocated_bytes("_aligned_malloc_dbg"));
#else
  ASSERT_EQ(10, buginsight_internal_allocated_bytes("_aligned_malloc"));
#endif

  void* p2=_aligned_recalloc_dbg(p, 5, 1, 32, __FILE__, __LINE__);
  ASSERT_TRUE(p2);
#ifdef _DEBUG
  ASSERT_EQ(_aligned_msize_dbg(p2, 32, 0), 5);
#endif

#ifdef _DEBUG
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_malloc_dbg"));
  ASSERT_EQ(5, buginsight_internal_allocated_bytes("_aligned_recalloc_dbg"));
#else
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_aligned_malloc"));
  ASSERT_EQ(5, buginsight_internal_allocated_bytes("_aligned_recalloc"));
#endif

  _aligned_free_dbg(p2);
}

UTEST_F(aligned_malloc_tests, test_api__aligned_free_zero_exit_0)
{
  _aligned_free(0);
  _aligned_free_dbg(0);
}






