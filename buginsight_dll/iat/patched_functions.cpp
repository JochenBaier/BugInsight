// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

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

//Based on VLD, Copyright 2005-2017 VLD Team (LGPL-2.1)
//BugInsight, Copyright 2024 Jochen Baier <email@jochen-baier.de>


#include "patched_functions.hpp"
#include "critical_section_monitor.hpp"
#include "malloc_monitor.hpp"
#include "mutex_monitor.hpp"
#include "strdup_monitor.hpp"
#include "_aligned_malloc_monitor.hpp"
#include "local_alloc_monitor.hpp"
#include "globalalloc_monitor.hpp"
#include "heap_monitor.hpp"
#include "getenvironmentvariable_monitor.hpp"
#include "memory_leak_map_with_callstack.hpp"

namespace patched_functions
{
void prepare_functions_to_patch(std::vector<function_to_patch_t>& m_functions_to_patch)
{
  //not yet working...
  //m_functions_to_patch.emplace_back(function_type_t::memory_leak, "GetProcAddress", "kernel32.dll", (void**)&GetProcAddress_fptr, buginsight_getprocaddress);

    //FIXME function typ
  m_functions_to_patch.emplace_back(function_type_t::deadlock, "GetEnvironmentVariableA", "kernel32.dll", (void**)&GetEnvironmentVariableA_fptr, buginsight_getenvironmentvariablea);

  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "HeapAlloc", "kernel32.dll", (void**)&HeapAlloc_fptr, buginsight_heapalloc);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "HeapReAlloc", "kernel32.dll", (void**)&HeapReAlloc_fptr, buginsight_heaprepalloc);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "HeapFree", "kernel32.dll", (void**)&HeapFree_fptr, buginsight_heapfree);

  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "LocalAlloc", "kernel32.dll", (void**)&LocalAlloc_fptr, buginsight_localalloc);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "LocalReAlloc", "kernel32.dll", (void**)&LocalReAlloc_fptr, buginsight_localrealloc);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "LocalFree", "kernel32.dll", (void**)&LocalFree_fptr, buginsight_localfree);

  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "GlobalAlloc", "kernel32.dll", (void**)&GlobalAlloc_fptr, buginsight_globalalloc);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "GlobalReAlloc", "kernel32.dll", (void**)&GlobalReAlloc_fptr, buginsight_globalrealloc);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "GlobalFree", "kernel32.dll", (void**)&GlobalFree_fptr, buginsight_globalfree);

  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "FormatMessageA", "kernel32.dll", (void**)&FormatMessageA_fptr, buginsight_formatmessagea);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "FormatMessageW", "kernel32.dll", (void**)&FormatMessageW_fptr, buginsight_formatmessagew);

  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "StrDupA", "shlwapi.dll", (void**)&StrDupA_fptr, buginsight_StrDupA);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "StrDupW", "shlwapi.dll", (void**)&StrDupW_fptr, buginsight_StrDupW);

  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "CryptProtectData", "crypt32.dll", (void**)&CryptProtectData_fptr, buginsight_CryptProtectData);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "CryptUnprotectData", "crypt32.dll", (void**)&CryptUnprotectData_fptr, buginsight_CryptUnprotectData);

  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "ConvertSidToStringSidW", "advapi32.dll", (void**)&ConvertSidToStringSidW_fptr, buginsight_ConvertSidToStringSidW);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "ConvertSidToStringSidA", "advapi32.dll", (void**)&ConvertSidToStringSidA_fptr, buginsight_ConvertSidToStringSidA);

  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "ConvertStringSidToSidW", "advapi32.dll", (void**)&ConvertStringSidToSidW_fptr, buginsight_ConvertStringSidToSidW);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "ConvertStringSidToSidA", "advapi32.dll", (void**)&ConvertStringSidToSidA_fptr, buginsight_ConvertStringSidToSidA);

  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "GetNamedSecurityInfoW", "advapi32.dll", (void**)&GetNamedSecurityInfoW_fptr, buginsight_GetNamedSecurityInfoW);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "GetNamedSecurityInfoA", "advapi32.dll", (void**)&GetNamedSecurityInfoA_fptr, buginsight_GetNamedSecurityInfoA);


  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "SetEntriesInAclA", "advapi32.dll", (void**)&SetEntriesInAclA_fptr, buginsight_SetEntriesInAclA);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "SetEntriesInAclW", "advapi32.dll", (void**)&SetEntriesInAclW_fptr, buginsight_SetEntriesInAclW);

  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "AllocateAndInitializeSid", "advapi32.dll", (void**)&AllocateAndInitializeSid_fptr, buginsight_AllocateAndInitializeSid);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "FreeSid", "advapi32.dll", (void**)&FreeSid_fptr, buginsight_FreeSid);

  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "GetSecurityInfo", "advapi32.dll", (void**)&GetSecurityInfo_fptr, buginsight_GetSecurityInfo);

  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "CommandLineToArgvW", "shell32.dll", (void**)&CommandLineToArgvW_fptr, buginsight_CommandLineToArgvW);


  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "malloc", "ucrtbase.dll", (void**)&malloc_monitor_t<false>::malloc_fptr, malloc_monitor_t<false>::bi_malloc);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "malloc", "ucrtbased.dll", (void**)&malloc_monitor_t<true>::malloc_fptr, malloc_monitor_t<true>::bi_malloc);

  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_expand", "ucrtbase.dll", (void**)&malloc_monitor_t<false>::_expand_fptr, malloc_monitor_t<false>::bi__expand);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_expand", "ucrtbased.dll", (void**)&malloc_monitor_t<true>::_expand_fptr, malloc_monitor_t<true>::bi__expand);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "calloc", "ucrtbase.dll", (void**)&malloc_monitor_t<false>::calloc_fptr, malloc_monitor_t<false>::bi_calloc);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "calloc", "ucrtbased.dll", (void**)&malloc_monitor_t<true>::calloc_fptr, malloc_monitor_t<true>::bi_calloc);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "realloc", "ucrtbase.dll", (void**)&malloc_monitor_t<false>::realloc_fptr, malloc_monitor_t<false>::bi_realloc);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "realloc", "ucrtbased.dll", (void**)&malloc_monitor_t<true>::realloc_fptr, malloc_monitor_t<true>::bi_realloc);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_recalloc", "ucrtbase.dll", (void**)&malloc_monitor_t<false>::_recalloc_fptr, malloc_monitor_t<false>::bi__recalloc);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_recalloc", "ucrtbased.dll", (void**)&malloc_monitor_t<true>::_recalloc_fptr, malloc_monitor_t<true>::bi__recalloc);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "free", "ucrtbase.dll", (void**)&malloc_monitor_t<false>::free_fptr, malloc_monitor_t<false>::bi_free);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "free", "ucrtbased.dll", (void**)&malloc_monitor_t<true>::free_fptr, malloc_monitor_t<true>::bi_free);

  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_strdup", "ucrtbase.dll", (void**)&strdup_monitor_t<false>::_strdup_fptr, strdup_monitor_t<false>::bi__strdup);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_strdup", "ucrtbased.dll", (void**)&strdup_monitor_t<true>::_strdup_fptr, strdup_monitor_t<true>::bi__strdup);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_wcsdup", "ucrtbase.dll", (void**)&strdup_monitor_t<false>::_wcsdup_fptr, (void**)&strdup_monitor_t<false>::bi__wcsdup);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_wcsdup", "ucrtbased.dll", (void**)&strdup_monitor_t<true>::_wcsdup_fptr, (void**)&strdup_monitor_t<true>::bi__wcsdup);

  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_aligned_malloc", "ucrtbase.dll", (void**)&_aligned_malloc_monitor_t<false>::_aligned_malloc_fptr, _aligned_malloc_monitor_t<false>::bi__aligned_malloc);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_aligned_malloc", "ucrtbased.dll", (void**)&_aligned_malloc_monitor_t<true>::_aligned_malloc_fptr, _aligned_malloc_monitor_t<true>::bi__aligned_malloc);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_aligned_free", "ucrtbase.dll", (void**)&_aligned_malloc_monitor_t<false>::_aligned_free_fptr, _aligned_malloc_monitor_t<false>::bi__aligned_free);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_aligned_free", "ucrtbased.dll", (void**)&_aligned_malloc_monitor_t<true>::_aligned_free_fptr, _aligned_malloc_monitor_t<true>::bi__aligned_free);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_aligned_offset_malloc", "ucrtbase.dll", (void**)&_aligned_malloc_monitor_t<false>::_aligned_offset_malloc_fptr, _aligned_malloc_monitor_t<false>::bi__aligned_offset_malloc);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_aligned_offset_malloc", "ucrtbased.dll", (void**)&_aligned_malloc_monitor_t<true>::_aligned_offset_malloc_fptr, _aligned_malloc_monitor_t<true>::bi__aligned_offset_malloc);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_aligned_offset_realloc", "ucrtbase.dll", (void**)&_aligned_malloc_monitor_t<false>::_aligned_offset_realloc_fptr, _aligned_malloc_monitor_t<false>::bi__aligned_offset_realloc);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_aligned_offset_realloc", "ucrtbased.dll", (void**)&_aligned_malloc_monitor_t<true>::_aligned_offset_realloc_fptr, _aligned_malloc_monitor_t<true>::bi__aligned_offset_realloc);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_aligned_offset_recalloc", "ucrtbase.dll", (void**)&_aligned_malloc_monitor_t<false>::_aligned_offset_recalloc_fptr, _aligned_malloc_monitor_t<false>::bi__aligned_offset_recalloc);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_aligned_offset_recalloc", "ucrtbased.dll", (void**)&_aligned_malloc_monitor_t<true>::_aligned_offset_recalloc_fptr, _aligned_malloc_monitor_t<true>::bi__aligned_offset_recalloc);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_aligned_realloc", "ucrtbase.dll", (void**)&_aligned_malloc_monitor_t<false>::_aligned_realloc_fptr, _aligned_malloc_monitor_t<false>::bi__aligned_realloc);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_aligned_realloc", "ucrtbased.dll", (void**)&_aligned_malloc_monitor_t<true>::_aligned_realloc_fptr, _aligned_malloc_monitor_t<true>::bi__aligned_realloc);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_aligned_recalloc", "ucrtbase.dll", (void**)&_aligned_malloc_monitor_t<false>::_aligned_recalloc_fptr, _aligned_malloc_monitor_t<false>::bi__aligned_recalloc);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_aligned_recalloc", "ucrtbased.dll", (void**)&_aligned_malloc_monitor_t<true>::_aligned_recalloc_fptr, _aligned_malloc_monitor_t<true>::bi__aligned_recalloc);


  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_getcwd", "ucrtbase.dll", (void**)&malloc_monitor_t<false>::_getcwd_fptr, malloc_monitor_t<false>::bi__getcwd);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_getcwd", "ucrtbased.dll", (void**)&malloc_monitor_t<true>::_getcwd_fptr, malloc_monitor_t<true>::bi__getcwd);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_wgetcwd", "ucrtbase.dll", (void**)&malloc_monitor_t<false>::_wgetcwd_fptr, malloc_monitor_t<false>::_wgetcwd);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_wgetcwd", "ucrtbased.dll", (void**)&malloc_monitor_t<true>::_wgetcwd_fptr, malloc_monitor_t<true>::_wgetcwd);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_getdcwd", "ucrtbase.dll", (void**)&malloc_monitor_t<false>::_getdcwd_fptr, malloc_monitor_t<false>::bi__getdcwd);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_getdcwd", "ucrtbased.dll", (void**)&malloc_monitor_t<true>::_getdcwd_fptr, malloc_monitor_t<true>::bi__getdcwd);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_wgetdcwd", "ucrtbase.dll", (void**)&malloc_monitor_t<false>::_wgetdcwd_fptr, malloc_monitor_t<false>::bi__wgetdcwd);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_wgetdcwd", "ucrtbased.dll", (void**)&malloc_monitor_t<true>::_wgetdcwd_fptr, malloc_monitor_t<true>::bi__wgetdcwd);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_tempnam", "ucrtbase.dll", (void**)&malloc_monitor_t<false>::_tempnam_fptr, malloc_monitor_t<false>::bi__tempnam);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_tempnam", "ucrtbased.dll", (void**)&malloc_monitor_t<true>::_tempnam_fptr, malloc_monitor_t<true>::bi__tempnam);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_wtempnam", "ucrtbase.dll", (void**)&malloc_monitor_t<false>::_wtempnam_fptr, malloc_monitor_t<false>::bi__wtempnam);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_wtempnam", "ucrtbased.dll", (void**)&malloc_monitor_t<true>::_wtempnam_fptr, malloc_monitor_t<true>::bi__wtempnam);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_dupenv_s", "ucrtbase.dll", (void**)&malloc_monitor_t<false>::_dupenv_s_fptr, malloc_monitor_t<false>::bi__dupenv_s);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_dupenv_s", "ucrtbased.dll", (void**)&malloc_monitor_t<true>::_dupenv_s_fptr, malloc_monitor_t<true>::bi__dupenv_s);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_wdupenv_s", "ucrtbase.dll", (void**)&malloc_monitor_t<false>::_wdupenv_s_fptr, malloc_monitor_t<false>::bi__wdupenv_s);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_wdupenv_s", "ucrtbased.dll", (void**)&malloc_monitor_t<true>::_wdupenv_s_fptr, malloc_monitor_t<true>::bi__wdupenv_s);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_fullpath", "ucrtbase.dll", (void**)&malloc_monitor_t<false>::_fullpath_fptr, malloc_monitor_t<false>::bi__fullpath);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_fullpath", "ucrtbased.dll", (void**)&malloc_monitor_t<true>::_fullpath_fptr, malloc_monitor_t<true>::bi__fullpath);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_wfullpath", "ucrtbase.dll", (void**)&malloc_monitor_t<false>::_wfullpath_fptr, malloc_monitor_t<false>::bi__wfullpath);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_wfullpath", "ucrtbased.dll", (void**)&malloc_monitor_t<true>::_wfullpath_fptr, malloc_monitor_t<true>::bi__wfullpath);

  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_malloc_dbg", "ucrtbased.dll", (void**)&malloc_monitor_t<true>::_malloc_dbg_fptr, malloc_monitor_t<true>::bi__malloc_dbg);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_expand_dbg", "ucrtbased.dll", (void**)&malloc_monitor_t<true>::_expand_dbg_fptr, malloc_monitor_t<true>::bi__expand_dbg);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_calloc_dbg", "ucrtbased.dll", (void**)&malloc_monitor_t<true>::_calloc_dbg_fptr, malloc_monitor_t<true>::bi__calloc_dbg);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_realloc_dbg", "ucrtbased.dll", (void**)&malloc_monitor_t<true>::_realloc_dbg_fptr, malloc_monitor_t<true>::bi__realloc_dbg);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_recalloc_dbg", "ucrtbased.dll", (void**)&malloc_monitor_t<true>::_recalloc_dbg_fptr, malloc_monitor_t<true>::bi__recalloc_dbg);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_free_dbg", "ucrtbased.dll", (void**)&malloc_monitor_t<true>::_free_dbg_fptr, malloc_monitor_t<true>::bi__free_dbg);

  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_strdup_dbg", "ucrtbased.dll", (void**)&strdup_monitor_t<true>::_strdup_dbg_fptr, strdup_monitor_t<true>::bi__strdup_dbg);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_wcsdup_dbg", "ucrtbase.dll", (void**)&strdup_monitor_t<false>::_wcsdup_dbg_fptr, (void**)&strdup_monitor_t<false>::bi__wcsdup_dbg);

  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_aligned_malloc_dbg", "ucrtbased.dll", (void**)&_aligned_malloc_monitor_t<true>::_aligned_malloc_dbg_fptr, (void**)&_aligned_malloc_monitor_t<true>::bi__aligned_malloc_dbg);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_aligned_free_dbg", "ucrtbased.dll", (void**)&_aligned_malloc_monitor_t<true>::_aligned_free_dbg_fptr, (void**)&_aligned_malloc_monitor_t<true>::bi__aligned_free_dbg);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_aligned_offset_malloc_dbg", "ucrtbased.dll", (void**)&_aligned_malloc_monitor_t<true>::_aligned_offset_malloc_dbg_fptr, (void**)&_aligned_malloc_monitor_t<true>::bi__aligned_offset_malloc_dbg);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_aligned_offset_realloc_dbg", "ucrtbased.dll", (void**)&_aligned_malloc_monitor_t<true>::_aligned_offset_realloc_dbg_fptr, (void**)&_aligned_malloc_monitor_t<true>::bi__aligned_offset_realloc_dbg);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_aligned_offset_recalloc_dbg", "ucrtbased.dll", (void**)&_aligned_malloc_monitor_t<true>::_aligned_offset_recalloc_dbg_fptr, (void**)&_aligned_malloc_monitor_t<true>::bi__aligned_offset_recalloc_dbg);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_aligned_realloc_dbg", "ucrtbased.dll", (void**)&_aligned_malloc_monitor_t<true>::_aligned_realloc_dbg_fptr, (void**)&_aligned_malloc_monitor_t<true>::bi__aligned_realloc_dbg);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_aligned_recalloc_dbg", "ucrtbased.dll", (void**)&_aligned_malloc_monitor_t<true>::_aligned_recalloc_dbg_fptr, (void**)&_aligned_malloc_monitor_t<true>::bi__aligned_recalloc_dbg);

  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_getcwd_dbg", "ucrtbased.dll", (void**)&malloc_monitor_t<true>::_getcwd_dbg_fptr, malloc_monitor_t<true>::bi__getcwd_dbg);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_wgetcwd_dbg", "ucrtbased.dll", (void**)&malloc_monitor_t<true>::_wgetcwd_dbg_fptr, malloc_monitor_t<true>::bi__wgetcwd_dbg);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_getdcwd_dbg", "ucrtbased.dll", (void**)&malloc_monitor_t<true>::_getdcwd_dbg_ftpr, malloc_monitor_t<true>::bi__getdcwd_dbg);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_wgetdcwd_dbg", "ucrtbased.dll", (void**)&malloc_monitor_t<true>::_wgetdcwd_dbg_fptr, malloc_monitor_t<true>::bi__wgetdcwd_dbg);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_tempnam_dbg", "ucrtbased.dll", (void**)&malloc_monitor_t<true>::_tempnam_dbg_fptr, malloc_monitor_t<true>::bi__tempnam_dbg);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_wtempnam_dbg", "ucrtbased.dll", (void**)&malloc_monitor_t<true>::_wtempnam_dbg_fptr, malloc_monitor_t<true>::bi__wtempnam_dbg);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_dupenv_s_dbg", "ucrtbased.dll", (void**)&malloc_monitor_t<true>::_dupenv_s_dbg_fptr, malloc_monitor_t<true>::bi__dupenv_s_dbg);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_wdupenv_s_dbg", "ucrtbased.dll", (void**)&malloc_monitor_t<true>::_wdupenv_s_dbg_fptr, malloc_monitor_t<true>::bi__wdupenv_dbg_s);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_fullpath_dbg", "ucrtbased.dll", (void**)&malloc_monitor_t<true>::_fullpath_dbg_fptr, malloc_monitor_t<true>::bi__fullpath_dbg);
  m_functions_to_patch.emplace_back(function_type_t::memory_leak, "_wfullpath_dbg", "ucrtbased.dll", (void**)&malloc_monitor_t<true>::_wfullpath_dbg_fptr, malloc_monitor_t<true>::bi__wfullpath_dbg);

  m_functions_to_patch.emplace_back(function_type_t::deadlock, "InitializeCriticalSection", "kernel32.dll", (void**)&InitializeCriticalSection_fptr, buginsight_initialize_critical_section);
  m_functions_to_patch.emplace_back(function_type_t::deadlock, "InitializeCriticalSectionAndSpinCount", "kernel32.dll", (void**)&InitializeCriticalSectionAndSpinCount_fptr, buginsight_initialize_critical_section_andspincount);
  m_functions_to_patch.emplace_back(function_type_t::deadlock, "TryEnterCriticalSection", "kernel32.dll", (void**)&TryEnterCriticalSection_fptr, buginsight_tryenter_critical_section);
  m_functions_to_patch.emplace_back(function_type_t::deadlock, "EnterCriticalSection", "kernel32.dll", (void**)&EnterCriticalSection_fptr, buginsight_enter_critical_section);
  m_functions_to_patch.emplace_back(function_type_t::deadlock, "LeaveCriticalSection", "kernel32.dll", (void**)&LeaveCriticalSection_fptr, buginsight_leave_critical_section);
  m_functions_to_patch.emplace_back(function_type_t::deadlock, "DeleteCriticalSection", "kernel32.dll", (void**)&DeleteCriticalSection_fptr, buginsight_delete_critical_section);

  m_functions_to_patch.emplace_back(function_type_t::deadlock, "_Mtx_lock", "msvcp140.dll", (void**)&mutex_monitor_t<false>::_Mtx_lock_fptr, mutex_monitor_t<false>::bi_mtx_lock);
  m_functions_to_patch.emplace_back(function_type_t::deadlock, "_Mtx_lock", "msvcp140d.dll", (void**)&mutex_monitor_t<true>::_Mtx_lock_fptr, mutex_monitor_t<true>::bi_mtx_lock);
  m_functions_to_patch.emplace_back(function_type_t::deadlock, "_Mtx_unlock", "msvcp140.dll", (void**)&mutex_monitor_t<false>::_Mtx_unlock_fptr, mutex_monitor_t<false>::bi_mtx_unlock);
  m_functions_to_patch.emplace_back(function_type_t::deadlock, "_Mtx_unlock", "msvcp140d.dll", (void**)&mutex_monitor_t<true>::_Mtx_unlock_fptr, mutex_monitor_t<true>::bi_mtx_unlock);
  m_functions_to_patch.emplace_back(function_type_t::deadlock, "_Mtx_trylock", "msvcp140.dll", (void**)&mutex_monitor_t<false>::_Mtx_trylock_fptr, mutex_monitor_t<false>::bi_mtx_trylock);
  m_functions_to_patch.emplace_back(function_type_t::deadlock, "_Mtx_trylock", "msvcp140d.dll", (void**)&mutex_monitor_t<true>::_Mtx_trylock_fptr, mutex_monitor_t<true>::bi_mtx_trylock);
  m_functions_to_patch.emplace_back(function_type_t::deadlock, "_Mtx_init_in_situ", "msvcp140.dll", (void**)&mutex_monitor_t<false>::_Mtx_init_in_situ_fptr, mutex_monitor_t<false>::bi_init_in_situ);
  m_functions_to_patch.emplace_back(function_type_t::deadlock, "_Mtx_init_in_situ", "msvcp140d.dll", (void**)&mutex_monitor_t<true>::_Mtx_init_in_situ_fptr, mutex_monitor_t<true>::bi_init_in_situ);
  m_functions_to_patch.emplace_back(function_type_t::deadlock, "_Mtx_destroy_in_situ", "msvcp140.dll", (void**)&mutex_monitor_t<false>::_Mtx_destroy_in_situ_fptr, mutex_monitor_t<false>::bi_mtx_destroy_in_situ);
  m_functions_to_patch.emplace_back(function_type_t::deadlock, "_Mtx_destroy_in_situ", "msvcp140d.dll", (void**)&mutex_monitor_t<true>::_Mtx_destroy_in_situ_fptr, mutex_monitor_t<true>::bi_mtx_destroy_in_situ);

  m_functions_to_patch.emplace_back(function_type_t::deadlock, "_Cnd_wait", "msvcp140.dll", (void**)&mutex_monitor_t<false>::_Cnd_wait_fptr, mutex_monitor_t<false>::bi__cnd_wait);
  m_functions_to_patch.emplace_back(function_type_t::deadlock, "_Cnd_wait", "msvcp140d.dll", (void**)&mutex_monitor_t<true>::_Cnd_wait_fptr, mutex_monitor_t<true>::bi__cnd_wait);


  m_functions_to_patch.emplace_back(function_type_t::deadlock, "_Cnd_timedwait", "msvcp140.dll", (void**)&mutex_monitor_t<false>::_Cnd_timedwait_fptr, &mutex_monitor_t<false>::bi__cnd_timedwait);
  m_functions_to_patch.emplace_back(function_type_t::deadlock, "_Cnd_timedwait", "msvcp140d.dll", (void**)&mutex_monitor_t<true>::_Cnd_timedwait_fptr, &mutex_monitor_t<true>::bi__cnd_timedwait);

}

}


