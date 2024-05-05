// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <cassert>

#include <cstdlib>
#include <vector>
#include <array>
#include <allocator.hpp>
#include <thread>

#include <utest.h>

extern size_t buginsight_internal_allocated_bytes(const char* p_function);

struct malloc_tests
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

UTEST_F_SETUP(malloc_tests)
{
}

UTEST_F_TEARDOWN(malloc_tests)
{
}

UTEST_F(malloc_tests, basic_exit_0)
{
  const size_t sizes[]{0, 1, 10, 1000, 1073741824llu};

  std::vector<allocated_t, meta_allocator<allocated_t>> allocations;
  std::vector<allocated_t, meta_allocator<allocated_t>> allocations_dbg;
  std::vector<void*, meta_allocator<void*>> pointer_dbg;

  size_t expected_allocations=0;

  for(size_t i=0; i < _countof(sizes); ++i)
  {
    const size_t size=sizes[i];
    size_t expected_allocations_after_free=expected_allocations;

    expected_allocations+=size;

    {
      void* p=malloc(size);
      ASSERT_TRUE(p);

#ifdef _DEBUG
      ASSERT_EQ(_msize(p), size);
#endif

      size_t real_alloc=buginsight_internal_allocated_bytes("malloc");

      ASSERT_EQ(expected_allocations, real_alloc);
      allocations.emplace_back(p, "malloc", expected_allocations_after_free);
    }

    {
      void* p=realloc(nullptr, size);
      ASSERT_TRUE(p);
#ifdef _DEBUG
      ASSERT_EQ(_msize(p), size);
#endif
      ASSERT_EQ(expected_allocations, buginsight_internal_allocated_bytes("realloc"));
      allocations.emplace_back(p, "realloc", expected_allocations_after_free);
    }

    {
      void* p=calloc(size, sizeof(uint8_t));
      ASSERT_TRUE(p);
#ifdef _DEBUG
      ASSERT_EQ(_msize(p), size * sizeof(uint8_t));
#endif
      ASSERT_EQ(expected_allocations, buginsight_internal_allocated_bytes("calloc"));
      allocations.emplace_back(p, "calloc", expected_allocations_after_free);
    }

    {
      void* p=_recalloc(nullptr, size, sizeof(uint8_t));
      ASSERT_TRUE(p);
#ifdef _DEBUG
      ASSERT_EQ(_msize(p), size * sizeof(uint8_t));
#endif
      ASSERT_EQ(expected_allocations, buginsight_internal_allocated_bytes("_recalloc"));
      allocations.emplace_back(p, "_recalloc", expected_allocations_after_free);
    }

  }

  std::reverse(allocations.begin(), allocations.end());
  std::reverse(allocations_dbg.begin(), allocations_dbg.end());

  for(const allocated_t& allocted : allocations)
  {
    free(allocted.m_pointer);

    size_t s=buginsight_internal_allocated_bytes(allocted.m_function.data());

    ASSERT_EQ(allocted.m_expected_alloc_after_free, s);
  }

  for(const allocated_t& allocted : allocations_dbg)
  {
    _free_dbg(allocted.m_pointer, _NORMAL_BLOCK);
    ASSERT_EQ(allocted.m_expected_alloc_after_free, buginsight_internal_allocated_bytes(allocted.m_function.data()));
  }
}

