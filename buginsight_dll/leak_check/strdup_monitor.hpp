//This program is free software:you can redistribute it and /or modify it under the terms of the GNU
//General Public License as published by the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
//even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//See the GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License along with this program.If not, see
//<https://www.gnu.org/licenses/>. 

#pragma once
#include "memory_leak_map_with_callstack.hpp"

extern memory_leak_repository_t* g_memory_leak_repository;
extern std::atomic_uint64_t g_monitored_api_calls;

using _strdup_t=char* (__cdecl*) (const char*);
using _strdup_dbg_t=char* (__cdecl*) (const char*, int, const char*, int);
using _wcsdup_t=wchar_t* (__cdecl*) (const wchar_t*);
using _wcsdup_dbg_t=wchar_t* (__cdecl*) (const wchar_t*, int, const char*, int);

template<bool debug>
class strdup_monitor_t
{
public:
  inline static _strdup_t _strdup_fptr{};
  static char* bi__strdup(const char* str)
  {
    ++g_monitored_api_calls;

    char* duplicated=_strdup_fptr(str);
    if(!duplicated)
    {
      return duplicated;
    }

    const size_t size=strlen(duplicated) + 1;

    g_memory_leak_repository->add((uintptr_t)duplicated, size, api_family_t::e_malloc, api_functions_t::e__strdup);
    return (char*)duplicated;
  }

  inline static _wcsdup_t _wcsdup_fptr{};
  static wchar_t* bi__wcsdup(const wchar_t* string)
  {
    ++g_monitored_api_calls;

    wchar_t* duplicated=_wcsdup_fptr(string);
    if(!duplicated)
    {
      return duplicated;
    }

    size_t const size_in_elements=wcslen(string) + 1;

    g_memory_leak_repository->add((uintptr_t)duplicated, size_in_elements * sizeof(wchar_t), api_family_t::e_malloc, api_functions_t::e__wcsdup);
    return duplicated;
  }

  inline static _strdup_dbg_t _strdup_dbg_fptr{};
  static char* bi__strdup_dbg(const char* strSource, int blockType, const char* filename, int linenumber)
  {
    ++g_monitored_api_calls;

    char* duplicated=_strdup_dbg_fptr(strSource, blockType, filename, linenumber);
    if(!duplicated)
    {
      return duplicated;
    }

    const size_t size=strlen(duplicated) + 1;

    g_memory_leak_repository->add((uintptr_t)duplicated, size, api_family_t::e_malloc, api_functions_t::e__strdup_dbg);
    return (char*)duplicated;
  }

  inline static _wcsdup_dbg_t _wcsdup_dbg_fptr{};
  static wchar_t* bi__wcsdup_dbg(const wchar_t* strSource, int blockType, const char* filename, int linenumber)
  {
    ++g_monitored_api_calls;

    wchar_t* duplicated=_wcsdup_dbg_fptr(strSource, blockType, filename, linenumber);
    if(!duplicated)
    {
      return duplicated;
    }

    size_t const size_in_elements=wcslen(duplicated) + 1;

    g_memory_leak_repository->add((uintptr_t)duplicated, size_in_elements * sizeof(wchar_t), api_family_t::e_malloc, api_functions_t::e__wcsdup_dbg);
    return duplicated;
  }
};
