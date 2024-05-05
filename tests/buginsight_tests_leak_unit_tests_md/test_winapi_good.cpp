// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include <cassert>

#include <cstdlib>
#include <vector>
#include <array>
#include <allocator.hpp>

#include <utest.h>
#include <direct.h>

extern size_t buginsight_internal_allocated_bytes(const char* p_function);

struct winapi_tests
{
};

UTEST_F_SETUP(winapi_tests)
{
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

UTEST_F_TEARDOWN(winapi_tests)
{
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

UTEST_F(winapi_tests, test__getcwd_exit_0)
{
  char* p=_getcwd(nullptr, 2048);

  size_t lenght=strlen(p);
  ASSERT_LT(5, lenght);

  size_t allocted=buginsight_internal_allocated_bytes("*");
  ASSERT_EQ(lenght + 1, allocted);

  free(p);
}

UTEST_F(winapi_tests, test__getcwd_dbg_exit_0)
{
  char* p=_getcwd_dbg(nullptr, 2048, _NORMAL_BLOCK, __FILE__, __LINE__);

  size_t lenght=strlen(p);
  ASSERT_LT(5, lenght);

  size_t allocted=buginsight_internal_allocated_bytes("*");
  ASSERT_EQ(lenght + 1, allocted);

  _free_dbg(p, _NORMAL_BLOCK);
}

UTEST_F(winapi_tests, test__wgetcwd_exit_0)
{
  wchar_t* p=_wgetcwd(nullptr, 2048);

  size_t lenght=wcslen(p);
  ASSERT_LT(5, lenght);
  ASSERT_EQ((lenght + 1) * sizeof(wchar_t), buginsight_internal_allocated_bytes("*"));

  free(p);
}

UTEST_F(winapi_tests, test__wgetcwd_dbg_exit_0)
{
  wchar_t* p=_wgetcwd_dbg(nullptr, 2048, _NORMAL_BLOCK, __FILE__, __LINE__);

  size_t lenght=wcslen(p);
  ASSERT_LT(5, lenght);
  ASSERT_EQ((lenght + 1) * sizeof(wchar_t), buginsight_internal_allocated_bytes("*"));

  _free_dbg(p, _NORMAL_BLOCK);
}

UTEST_F(winapi_tests, test__getdcwd_exit_0)
{
  char* p=_getdcwd(3, nullptr, 2048);

  size_t lenght=strlen(p);
  ASSERT_LT(5, lenght);
  ASSERT_EQ((lenght + 1) * sizeof(char), buginsight_internal_allocated_bytes("*"));

  free(p);
}

UTEST_F(winapi_tests, test__getdcwd_dbg_exit_0)
{
  char* p=_getdcwd_dbg(3, nullptr, 2048, _NORMAL_BLOCK, __FILE__, __LINE__);

  size_t lenght=strlen(p);
  ASSERT_LT(5, lenght);
  ASSERT_EQ((lenght + 1) * sizeof(char), buginsight_internal_allocated_bytes("*"));

  _free_dbg(p, _NORMAL_BLOCK);
}

UTEST_F(winapi_tests, test__wgetdcwd_exit_0)
{
  wchar_t* p=_wgetdcwd(3, nullptr, 2048);

  size_t lenght=wcslen(p);
  ASSERT_LT(5, lenght);
  ASSERT_EQ((lenght + 1) * sizeof(wchar_t), buginsight_internal_allocated_bytes("*"));

  free(p);
}

UTEST_F(winapi_tests, test__wgetdcwd_dbg_exit_0)
{
  wchar_t* p=_wgetdcwd_dbg(3, nullptr, 2048, _NORMAL_BLOCK, __FILE__, __LINE__);

  size_t lenght=wcslen(p);
  ASSERT_LT(5, lenght);
  ASSERT_EQ((lenght + 1) * sizeof(wchar_t), buginsight_internal_allocated_bytes("*"));

  _free_dbg(p, _NORMAL_BLOCK);
}

UTEST_F(winapi_tests, test__tempnam_exit_0)
{
  char* p=_tempnam(nullptr, "file_prefix");

  size_t lenght=strlen(p);
  ASSERT_LT(5, lenght);
  ASSERT_EQ((lenght + 1) * sizeof(char), buginsight_internal_allocated_bytes("*"));

  free(p);
}

UTEST_F(winapi_tests, test__tempnam_dbg_exit_0)
{
  char* p=_tempnam_dbg(nullptr, "file_prefix", _NORMAL_BLOCK, __FILE__, __LINE__);

  size_t lenght=strlen(p);
  ASSERT_LT(5, lenght);
  ASSERT_EQ((lenght + 1) * sizeof(char), buginsight_internal_allocated_bytes("*"));

  _free_dbg(p, _NORMAL_BLOCK);
}

UTEST_F(winapi_tests, test__wtempnam_exit_0)
{
  wchar_t* p=_wtempnam(nullptr, L"file_prefix");

  size_t lenght=wcslen(p);
  ASSERT_LT(5, lenght);
  ASSERT_EQ((lenght + 1) * sizeof(wchar_t), buginsight_internal_allocated_bytes("*"));

  free(p);
}

UTEST_F(winapi_tests, test__wtempnam_dbg_exit_0)
{
  wchar_t* p=_wtempnam_dbg(nullptr, L"file_prefix", _NORMAL_BLOCK, __FILE__, __LINE__);

  size_t lenght=wcslen(p);
  ASSERT_LT(5, lenght);
  ASSERT_EQ((lenght + 1) * sizeof(wchar_t), buginsight_internal_allocated_bytes("*"));

  _free_dbg(p, _NORMAL_BLOCK);
}

UTEST_F(winapi_tests, test__dupenv_s_exit_0)
{
  char* pValue;
  size_t len;
  errno_t err=_dupenv_s(&pValue, &len, "OS");
  ASSERT_EQ(err, 0);
  ASSERT_STREQ(pValue, "Windows_NT");
  ASSERT_EQ(len * sizeof(char), buginsight_internal_allocated_bytes("*"));

  free(pValue);

  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

UTEST_F(winapi_tests, test__dupenv_dbg_s_exit_0)
{
  char* pValue;
  size_t len;
  errno_t err=_dupenv_s_dbg(&pValue, &len, "OS", _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_EQ(err, 0);
  ASSERT_STREQ(pValue, "Windows_NT");
  ASSERT_EQ(len * sizeof(char), buginsight_internal_allocated_bytes("*"));

  free(pValue);

  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

UTEST_F(winapi_tests, test__wdupenv_s_exit_0)
{
  wchar_t* pValue;
  size_t len;
  errno_t err=_wdupenv_s(&pValue, &len, L"OS");
  ASSERT_EQ(err, 0);
  ASSERT_EQ(wcscmp(pValue, L"Windows_NT"), 0);
  ASSERT_EQ(len * sizeof(wchar_t), buginsight_internal_allocated_bytes("*"));

  free(pValue);

  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

UTEST_F(winapi_tests, test__wdupenv_s_dbg_exit_0)
{
  wchar_t* pValue;
  size_t len;
  errno_t err=_wdupenv_s_dbg(&pValue, &len, L"OS", _NORMAL_BLOCK, __FILE__, __LINE__);
  ASSERT_EQ(err, 0);
  ASSERT_EQ(wcscmp(pValue, L"Windows_NT"), 0);
  ASSERT_EQ(len * sizeof(wchar_t), buginsight_internal_allocated_bytes("*"));

  free(pValue);

  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

UTEST_F(winapi_tests, test__fullpath_exit_0)
{
  char working_dir[1024]{0};

  char* full=_fullpath(nullptr, "test", 0);

  char expected_buffer[2048]{0};

  DWORD res=GetCurrentDirectoryA(sizeof(working_dir), &working_dir[0]);
  sprintf_s(expected_buffer, "%s\\test", working_dir);
  ASSERT_STREQ(full, expected_buffer);

  ASSERT_EQ(strlen(expected_buffer) + 1, buginsight_internal_allocated_bytes("*"));

  free(full);

  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

UTEST_F(winapi_tests, test__fullpath_dbg_exit_0)
{
  char working_dir[1024]{0};

  char* full=_fullpath_dbg(nullptr, "test", 0, _NORMAL_BLOCK, __FILE__, __LINE__);

  char expected_buffer[2048]{0};

  DWORD res=GetCurrentDirectoryA(sizeof(working_dir), &working_dir[0]);
  sprintf_s(expected_buffer, "%s\\test", working_dir);
  ASSERT_STREQ(full, expected_buffer);

  ASSERT_EQ(strlen(expected_buffer) + 1, buginsight_internal_allocated_bytes("*"));

  free(full);

  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

UTEST_F(winapi_tests, test__fullpath_non_leak_exit_0)
{
  char working_dir[1024]{0};
  char buffer[1024]{0};

  char* full=_fullpath(buffer, "test", sizeof(working_dir));

  char expected_buffer[2048]{0};

  DWORD res=GetCurrentDirectoryA(sizeof(working_dir), &working_dir[0]);
  sprintf_s(expected_buffer, "%s\\test", working_dir);
  ASSERT_STREQ(full, expected_buffer);

  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

UTEST_F(winapi_tests, test__wfullpath_exit_0)
{
  wchar_t working_dir[1024]{0};

  wchar_t* full=_wfullpath(nullptr, L"test", 0);

  wchar_t expected_buffer[2048]{0};

  DWORD res=GetCurrentDirectoryW(sizeof(working_dir), &working_dir[0]);
  swprintf_s(expected_buffer, L"%s\\test", working_dir);
  ASSERT_EQ(wcscmp(expected_buffer, full), 0);

  free(full);

  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

UTEST_F(winapi_tests, test__wfullpath_dbg_exit_0)
{
  wchar_t working_dir[1024]{0};

  wchar_t* full=_wfullpath_dbg(nullptr, L"test", 0, _NORMAL_BLOCK, __FILE__, __LINE__);

  wchar_t expected_buffer[2048]{0};

  DWORD res=GetCurrentDirectoryW(sizeof(working_dir), &working_dir[0]);
  swprintf_s(expected_buffer, L"%s\\test", working_dir);
  ASSERT_EQ(wcscmp(expected_buffer, full), 0);

  free(full);

  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}