UTEST_F(malloc_tests, basic_dbg_exit_0)
{
  const size_t sizes[]{0, 1, 10, 1000, 1073741824llu};

  std::vector<allocated_t, meta_allocator<allocated_t>> allocations;
  std::vector<allocated_t, meta_allocator<allocated_t>> allocations_dbg;
  std::vector<void*, meta_allocator<void*>> pointer_dbg;

  size_t expected_allocations=0;

  for(size_t i=0; i < _countof(sizes); ++i)
  {
    const size_t size=sizes[i];
    size_t expected_allocations_after_free=expected_allocations;

    expected_allocations+=size;

    {
      void* p=_malloc_dbg(size, _NORMAL_BLOCK, __FILE__, __LINE__);
      ASSERT_TRUE(p);
#ifdef _DEBUG
      ASSERT_EQ(_msize(p), size);
#endif
#ifdef _DEBUG
      ASSERT_EQ(expected_allocations, buginsight_internal_allocated_bytes("_malloc_dbg"));
      allocations_dbg.emplace_back(p, "_malloc_dbg", expected_allocations_after_free);
#else
      ASSERT_EQ(expected_allocations, buginsight_internal_allocated_bytes("malloc"));
      allocations_dbg.emplace_back(p, "malloc", expected_allocations_after_free);
#endif
    }

    {
      void* p=_realloc_dbg(nullptr, size, _NORMAL_BLOCK, __FILE__, __LINE__);
      ASSERT_TRUE(p);
#ifdef _DEBUG
      ASSERT_EQ(_msize(p), size);
#endif
#ifdef _DEBUG
      ASSERT_EQ(expected_allocations, buginsight_internal_allocated_bytes("_realloc_dbg"));
      allocations_dbg.emplace_back(p, "_realloc_dbg", expected_allocations_after_free);
#else
      ASSERT_EQ(expected_allocations, buginsight_internal_allocated_bytes("realloc"));
      allocations_dbg.emplace_back(p, "realloc", expected_allocations_after_free);
#endif
    }

    {
      void* p=_calloc_dbg(size, sizeof(uint8_t), _NORMAL_BLOCK, __FILE__, __LINE__);
      ASSERT_TRUE(p);
#ifdef _DEBUG
      ASSERT_EQ(_msize(p), size * sizeof(uint8_t));
#endif

#ifdef _DEBUG
      ASSERT_EQ(expected_allocations, buginsight_internal_allocated_bytes("_calloc_dbg"));
      allocations_dbg.emplace_back(p, "_calloc_dbg", expected_allocations_after_free);
#else
      ASSERT_EQ(expected_allocations, buginsight_internal_allocated_bytes("calloc"));
      allocations_dbg.emplace_back(p, "calloc", expected_allocations_after_free);
#endif

    }

    {
      void* p=_recalloc_dbg(nullptr, size, sizeof(uint8_t), _NORMAL_BLOCK, __FILE__, __LINE__);
      ASSERT_TRUE(p);
#ifdef _DEBUG
      ASSERT_EQ(_msize(p), size * sizeof(uint8_t));
#endif

#ifdef _DEBUG
      ASSERT_EQ(expected_allocations, buginsight_internal_allocated_bytes("_recalloc_dbg"));
      allocations_dbg.emplace_back(p, "_recalloc_dbg", expected_allocations_after_free);
#else
      ASSERT_EQ(expected_allocations, buginsight_internal_allocated_bytes("_recalloc"));
      allocations_dbg.emplace_back(p, "_recalloc", expected_allocations_after_free);
#endif
    }

  }

  std::reverse(allocations.begin(), allocations.end());
  std::reverse(allocations_dbg.begin(), allocations_dbg.end());

  for(const allocated_t& allocted : allocations)
  {
    free(allocted.m_pointer);

    size_t s=buginsight_internal_allocated_bytes(allocted.m_function.data());

    ASSERT_EQ(allocted.m_expected_alloc_after_free, s);
  }

  for(const allocated_t& allocted : allocations_dbg)
  {
    _free_dbg(allocted.m_pointer, _NORMAL_BLOCK);
    ASSERT_EQ(allocted.m_expected_alloc_after_free, buginsight_internal_allocated_bytes(allocted.m_function.data()));
  }
}

UTEST_F(malloc_tests, size_too_large_exit_0)
{
  void* p=malloc(_HEAP_MAXREQ - 2 + 1);
  ASSERT_TRUE(p == nullptr);

  p=_malloc_dbg(_HEAP_MAXREQ - 2 + 1, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p == nullptr);

  p=realloc(0, _HEAP_MAXREQ - 2 + 1);
  ASSERT_TRUE(p == nullptr);

  p=_realloc_dbg(0, _HEAP_MAXREQ - 2 + 1, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p == nullptr);

  p=_recalloc(0, _HEAP_MAXREQ - 2 + 1, 1);
  ASSERT_TRUE(p == nullptr);

  p=calloc(_HEAP_MAXREQ - 2 + 1, 1);
  ASSERT_TRUE(p == nullptr);

  p=_calloc_dbg(_HEAP_MAXREQ - 2 + 1, 1, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p == nullptr);

  p=_recalloc(nullptr, _HEAP_MAXREQ - 2 + 1, 1);
  ASSERT_TRUE(p == nullptr);

  p=_recalloc_dbg(0, _HEAP_MAXREQ - 2 + 1, 1, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p == nullptr);
}

