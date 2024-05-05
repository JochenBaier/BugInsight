// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "utest.h"

#include <cstdlib>
#include <vector>
#include <string>

extern size_t buginsight_internal_allocated_bytes(const char* p_function);

struct new_delete_tests
{
};

UTEST_F_SETUP(new_delete_tests)
{
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

UTEST_F_TEARDOWN(new_delete_tests)
{
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

struct struct_for_new
{
  struct_for_new()
  {
    m_mem=new int;
    m_mem_array=new int[10];
  }

  ~struct_for_new()
  {
    delete m_mem;
    delete[] m_mem_array;
  }

  uint64_t m_value{123456789};

  int* m_mem{nullptr};
  int* m_mem_array{nullptr};
};

UTEST_F(new_delete_tests, test_new_scalar_exit_0)
{
  int* pint=new int;
  ASSERT_EQ(sizeof(int), buginsight_internal_allocated_bytes("malloc"));

  uint64_t* puint64=new uint64_t;
  ASSERT_EQ(sizeof(int) + sizeof(uint64_t), buginsight_internal_allocated_bytes("malloc"));

  delete pint;
  ASSERT_EQ(sizeof(uint64_t), buginsight_internal_allocated_bytes("malloc"));

  delete puint64;
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("malloc"));
}

UTEST_F(new_delete_tests, test_new_array_exit_0)
{
  int* pint=new int[10];
  ASSERT_EQ(sizeof(int) * 10, buginsight_internal_allocated_bytes("malloc"));

  uint64_t* puint64=new uint64_t[20];
  ASSERT_EQ(sizeof(int) * 10 + sizeof(uint64_t) * 20, buginsight_internal_allocated_bytes("malloc"));

  delete[] pint;
  ASSERT_EQ(sizeof(uint64_t) * 20, buginsight_internal_allocated_bytes("malloc"));

  delete[] puint64;
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("malloc"));
}

UTEST_F(new_delete_tests, test_new_struct_exit_0)
{
  struct_for_new* p_struct=new struct_for_new;
  ASSERT_EQ(sizeof(struct_for_new) + sizeof(int) + sizeof(int) * 10, buginsight_internal_allocated_bytes("malloc"));

  delete p_struct;
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("malloc"));
}

UTEST_F(new_delete_tests, test_new_array_struct_exit_0)
{
  size_t current_size_before=buginsight_internal_allocated_bytes("malloc");

  size_t s=sizeof(struct_for_new);

  struct_for_new* p_struct=new struct_for_new[10];

  size_t expected_size=sizeof(size_t) + 10 * (sizeof(struct_for_new) + sizeof(int) + (sizeof(int) * 10));
  size_t current_size=buginsight_internal_allocated_bytes("malloc");
  ASSERT_EQ(expected_size, current_size);

  delete[] p_struct;

  ASSERT_EQ(0, buginsight_internal_allocated_bytes("malloc"));
}

UTEST_F(new_delete_tests, test_string_array_exit_0)
{
  std::string* pStringArray=new std::string[3];
  pStringArray[0]="one";
  pStringArray[1]="two";
  pStringArray[2]="three";

#ifdef _DEBUG
  size_t expected_size=176;
#else
  size_t expected_size=104;
#endif
  size_t current_size=buginsight_internal_allocated_bytes("malloc");
  ASSERT_EQ(expected_size, current_size);

  delete[] pStringArray;

  ASSERT_EQ(0, buginsight_internal_allocated_bytes("malloc"));
}

UTEST_F(new_delete_tests, test_operator_new_exit_0)
{
  void* operator_new=operator new(500);

  size_t expected_size=500;
  size_t current_size=buginsight_internal_allocated_bytes("malloc");
  ASSERT_EQ(expected_size, current_size);

  delete operator_new;
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("malloc"));
}

UTEST_F(new_delete_tests, test_operator_new_array_exit_0)
{
  void* operator_new=operator new[](500);

  size_t expected_size=500;
  size_t current_size=buginsight_internal_allocated_bytes("malloc");
  ASSERT_EQ(expected_size, current_size);

  delete[] operator_new;
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("malloc"));
}

