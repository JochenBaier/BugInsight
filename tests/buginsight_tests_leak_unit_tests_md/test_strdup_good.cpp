// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "utest.h"

#include <cstdlib>
#include <vector>

extern size_t buginsight_internal_allocated_bytes(const char* p_function);


struct strdup_tests
{
};

UTEST_F_SETUP(strdup_tests)
{
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

UTEST_F_TEARDOWN(strdup_tests)
{
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

UTEST_F(strdup_tests, test_api_strdup_basic_exit_0)
{
  char buffer[]="This is the buffer text";
  char* newstring=strdup(buffer);
  ASSERT_EQ(strlen(buffer) + 1, buginsight_internal_allocated_bytes("strdup"));


  ASSERT_STREQ(buffer, newstring);
  ASSERT_EQ(strlen(buffer), strlen(newstring));

  free(newstring);
}

UTEST_F(strdup_tests, test_api_strdup_empty_exit_0)
{
  char buffer[]="";
  char* newstring=strdup(buffer);
  ASSERT_EQ(strlen(buffer) + 1, buginsight_internal_allocated_bytes("strdup"));

  ASSERT_STREQ(buffer, newstring);
  ASSERT_EQ(strlen(buffer), strlen(newstring));

  free(newstring);
}

UTEST_F(strdup_tests, test_api_underscore_strdup_basic_exit_0)
{
  char buffer[]="This is the buffer text";
  char* newstring=_strdup(buffer);
  ASSERT_EQ(strlen(buffer) + 1, buginsight_internal_allocated_bytes("_strdup"));

  ASSERT_STREQ(buffer, newstring);
  ASSERT_EQ(strlen(buffer), strlen(newstring));

  free(newstring);
}

UTEST_F(strdup_tests, test_api_underscore_strdup_empty_exit_0)
{
  char buffer[]="";
  char* newstring=_strdup(buffer);
  ASSERT_EQ(strlen(buffer) + 1, buginsight_internal_allocated_bytes("_strdup"));

  ASSERT_STREQ(buffer, newstring);
  ASSERT_EQ(strlen(buffer), strlen(newstring));

  free(newstring);
}

UTEST_F(strdup_tests, test_api_wcsdup_basic_exit_0)
{
  wchar_t buffer[]=L"This is the buffer text";
  wchar_t* newstring=wcsdup(buffer);
  ASSERT_EQ((wcslen(buffer) + 1) * sizeof(wchar_t), buginsight_internal_allocated_bytes("wcsdup"));

  ASSERT_EQ(wcscmp(buffer, newstring), 0);
  ASSERT_EQ(wcslen(buffer), wcslen(newstring));

  free(newstring);
}

UTEST_F(strdup_tests, test_api_wcsdup_empty_exit_0)
{
  wchar_t buffer[]=L"";
  wchar_t* newstring=wcsdup(buffer);
  ASSERT_EQ((wcslen(buffer) + 1) * sizeof(wchar_t), buginsight_internal_allocated_bytes("wcsdup"));

  ASSERT_EQ(wcscmp(buffer, newstring), 0);
  ASSERT_EQ(wcslen(buffer), wcslen(newstring));

  free(newstring);
}

UTEST_F(strdup_tests, test_api_underscore_wcsdup_basic_exit_0)
{
  wchar_t buffer[]=L"This is the buffer text";
  wchar_t* newstring=_wcsdup(buffer);
  ASSERT_EQ((wcslen(buffer) + 1) * sizeof(wchar_t), buginsight_internal_allocated_bytes("_wcsdup"));

  ASSERT_EQ(wcscmp(buffer, newstring), 0);
  ASSERT_EQ(wcslen(buffer), wcslen(newstring));

  free(newstring);
}

UTEST_F(strdup_tests, test_api_underscore_wcsdup_empty_exit_0)
{
  wchar_t buffer[]=L"";
  wchar_t* newstring=_wcsdup(buffer);
  ASSERT_EQ((wcslen(buffer) + 1) * sizeof(wchar_t), buginsight_internal_allocated_bytes("_wcsdup"));

  ASSERT_EQ(wcscmp(buffer, newstring), 0);
  ASSERT_EQ(wcslen(buffer), wcslen(newstring));

  free(newstring);
}

UTEST_F(strdup_tests, test_api_strdup_zero_size_exit_0)
{
  char* newstring=strdup(0);
  ASSERT_TRUE(newstring == nullptr);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("strdup"));
}

UTEST_F(strdup_tests, test_api_wcsdup_zero_size_exit_0)
{
  wchar_t* newstring=wcsdup(0);
  ASSERT_TRUE(newstring == nullptr);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("wcsdup"));
}