UTEST_F(malloc_tests, calloc_uint64_t_exit_0)
{
  {
    void* p=calloc(10, sizeof(uint64_t));
    ASSERT_TRUE(p);

    ASSERT_EQ(10 * sizeof(uint64_t), buginsight_internal_allocated_bytes("calloc"));
    free(p);
  }

  {
    void* p=_calloc_dbg(10, sizeof(uint64_t), _NORMAL_BLOCK, __FILE__, __LINE__);
    ASSERT_TRUE(p);
#ifdef _DEBUG
    ASSERT_EQ(10 * sizeof(uint64_t), buginsight_internal_allocated_bytes("_calloc_dbg"));
#else
    ASSERT_EQ(10 * sizeof(uint64_t), buginsight_internal_allocated_bytes("calloc"));
#endif
    _free_dbg(p, _NORMAL_BLOCK);
  }

  {
    void* p=_recalloc(nullptr, 10, sizeof(uint64_t));
    ASSERT_TRUE(p);
    ASSERT_EQ(10 * sizeof(uint64_t), buginsight_internal_allocated_bytes("_recalloc"));

    void* p2=_recalloc(p, 15, sizeof(uint64_t));
    ASSERT_TRUE(p2);
    ASSERT_EQ(15 * sizeof(uint64_t), buginsight_internal_allocated_bytes("_recalloc"));
    free(p2);
  }

  {
    void* p=_recalloc_dbg(nullptr, 10, sizeof(uint64_t), _NORMAL_BLOCK, __FILE__, __LINE__);
    ASSERT_TRUE(p);

#ifdef _DEBUG
    ASSERT_EQ(10 * sizeof(uint64_t), buginsight_internal_allocated_bytes("_recalloc_dbg"));
#else
    ASSERT_EQ(10 * sizeof(uint64_t), buginsight_internal_allocated_bytes("_recalloc"));
#endif

    void* p2=_recalloc_dbg(p, 15, sizeof(uint64_t), _NORMAL_BLOCK, __FILE__, __LINE__);
    ASSERT_TRUE(p2);
#ifdef _DEBUG
    ASSERT_EQ(15 * sizeof(uint64_t), buginsight_internal_allocated_bytes("_recalloc_dbg"));
#else
    ASSERT_EQ(15 * sizeof(uint64_t), buginsight_internal_allocated_bytes("_recalloc"));
#endif
    _free_dbg(p2, _NORMAL_BLOCK);
  }
}

UTEST_F(malloc_tests, test_api_realloc_resize_10_to_20_exit_0)
{
  size_t before=buginsight_internal_allocated_bytes("malloc");
  void* p=malloc(10);
  ASSERT_TRUE(p);
  ASSERT_EQ(10, buginsight_internal_allocated_bytes("malloc"));


  void* p2=realloc(p, 20);
  ASSERT_TRUE(p2);
#ifdef _DEBUG
  ASSERT_EQ(_msize(p2), 20);
#endif

  ASSERT_EQ(0, buginsight_internal_allocated_bytes("malloc"));
  ASSERT_EQ(20, buginsight_internal_allocated_bytes("realloc"));

  free(p2);
}

UTEST_F(malloc_tests, test_api_realloc_dbg_resize_10_to_20_exit_0)
{
  void* p=_malloc_dbg(10, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p);

#ifdef _DEBUG
  ASSERT_EQ(10, buginsight_internal_allocated_bytes("_malloc_dbg"));
#else
  ASSERT_EQ(10, buginsight_internal_allocated_bytes("malloc"));
#endif

  void* p2=_realloc_dbg(p, 20, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p2);
#ifdef _DEBUG
  ASSERT_EQ(_msize(p2), 20);
#endif

#ifdef _DEBUG
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_malloc_dbg"));
  ASSERT_EQ(20, buginsight_internal_allocated_bytes("_realloc_dbg"));
#else
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("malloc"));
  ASSERT_EQ(20, buginsight_internal_allocated_bytes("realloc"));
#endif

  _free_dbg(p2, _NORMAL_BLOCK);
}