UTEST_F(new_delete_tests, test_operator_new_dbg_exit_0)
{
  int* new_dbg_client_blocknew=new (_NORMAL_BLOCK, __FILE__, __LINE__) int;

  size_t expected_size=sizeof(int);

#ifdef _DEBUG
  size_t current_size=buginsight_internal_allocated_bytes("_malloc_dbg");
#else
  size_t current_size=buginsight_internal_allocated_bytes("malloc");
#endif

  ASSERT_EQ(expected_size, current_size);

  delete new_dbg_client_blocknew;

#ifdef _DEBUG
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_malloc_dbg"));
#else
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("malloc"));
#endif
}

UTEST_F(new_delete_tests, test_operator_new_array_dbg_exit_0)
{
  int* new_dbg_client_blocknew=new (_NORMAL_BLOCK, __FILE__, __LINE__) int[100];

  size_t expected_size=100 * sizeof(int);

#ifdef _DEBUG
  size_t current_size=buginsight_internal_allocated_bytes("_malloc_dbg");
#else
  size_t current_size=buginsight_internal_allocated_bytes("malloc");
#endif

  ASSERT_EQ(expected_size, current_size);

  delete[] new_dbg_client_blocknew;

#ifdef _DEBUG
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("_malloc_dbg"));
#else
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("malloc"));
#endif
}

UTEST_F(new_delete_tests, test_std_vector_int_exit_0)
{
  {
    std::vector<int> int_array;
    int_array.resize(10);

#ifdef _DEBUG
    size_t expected_size=56; 
#else
    size_t expected_size=40;
#endif
    size_t current_size=buginsight_internal_allocated_bytes("malloc");
    printf("expected_size: %zu\n", expected_size);
    printf("current_size: %zu\n", current_size);

    ASSERT_EQ(expected_size, current_size);
  }

  ASSERT_EQ(0, buginsight_internal_allocated_bytes("malloc"));
}

UTEST_F(new_delete_tests, test_std_vector_struct_exit_0)
{
  {
    std::vector<struct_for_new> int_array;
    int_array.resize(10);

#ifdef _DEBUG
    size_t expected_size=696;
#else
    size_t expected_size=680; 
#endif
    size_t current_size=buginsight_internal_allocated_bytes("malloc");
    printf("current_size: %zu\n", current_size);

    ASSERT_EQ(expected_size, current_size);
  }

  ASSERT_EQ(0, buginsight_internal_allocated_bytes("malloc"));
}

//from  https://www.reddit.com/r/cpp/comments/18svlgj/my_favourite_memory_leak_bj%C3%B6rn_fahller_meeting_c/
UTEST_F(new_delete_tests, test_std_vector_leak_exit_0)
{
  {
    struct V : std::vector<V> {};
    V v;
    v.emplace_back();
    v.swap(v.front());

#ifdef _DEBUG
    size_t expected_size=64;
#else
    size_t expected_size=24;
#endif
    size_t current_size=buginsight_internal_allocated_bytes("malloc");
    printf("current_size: %zu\n", current_size);
    ASSERT_EQ(expected_size, current_size);

  }

  GetEnvironmentVariableA("buginsight/set/mem/reset_all", nullptr, 0);
}

class Base {
public:
  Base()
  {
    m_no_leak=malloc(100);
  }

  // Non-virtual destructor
  ~Base()
  {
    free(m_no_leak);
  }

  void* m_no_leak;
};

class Derived : public Base {
public:
  Derived()
  {
    m_leak=malloc(200);
  }

  ~Derived()
  {
    free(m_leak);
  }
  void* m_leak;
};

UTEST_F(new_delete_tests, test_non_virtual_dtor_exit_0)
{
  Base* basePtr=new Derived();  // Creating a Derived object through a Base pointer
  delete basePtr;  // This will only call the Base destructor, not the Derived destructor

  size_t expected_size=200;
  size_t current_size=buginsight_internal_allocated_bytes("malloc");
  ASSERT_EQ(expected_size, current_size);
  GetEnvironmentVariableA("buginsight/set/mem/reset_all", nullptr, 0);
}

UTEST_F(new_delete_tests, new_scalar_array_delete_exit_0)
{
  int* p=new int;
  ASSERT_TRUE(p);

  delete[] p;
}









