// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <cassert>

#include <cstdlib>
#include <vector>
#include <array>
#include <allocator.hpp>

#include <utest.h>
#include <direct.h>
#include "../shared/test_helper.hpp"

extern size_t buginsight_internal_allocated_bytes(const char* p_function);

struct winapi_bad_tests
{
};

UTEST_F_SETUP(winapi_bad_tests)
{
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

UTEST_F_TEARDOWN(winapi_bad_tests)
{
}

UTEST_F(winapi_bad_tests, I_getcwdI1I24I_leak_66)
{
  char* p=_getcwd(nullptr, 2048);
  ASSERT_TRUE(p);
  test_helper::no_optimize(p);
}

UTEST_F(winapi_bad_tests, I_getcwd_dbgI1I24I_leak_66)
{
  char* p=_getcwd_dbg(nullptr, 2048, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p);
  test_helper::no_optimize(p);
}

UTEST_F(winapi_bad_tests, I_wgetcwdI1I48I_leak_66)
{
  wchar_t* p=_wgetcwd(nullptr, 2048);
  ASSERT_TRUE(p);
  test_helper::no_optimize(p);
}

UTEST_F(winapi_bad_tests, I_wgetcwd_dbgI1I48I_leak_66)
{
  wchar_t* p=_wgetcwd_dbg(nullptr, 2048, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p);
  test_helper::no_optimize(p);
}

UTEST_F(winapi_bad_tests, I_getdcwdI1I24I_leak_66)
{
  char* p=_getdcwd(3, nullptr, 2048);
  ASSERT_TRUE(p);
  test_helper::no_optimize(p);
}

UTEST_F(winapi_bad_tests, I_getdcwd_dbgI1I24I_leak_66)
{
  char* p=_getdcwd_dbg(3, nullptr, 2048, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p);
  test_helper::no_optimize(p);
}

UTEST_F(winapi_bad_tests, I_wgetdcwdI1I48I_leak_66)
{
  wchar_t* p=_wgetdcwd(3, nullptr, 2048);
  ASSERT_TRUE(p);
  test_helper::no_optimize(p);
}

UTEST_F(winapi_bad_tests, I_wgetdcwd_dbgI1I48I_leak_66)
{
  wchar_t* p=_wgetdcwd_dbg(3, nullptr, 2048, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p);
  test_helper::no_optimize(p);
}

UTEST_F(winapi_bad_tests, I_tempnamI1I45I_leak_66)
{
  char* p=_tempnam(nullptr, "file_prefix");
  ASSERT_TRUE(p);
  test_helper::no_optimize(p);
}

UTEST_F(winapi_bad_tests, I_tempnam_dbgI1I45I_leak_66)
{
  char* p=_tempnam_dbg(nullptr, "file_prefix", _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p);
  test_helper::no_optimize(p);
}

UTEST_F(winapi_bad_tests, I_wtempnamI1I90I_leak_66)
{
  wchar_t* p=_wtempnam(nullptr, L"file_prefix");
  ASSERT_TRUE(p);
  test_helper::no_optimize(p);
}

UTEST_F(winapi_bad_tests, I_wtempnam_dbgI1I90I_leak_66)
{
  wchar_t* p=_wtempnam_dbg(nullptr, L"file_prefix", _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(p);
  test_helper::no_optimize(p);
}

UTEST_F(winapi_bad_tests, I_dupenv_sI1I11I_leak_66)
{
  char* pValue=nullptr;
  size_t len;
  errno_t err=_dupenv_s(&pValue, &len, "OS");
  ASSERT_EQ(err, 0);
  ASSERT_TRUE(pValue);
}

UTEST_F(winapi_bad_tests, I_dupenv_s_dbgI1I11I_leak_66)
{
  char* pValue=nullptr;
  size_t len;
  errno_t err=_dupenv_s_dbg(&pValue, &len, "OS", _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_EQ(err, 0);
  ASSERT_TRUE(pValue);
}

UTEST_F(winapi_bad_tests, I_wdupenv_sI1I22I_leak_66)
{
  wchar_t* pValue=nullptr;
  size_t len;
  errno_t err=_wdupenv_s(&pValue, &len, L"OS");
  ASSERT_EQ(err, 0);
  ASSERT_TRUE(pValue);
}

UTEST_F(winapi_bad_tests, I_wdupenv_s_dbgI1I22I_leak_66)
{
  wchar_t* pValue;
  size_t len;
  errno_t err=_wdupenv_s_dbg(&pValue, &len, L"OS", _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_EQ(err, 0);
  ASSERT_TRUE(pValue);
}

UTEST_F(winapi_bad_tests, I_fullpathI1I29I_leak_66)
{
  char working_dir[1024]{0};
  char* full=_fullpath(nullptr, "test", 0);
  ASSERT_TRUE(full);
}

UTEST_F(winapi_bad_tests, I_fullpath_dbgI1I29I_leak_66)
{
  char working_dir[1024]{0};
  char* full=_fullpath_dbg(nullptr, "test", 0, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(full);
}

UTEST_F(winapi_bad_tests, I_wfullpathI1I58I_leak_66)
{
  wchar_t working_dir[1024]{0};
  wchar_t* full=_wfullpath(nullptr, L"test", 0);
  ASSERT_TRUE(full);
}

UTEST_F(winapi_bad_tests, I_wfullpath_dbgI1I58I_leak_66)
{
  wchar_t working_dir[1024]{0};
  wchar_t* full=_wfullpath_dbg(nullptr, L"test", 0, _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_TRUE(full);
}