UTEST_F(malloc_tests, test_api_realloc_resize_10_to_5_exit_0)
{
  void* p=malloc(10);
  ASSERT_TRUE(p);
  ASSERT_EQ(10, buginsight_internal_allocated_bytes("malloc"));

  void* p2=realloc(p, 5);
  ASSERT_TRUE(p2);
#ifdef _DEBUG
  ASSERT_EQ(_msize(p2), 5);
#endif

  ASSERT_EQ(0, buginsight_internal_allocated_bytes("malloc"));
  ASSERT_EQ(5, buginsight_internal_allocated_bytes("realloc"));

  free(p2);
}

UTEST_F(malloc_tests, test_api_realloc_dbg_resize_10_to_5_exit_0)
{
  void* p=_malloc_dbg(10, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p);

#ifdef _DEBUG
  ASSERT_EQ(10, buginsight_internal_allocated_bytes("_malloc_dbg"));
#else
  ASSERT_EQ(10, buginsight_internal_allocated_bytes("malloc"));
#endif

  void* p2=_realloc_dbg(p, 5, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p2);
#ifdef _DEBUG
  ASSERT_EQ(_msize(p2), 5);
#endif

#ifdef _DEBUG
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_malloc_dbg"));
  ASSERT_EQ(5, buginsight_internal_allocated_bytes("_realloc_dbg"));
#else
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("malloc"));
  ASSERT_EQ(5, buginsight_internal_allocated_bytes("realloc"));
#endif

  _free_dbg(p2, _NORMAL_BLOCK);
}

UTEST_F(malloc_tests, test_api_realloc_size_zero_exit_0)
{
  void* p=malloc(10000);
  ASSERT_TRUE(p);
  ASSERT_EQ(10000, buginsight_internal_allocated_bytes("malloc"));

  void* p2=realloc(p, 0);
  ASSERT_TRUE(p2 == nullptr);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("malloc"));
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("realloc"));
}

UTEST_F(malloc_tests, test_api_realloc_block_and_size_zero_exit_0)
{
  auto before=buginsight_internal_allocated_bytes("realloc");
  void* p2=realloc(0, 0);
  ASSERT_TRUE(p2);

#ifdef _DEBUG
  ASSERT_EQ(_msize(p2), 0);
#else
  ASSERT_EQ(_msize(p2), 1);
#endif

  free(p2);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("realloc"));
}

UTEST_F(malloc_tests, test_api_realloc_dbg_block_and_size_zero_exit_0)
{
  void* p2=_realloc_dbg(0, 0, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p2);
#ifdef _DEBUG
  ASSERT_EQ(_msize(p2), 0);
#else
  ASSERT_EQ(_msize(p2), 1);
#endif

  free(p2);
#ifdef _DEBUG
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_realloc_dbg"));
#else
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("realloc"));
#endif
}

UTEST_F(malloc_tests, test_api_recalloc_block_and_size_zero_exit_0)
{
  void* p2=_recalloc(0, 0, 0);
  ASSERT_TRUE(p2);
#ifdef _DEBUG
  ASSERT_EQ(_msize(p2), 0);
#else
  ASSERT_EQ(_msize(p2), 1);
#endif

  free(p2);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_recalloc"));
}

UTEST_F(malloc_tests, test_api_recalloc_dbg_block_and_size_zero_exit_0)
{
  void* p2=_recalloc_dbg(0, 0, 0, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p2);
#ifdef _DEBUG
  ASSERT_EQ(_msize(p2), 0);
#else
  ASSERT_EQ(_msize(p2), 1);
#endif

  free(p2);
#ifdef _DEBUG
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_recalloc_dbg"));
#else
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("recalloc"));
#endif
}

UTEST_F(malloc_tests, test_api_realloc_dbg_size_zero_exit_0)
{
  void* p=_malloc_dbg(10000, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p);

#ifdef _DEBUG
  ASSERT_EQ(10000, buginsight_internal_allocated_bytes("_malloc_dbg"));
#else
  ASSERT_EQ(10000, buginsight_internal_allocated_bytes("malloc"));
#endif

  void* p2=_realloc_dbg(p, 0, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p2 == nullptr);

#ifdef _DEBUG
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_realloc_dbg"));
#else
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("realloc"));
#endif
}

