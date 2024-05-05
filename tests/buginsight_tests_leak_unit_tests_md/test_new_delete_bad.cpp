// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <vector>

#include "utest.h"

extern size_t buginsight_internal_allocated_bytes(const char* p_function);


struct struct_for_new_t
{
  struct_for_new_t()
  {
    m_p=malloc(100);
  }

  ~struct_for_new_t()
  {
    free(m_p);
  }

  void* m_p{nullptr};
};


struct new_delete_bad_tests
{
};

UTEST_F_SETUP(new_delete_bad_tests)
{
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

UTEST_F_TEARDOWN(new_delete_bad_tests)
{
}

UTEST_F(new_delete_bad_tests, new_array_free_with_free_exit_66)
{
  struct_for_new_t* p=new struct_for_new_t[10];
  ASSERT_TRUE(p);

  free(p);
}

UTEST_F(new_delete_bad_tests, new_scalar_double_delete_exit_66)
{
  int* p=new int;
  ASSERT_TRUE(p);

  delete p;
  delete p;
}

UTEST_F(new_delete_bad_tests, new_array_double_delete_exit_66)
{
  int* p=new int[100];
  ASSERT_TRUE(p);

  delete[] p;
  delete[] p;
}

UTEST_F(new_delete_bad_tests, new_array_delete_scalar_exit_66)
{
  struct_for_new_t* p=new struct_for_new_t[10];
  ASSERT_TRUE(p);

  delete p; //will leak
}

UTEST_F(new_delete_bad_tests, ImallocI1I4I_leak_66)
{
  int* p=new int;
  ASSERT_TRUE(p);
}

UTEST_F(new_delete_bad_tests, ImallocI2I8I_leak_66)
{
  int* p=new int;
  ASSERT_TRUE(p);
  int* p2=new int;
  ASSERT_TRUE(p2);
}

UTEST_F(new_delete_bad_tests, ImallocI1I16I_leak_66)
{
  int* p=new int[4];
  ASSERT_TRUE(p);
}

#ifndef _DEBUG
//from  https://www.reddit.com/r/cpp/comments/18svlgj/my_favourite_memory_leak_bj%C3%B6rn_fahller_meeting_c/
UTEST_F(new_delete_bad_tests, ImallocI1I24I_leak_66)
{
  struct V : std::vector<V> {};
  V v;
  v.emplace_back();
  v.swap(v.front());

  size_t expected_size=24;
  size_t current_size=buginsight_internal_allocated_bytes("malloc");
  ASSERT_EQ(expected_size, current_size);
}

#endif

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

UTEST_F(new_delete_bad_tests, ImallocI1I200I_leak_66)
{
  Base* basePtr=new Derived();  // Creating a Derived object through a Base pointer
  delete basePtr;  // This will only call the Base destructor, not the Derived destructor

  size_t expected_size=200;
  size_t current_size=buginsight_internal_allocated_bytes("malloc");
  ASSERT_EQ(expected_size, current_size);
}




