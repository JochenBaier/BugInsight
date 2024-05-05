

#include "utest.h"

#include <cstdlib>
#include <vector>

extern size_t buginsight_internal_allocated_bytes(const char* p_function);


struct strdup_bad_tests
{
};

UTEST_F_SETUP(strdup_bad_tests)
{
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

UTEST_F_TEARDOWN(strdup_bad_tests)
{
}

UTEST_F(strdup_bad_tests, IstrdupI1I24I_leak_66)
{
  char buffer[]="This is the buffer text";
  char* newstring=strdup(buffer);
}

UTEST_F(strdup_bad_tests, IstrdupI2I48I_leak_66)
{
  char buffer[]="This is the buffer text";
  char* newstring=strdup(buffer);
  char* newstring2=strdup(buffer);
}

UTEST_F(strdup_bad_tests, IstrdupI1I1I_leak_66)
{
  char buffer[]="";
  char* newstring=strdup(buffer);
  ASSERT_TRUE(newstring);
}

UTEST_F(strdup_bad_tests, I_strdupI1I24I_leak_66)
{
  char buffer[]="This is the buffer text";
  char* newstring=_strdup(buffer);
  ASSERT_TRUE(newstring);
}

UTEST_F(strdup_bad_tests, I_strdupI1I1I_leak_66)
{
  char buffer[]="";
  char* newstring=_strdup(buffer);
  ASSERT_TRUE(newstring);
}

UTEST_F(strdup_bad_tests, IwcsdupI1I48I_leak_66)
{
  wchar_t buffer[]=L"This is the buffer text";
  wchar_t* newstring=wcsdup(buffer);
  ASSERT_TRUE(newstring);
}

UTEST_F(strdup_bad_tests, IwcsdupI1I2I_leak_66)
{
  wchar_t buffer[]=L"";
  wchar_t* newstring=wcsdup(buffer);
  ASSERT_TRUE(newstring);
}

UTEST_F(strdup_bad_tests, I_wcsdupI1I48I_leak_66)
{
  wchar_t buffer[]=L"This is the buffer text";
  wchar_t* newstring=_wcsdup(buffer);
  ASSERT_TRUE(newstring);
}

UTEST_F(strdup_bad_tests, I_wcsdupI1I2I_leak_66)
{
  wchar_t buffer[]=L"";
  wchar_t* newstring=_wcsdup(buffer);
  ASSERT_TRUE(newstring);
}