UTEST_F(malloc_tests, test_api_calloc_resize_10_to_20_exit_0)
{
  void* p=calloc(10, 1);
  ASSERT_TRUE(p);
  ASSERT_EQ(10, buginsight_internal_allocated_bytes("calloc"));

  void* p2=_recalloc(p, 20, 1);
  ASSERT_TRUE(p2);
  ASSERT_EQ(_msize(p2), 20);

  ASSERT_EQ(0, buginsight_internal_allocated_bytes("calloc"));
  ASSERT_EQ(20, buginsight_internal_allocated_bytes("_recalloc"));

  free(p2);
}

UTEST_F(malloc_tests, test_api__recalloc_recalloc_10_to_20_exit_0)
{
  void* p=malloc(10);
  ASSERT_TRUE(p);
  ASSERT_EQ(10, buginsight_internal_allocated_bytes("malloc"));

  void* p2=_recalloc(p, 20, 1);
  ASSERT_TRUE(p2);
  ASSERT_EQ(_msize(p2), 20);

  ASSERT_EQ(0, buginsight_internal_allocated_bytes("calloc"));
  ASSERT_EQ(20, buginsight_internal_allocated_bytes("_recalloc"));

  free(p2);
}

UTEST_F(malloc_tests, test_api__recalloc_recalloc_10_to_5_exit_0)
{
  void* p=malloc(10);
  ASSERT_TRUE(p);
  ASSERT_EQ(10, buginsight_internal_allocated_bytes("malloc"));

  void* p2=_recalloc(p, 5, 1);
  ASSERT_TRUE(p2);
  ASSERT_EQ(_msize(p2), 5);

  ASSERT_EQ(0, buginsight_internal_allocated_bytes("calloc"));
  ASSERT_EQ(5, buginsight_internal_allocated_bytes("_recalloc"));

  free(p2);
}

UTEST_F(malloc_tests, test_api_recalloc_dbg_resize_10_to_20_exit_0)
{
  void* p=_calloc_dbg(10, 1, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p);
#ifdef _DEBUG
  ASSERT_EQ(10, buginsight_internal_allocated_bytes("_calloc_dbg"));
#else
  size_t s=buginsight_internal_allocated_bytes("calloc");
  ASSERT_EQ(10, s);
#endif

  void* p2=_recalloc_dbg(p, 20, 1, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p2);
  ASSERT_EQ(_msize(p2), 20);

#ifdef _DEBUG
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_calloc_dbg"));
  ASSERT_EQ(20, buginsight_internal_allocated_bytes("_recalloc_dbg"));
#else
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("calloc"));
  ASSERT_EQ(20, buginsight_internal_allocated_bytes("_recalloc"));
#endif

  _free_dbg(p2, _NORMAL_BLOCK);
}

UTEST_F(malloc_tests, test_api_recalloc_dbg_resize_10_to_5_exit_0)
{
  void* p=_calloc_dbg(10, 1, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p);

#ifdef _DEBUG
  ASSERT_EQ(10, buginsight_internal_allocated_bytes("_calloc_dbg"));
#else
  ASSERT_EQ(10, buginsight_internal_allocated_bytes("calloc"));
#endif

  void* p2=_recalloc_dbg(p, 5, 1, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p2);
  ASSERT_EQ(_msize(p2), 5);

#ifdef _DEBUG
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_calloc_dbg"));
  ASSERT_EQ(5, buginsight_internal_allocated_bytes("_recalloc_dbg"));
#else
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("calloc"));
  ASSERT_EQ(5, buginsight_internal_allocated_bytes("_recalloc"));
#endif


  _free_dbg(p2, _NORMAL_BLOCK);
}

