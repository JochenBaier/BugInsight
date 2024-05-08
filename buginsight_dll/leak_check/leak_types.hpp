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

#include <array>
#include <cstdint>
#include <string>

struct callstack_t
{
  std::array<void*, 32> m_ptr{};
  size_t m_size{};
};

enum class api_functions_t : uint8_t
{
  e_unknown,
  e_HeapCreate,
  e_HeapDestroy,
  e_HeapAlloc,
  e_HeapReAlloc,
  e_HeapFree,

  e_malloc,
  e__malloc_dbg,
  e_realloc,
  e__realloc_dbg,
  e_calloc,
  e__calloc_dbg,
  e__recalloc,
  e__recalloc_dbg,
  e__strdup,
  e__strdup_dbg,
  e__wcsdup,
  e__wcsdup_dbg,
  e__expand,
  e__expand_dbg,
  e_free,
  e__free_dbg,

  e__getcwd,
  e__getcwd_dbg,
  e__wgetcwd,
  e__wgetcwd_dbg,
  e__getdcwd,
  e__getdcwd_dbg,
  e__wgetdcwd,
  e__wgetdcwd_dbg,
  e__tempnam,
  e__tempnam_dbg,
  e__wtempnam,
  e__wtempnam_dbg,
  e__dupenv_s,
  e__dupenv_s_dbg,
  e__wdupenv_s,
  e__wdupenv_s_dbg,
  e__fullpath,
  e__fullpath_dbg,
  e__wfullpath,
  e__wfullpath_dbg,

  e__aligned_malloc,
  e__aligned_malloc_dbg,
  e__aligned_offset_malloc,
  e__aligned_offset_malloc_dbg,
  e__aligned_offset_realloc,
  e__aligned_offset_realloc_dbg,
  e__aligned_offset_recalloc,
  e__aligned_offset_recalloc_dbg,
  e__aligned_realloc,
  e__aligned_realloc_dbg,
  e__aligned_recalloc,
  e__aligned_recalloc_dbg,
  e__aligned_free,
  e__aligned_free_dbg,

  e_localalloc,
  e_localrealloc,
  e_localfree,

  e_globalalloc,
  e_globrealalloc,
  e_globalfree,

  e_FormatMessageA,
  e_FormatMessageW,
  e_CommandLineToArgvW,

  e_ConvertSidToStringSidW,
  e_ConvertSidToStringSidA,

  e_ConvertStringSidToSidW,
  e_ConvertStringSidToSidA,

  e_CryptProtectData,
  e_CryptUnprotectData,

  e_GetNamedSecurityInfoW,
  e_GetNamedSecurityInfoA,

  e_SetEntriesInAclA,
  e_SetEntriesInAclW,
  e_AllocateAndInitializeSid,
  e_FreeSid,
  e_GetSecurityInfo,
  e_StrDupA,
  e_StrDupW,

  e_SIZE
};

std::string api_function_to_string(const api_functions_t& p_api_function);
api_functions_t api_function_string_to_enum(const std::string& p_str);

enum class api_family_t : uint8_t
{
  e_unknow,
  e_malloc,
  e_aligned_malloc,
  e_heapapi,
  e_localalloc,
  e_globalalloc,
};

std::string api_family_to_string(const api_family_t& p_api_family);