UTEST_F(malloc_tests, _expand_exit_0)
{
  {
    void* p=malloc(1024);
    ASSERT_EQ(1024, buginsight_internal_allocated_bytes("malloc"));

    void* p_expaned=_expand(p, 1030);
    ASSERT_TRUE(p == p_expaned);
    ASSERT_EQ(_msize(p), 1030);

    ASSERT_EQ(0, buginsight_internal_allocated_bytes("malloc"));
    ASSERT_EQ(1030, buginsight_internal_allocated_bytes("_expand"));

    free(p_expaned);
  }

  {
    void* p=_malloc_dbg(1024, _NORMAL_BLOCK, __FILE__, __LINE__);

#ifdef _DEBUG
    ASSERT_EQ(1024, buginsight_internal_allocated_bytes("_malloc_dbg"));
#else
    ASSERT_EQ(1024, buginsight_internal_allocated_bytes("malloc"));
#endif

    void* p_expaned=_expand_dbg(p, 1030, _NORMAL_BLOCK, __FILE__, __LINE__);
    ASSERT_TRUE(p == p_expaned);
    ASSERT_EQ(_msize(p), 1030);

#ifdef _DEBUG
    ASSERT_EQ(0, buginsight_internal_allocated_bytes("_malloc_dbg"));
    ASSERT_EQ(1030, buginsight_internal_allocated_bytes("_expand_dbg"));
#else
    ASSERT_EQ(0, buginsight_internal_allocated_bytes("malloc"));
    ASSERT_EQ(1030, buginsight_internal_allocated_bytes("_expand"));
#endif

    free(p_expaned);
  }
}

UTEST_F(malloc_tests, test_api_malloc_free_zero_exit_0)
{
  free(0);
  _free_dbg(0, _NORMAL_BLOCK);
}

typedef LPVOID(__stdcall* malloc_fptr)(SIZE_T dwBytes);
typedef BOOL(__stdcall* free_ftpr) (LPVOID lpMem);

UTEST_F(malloc_tests, malloc_GetProcAddress_exit_0)
{
  UTEST_SKIP("GetProcAddress not yet supported");

#ifdef _DEBUG
  const HMODULE urtcbase_handle=GetModuleHandleA("ucrtbased.dll");
#else
  const HMODULE urtcbase_handle=GetModuleHandleA("ucrtbase.dll");
#endif

  malloc_fptr malloc_p=(malloc_fptr)GetProcAddress(urtcbase_handle, "malloc");

  void* p=malloc_p(42);
  ASSERT_TRUE(p);

  size_t allocated_size=buginsight_internal_allocated_bytes("malloc");
  ASSERT_EQ(42, buginsight_internal_allocated_bytes("malloc"));
  ASSERT_EQ(42, _msize(p));

  free_ftpr free_p=(free_ftpr)GetProcAddress(urtcbase_handle, "free");
  free_p(p);

  ASSERT_EQ(0, buginsight_internal_allocated_bytes("malloc"));
}

UTEST_F(malloc_tests, multithreaded_exit_0)
{
  //FXME size_t runs=100000;
  size_t runs=1;
  size_t thread_alloc=0;

  //std::thread malloc_dbg_thread([&]()
  {

#if 1
    for(size_t i=0; i < runs; ++i)
    {
      void* p=_malloc_dbg(i, _NORMAL_BLOCK, __FILE__, __LINE__);

      if((i % 1000) == 0)
      {
#ifdef _DEBUG
        size_t allocted=buginsight_internal_allocated_bytes("_malloc_dbg");
#else
        size_t allocted=buginsight_internal_allocated_bytes("malloc");
#endif
        ASSERT_EQ(i, allocted - thread_alloc);
      }

      _free_dbg(p, _NORMAL_BLOCK);

      if((i % 1000) == 0)
      {
#ifdef _DEBUG
        ASSERT_EQ(thread_alloc, buginsight_internal_allocated_bytes("_malloc_dbg"));
#else
        ASSERT_EQ(thread_alloc, buginsight_internal_allocated_bytes("malloc"));
#endif
      }
    }
#endif
    int brk=1;
  } //);


#if 1

  std::thread realloc_thread([&]()
    {
      for(size_t i=0; i < runs; ++i)
      {
        void* p=realloc(nullptr, i);
        if((i % 1000) == 0)
        {
          size_t allocted=buginsight_internal_allocated_bytes("realloc");
          ASSERT_EQ(i, allocted - thread_alloc);
        }
        free(p);

        if((i % 1000) == 0)
        {
          ASSERT_EQ(thread_alloc, buginsight_internal_allocated_bytes("realloc"));
        }
      }
      int brk=1;
    });

  std::thread realloc_dgb_thread([&]()
    {
      for(size_t i=0; i < runs; ++i)
      {
        void* p=_realloc_dbg(nullptr, i, _NORMAL_BLOCK, __FILE__, __LINE__);
        if((i % 1000) == 0)
        {
#ifdef _DEBUG
          size_t allocted=buginsight_internal_allocated_bytes("_realloc_dbg");
#else
          size_t allocted=buginsight_internal_allocated_bytes("realloc");
#endif
          ASSERT_EQ(i, allocted - thread_alloc);
        }
        _free_dbg(p, _NORMAL_BLOCK);
        if((i % 1000) == 0)
        {
#ifdef _DEBUG
          ASSERT_EQ(thread_alloc, buginsight_internal_allocated_bytes("_realloc_dbg"));
#else
          ASSERT_EQ(thread_alloc, buginsight_internal_allocated_bytes("realloc"));
#endif
        }
      }
      int brk=1;
    });

  std::thread calloc_thread([&]()
    {
      for(size_t i=0; i < runs; ++i)
      {
        void* p=calloc(i, sizeof(uint8_t));
        if((i % 1000) == 0)
        {
          size_t allocted=buginsight_internal_allocated_bytes("calloc");
          ASSERT_EQ(i, allocted - thread_alloc);
        }
        free(p);
        if((i % 1000) == 0)
        {
          ASSERT_EQ(thread_alloc, buginsight_internal_allocated_bytes("calloc"));
        }
      }
      int brk=1;
    });

  std::thread calloc_dbg_thread([&]()
    {
      for(size_t i=0; i < runs; ++i)
      {
        void* p=_calloc_dbg(i, sizeof(uint8_t), _NORMAL_BLOCK, __FILE__, __LINE__);
        if((i % 1000) == 0)
        {
#ifdef _DEBUG
          size_t allocted=buginsight_internal_allocated_bytes("_calloc_dbg");
#else
          size_t allocted=buginsight_internal_allocated_bytes("calloc");
#endif
          ASSERT_EQ(i, allocted - thread_alloc);
        }
        _free_dbg(p, _NORMAL_BLOCK);
        if((i % 1000) == 0)
        {
#ifdef _DEBUG
          ASSERT_EQ(thread_alloc, buginsight_internal_allocated_bytes("_calloc_dbg"));
#else
          ASSERT_EQ(thread_alloc, buginsight_internal_allocated_bytes("calloc"));
#endif
        }
      }
      int brk=1;
    });

  std::thread _recalloc_thread([&]()
    {
      for(size_t i=0; i < runs; ++i)
      {
        void* p=_recalloc(0, i, sizeof(uint8_t));

        if((i % 1000) == 0)
        {
          size_t allocted=buginsight_internal_allocated_bytes("_recalloc");
          ASSERT_EQ(i, allocted - thread_alloc);
        }

        free(p);
        if((i % 1000) == 0)
        {
          ASSERT_EQ(thread_alloc, buginsight_internal_allocated_bytes("_recalloc"));
        }
      }
      int brk=1;
    });

  std::thread _recalloc_dbg_thread([&]()
    {
      for(size_t i=0; i < runs; ++i)
      {
        void* p=_recalloc_dbg(0, i, sizeof(uint8_t), _NORMAL_BLOCK, __FILE__, __LINE__);

        if((i % 1000) == 0)
        {
#ifdef _DEBUG
          size_t allocted=buginsight_internal_allocated_bytes("_recalloc_dbg");
#else
          size_t allocted=buginsight_internal_allocated_bytes("_recalloc");
#endif
          ASSERT_EQ(i, allocted - thread_alloc);
        }
        _free_dbg(p, _NORMAL_BLOCK);

        if((i % 1000) == 0)
        {
#ifdef _DEBUG
          ASSERT_EQ(thread_alloc, buginsight_internal_allocated_bytes("_recalloc_dbg"));
#else
          ASSERT_EQ(thread_alloc, buginsight_internal_allocated_bytes("_recalloc"));
#endif
        }
      }
      int brk=1;
    });

  for(size_t i=0; i < runs; ++i)
  {
    void* p=malloc(i);
    free(p);
  }

#endif

  //malloc_dbg_thread.join();

#if 1
  realloc_thread.join();
  realloc_dgb_thread.join();
  calloc_thread.join();
  calloc_dbg_thread.join();
  _recalloc_thread.join();
  _recalloc_dbg_thread.join();
#endif

  int brk=1;
}



