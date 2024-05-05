// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

//Licensed to the Apache Software Foundation (ASF) under one
//or more contributor license agreements.  See the NOTICE file
//distributed with this work for additional information
//regarding copyright ownership.  The ASF licenses this file
//to you under the Apache License, Version 2.0 (the
//"License"); you may not use this file except in compliance
//with the License.  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing,
//software distributed under the License is distributed on an
//"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
//KIND, either express or implied.  See the License for the
//specific language governing permissions and limitations
//under the License.  

//BugInsight, Copyright 2024 Jochen Baier <email@jochen-baier.de>


#include "leak_types.hpp"

#include "boost/algorithm/string.hpp"

std::string api_function_to_string(const api_functions_t& p_api_function)
{
  switch(p_api_function)
  {
  case api_functions_t::e_HeapCreate: return "HeapCreate";
  case api_functions_t::e_HeapDestroy: return "HeapDestroy";
  case api_functions_t::e_HeapAlloc: return "HeapAlloc";
  case api_functions_t::e_HeapReAlloc: return "HeapReAlloc";
  case api_functions_t::e_HeapFree: return "HeapFree";

  case api_functions_t::e_malloc: return "malloc";
  case api_functions_t::e__malloc_dbg: return "_malloc_dbg";
  case api_functions_t::e_realloc: return "realloc";
  case api_functions_t::e__realloc_dbg: return "_realloc_dbg";
  case api_functions_t::e__recalloc: return "_recalloc";
  case api_functions_t::e__recalloc_dbg: return "_recalloc_dbg";
  case api_functions_t::e_calloc: return "calloc";
  case api_functions_t::e__calloc_dbg: return "_calloc_dbg";
  case api_functions_t::e__expand: return "_expand";
  case api_functions_t::e__expand_dbg: return "_expand_dbg";
  case api_functions_t::e_free: return "free";
  case api_functions_t::e__free_dbg: return "_free_dbg";

  case api_functions_t::e__getcwd: return "_getcwd";
  case api_functions_t::e__getcwd_dbg: return "_getcwd_dbg";
  case api_functions_t::e__wgetcwd: return "_wgetcwd";
  case api_functions_t::e__wgetcwd_dbg: return "_wgetcwd_dbg";
  case api_functions_t::e__getdcwd: return "_getdcwd";
  case api_functions_t::e__getdcwd_dbg: return "_getdcwd_dbg";
  case api_functions_t::e__wgetdcwd: return "_wgetdcwd";
  case api_functions_t::e__wgetdcwd_dbg: return "_wgetdcwd_dbg";
  case api_functions_t::e__tempnam: return "_tempnam";
  case api_functions_t::e__tempnam_dbg: return "_tempnam_dbg";
  case api_functions_t::e__wtempnam: return "_wtempnam";
  case api_functions_t::e__wtempnam_dbg: return "_wtempnam_dbg";

  case api_functions_t::e_localalloc: return "LocalAlloc";
  case api_functions_t::e_localrealloc: return "LocalReAlloc";
  case api_functions_t::e_localfree: return "LocalFree";

  case api_functions_t::e_globalalloc: return "GlobalAlloc";
  case api_functions_t::e_globrealalloc: return "GlobalReAlloc";
  case api_functions_t::e_globalfree: return "GlobalFree";

  case api_functions_t::e__aligned_malloc: return "_aligned_malloc";
  case api_functions_t::e__aligned_malloc_dbg: return "_aligned_malloc_dbg";
  case api_functions_t::e__aligned_offset_malloc: return "_aligned_offset_malloc";
  case api_functions_t::e__aligned_offset_malloc_dbg: return "_aligned_offset_malloc_dbg";
  case api_functions_t::e__aligned_offset_realloc: return "_aligned_offset_realloc";
  case api_functions_t::e__aligned_offset_realloc_dbg: return "_aligned_offset_realloc_dbg";
  case api_functions_t::e__aligned_offset_recalloc: return "_aligned_offset_recalloc";
  case api_functions_t::e__aligned_offset_recalloc_dbg: return "_aligned_offset_recalloc_dbg";
  case api_functions_t::e__aligned_realloc: return "_aligned_realloc";
  case api_functions_t::e__aligned_realloc_dbg: return "_aligned_realloc_dbg";
  case api_functions_t::e__aligned_recalloc: return "_aligned_recalloc";
  case api_functions_t::e__aligned_recalloc_dbg: return "_aligned_recalloc_dbg";
  case api_functions_t::e__aligned_free: return "_aligned_free";
  case api_functions_t::e__aligned_free_dbg: return "_aligned_free_dbg";

  case api_functions_t::e__strdup: return "_strdup";
  case api_functions_t::e__strdup_dbg: return "_strdup_dbg";
  case api_functions_t::e__wcsdup: return "_wcsdup";
  case api_functions_t::e__wcsdup_dbg: return "_wcsdup_dbg";

  case api_functions_t::e__dupenv_s: return "_dupenv_s";
  case api_functions_t::e__dupenv_s_dbg: return "_dupenv_s_dbg";
  case api_functions_t::e__wdupenv_s: return "_wdupenv_s";
  case api_functions_t::e__wdupenv_s_dbg: return "_wdupenv_s_dbg";
  case api_functions_t::e__fullpath: return "_fullpath";
  case api_functions_t::e__fullpath_dbg: return "_fullpath_dbg";
  case api_functions_t::e__wfullpath: return "_wfullpath";
  case api_functions_t::e__wfullpath_dbg: return "_wfullpath_dbg";
  case api_functions_t::e_FormatMessageA: return "FormatMessageA";
  case api_functions_t::e_FormatMessageW: return "FormatMessageW";
  case api_functions_t::e_CommandLineToArgvW: return "CommandLineToArgvW";

  case api_functions_t::e_ConvertSidToStringSidW: return "ConvertSidToStringSidW";
  case api_functions_t::e_ConvertSidToStringSidA: return "ConvertSidToStringSidA";

  case api_functions_t::e_ConvertStringSidToSidW: return "ConvertStringSidToSidW";
  case api_functions_t::e_ConvertStringSidToSidA: return "ConvertStringSidToSidA";

  case api_functions_t::e_CryptProtectData: return "CryptProtectData";
  case api_functions_t::e_CryptUnprotectData: return "CryptUnprotectData";

  case api_functions_t::e_GetNamedSecurityInfoW: return "GetNamedSecurityInfoW";
  case api_functions_t::e_GetNamedSecurityInfoA: return "GetNamedSecurityInfoA";

  case api_functions_t::e_SetEntriesInAclA: return "SetEntriesInAclA";
  case api_functions_t::e_SetEntriesInAclW: return "SetEntriesInAclW";

  case api_functions_t::e_AllocateAndInitializeSid: return "AllocateAndInitializeSid";
  case api_functions_t::e_FreeSid: return "FreeSid";
  case api_functions_t::e_GetSecurityInfo: return "GetSecurityInfo";
  case api_functions_t::e_StrDupA: return " StrDupA";
  case api_functions_t::e_StrDupW: return " StrDupW";



  case api_functions_t::e_unknown:
    [[fallthrough]];
  default:  // NOLINT(clang-diagnostic-covered-switch-default)
  {
    assert(false); return "Unknown";
  }
  }
}

api_functions_t api_function_string_to_enum(const std::string& p_str)
{
  const std::string str_lower=boost::to_lower_copy(p_str);

  if(str_lower == "heapcreate") return api_functions_t::e_HeapCreate;
  if(str_lower == "heapdestroy") return api_functions_t::e_HeapDestroy;
  if(str_lower == "heapalloc") return api_functions_t::e_HeapAlloc;
  if(str_lower == "heaprealloc") return api_functions_t::e_HeapReAlloc;
  if(str_lower == "heapfree") return api_functions_t::e_HeapFree;
  if(str_lower == "malloc") return api_functions_t::e_malloc;
  if(str_lower == "_malloc_dbg") return api_functions_t::e__malloc_dbg;
  if(str_lower == "realloc") return api_functions_t::e_realloc;
  if(str_lower == "_realloc_dbg") return api_functions_t::e__realloc_dbg;
  if(str_lower == "calloc") return api_functions_t::e_calloc;
  if(str_lower == "_calloc_dbg") return api_functions_t::e__calloc_dbg;
  if(str_lower == "_recalloc") return api_functions_t::e__recalloc;
  if(str_lower == "_recalloc_dbg") return api_functions_t::e__recalloc_dbg;
  if(str_lower == "strdup") return api_functions_t::e__strdup;
  if(str_lower == "_strdup") return api_functions_t::e__strdup;
  if(str_lower == "_strdup_dbg") return api_functions_t::e__strdup_dbg;
  if(str_lower == "wcsdup") return api_functions_t::e__wcsdup;
  if(str_lower == "_wcsdup") return api_functions_t::e__wcsdup;
  if(str_lower == "_wcsdup_dbg") return api_functions_t::e__wcsdup_dbg;
  if(str_lower == "_expand") return api_functions_t::e__expand;
  if(str_lower == "_expand_dbg") return api_functions_t::e__expand_dbg;
  if(str_lower == "free") return api_functions_t::e_free;
  if(str_lower == "_free_dbg") return api_functions_t::e__free_dbg;
  if(str_lower == "_getcwd") return api_functions_t::e__getcwd;
  if(str_lower == "_getcwd_dbg") return api_functions_t::e__getcwd_dbg;
  if(str_lower == "_wgetcwd") return api_functions_t::e__wgetcwd;
  if(str_lower == "_wgetcwd_dbg") return api_functions_t::e__wgetcwd_dbg;
  if(str_lower == "_getdcwd") return api_functions_t::e__getdcwd;
  if(str_lower == "_getdcwd_dbg") return api_functions_t::e__getdcwd_dbg;
  if(str_lower == "_wgetdcwd") return api_functions_t::e__wgetdcwd;
  if(str_lower == "_wgetdcwd_dbg") return api_functions_t::e__wgetdcwd_dbg;
  if(str_lower == "_tempnam") return api_functions_t::e__tempnam;
  if(str_lower == "_tempnam_dbg") return api_functions_t::e__tempnam_dbg;
  if(str_lower == "_wtempnam") return api_functions_t::e__wtempnam;
  if(str_lower == "_wtempnam_dbg") return api_functions_t::e__wtempnam_dbg;
  if(str_lower == "_aligned_malloc") return api_functions_t::e__aligned_malloc;
  if(str_lower == "_aligned_malloc_dbg") return api_functions_t::e__aligned_malloc_dbg;
  if(str_lower == "_aligned_offset_malloc") return api_functions_t::e__aligned_offset_malloc;
  if(str_lower == "_aligned_offset_malloc_dbg") return api_functions_t::e__aligned_offset_malloc_dbg;
  if(str_lower == "_aligned_offset_realloc") return api_functions_t::e__aligned_offset_realloc;
  if(str_lower == "_aligned_offset_realloc_dbg") return api_functions_t::e__aligned_offset_realloc_dbg;
  if(str_lower == "_aligned_offset_recalloc") return api_functions_t::e__aligned_offset_recalloc;
  if(str_lower == "_aligned_offset_recalloc_dbg") return api_functions_t::e__aligned_offset_recalloc_dbg;
  if(str_lower == "_aligned_realloc") return api_functions_t::e__aligned_realloc;
  if(str_lower == "_aligned_realloc_dbg") return api_functions_t::e__aligned_realloc_dbg;
  if(str_lower == "_aligned_recalloc") return api_functions_t::e__aligned_recalloc;
  if(str_lower == "_aligned_recalloc_dbg") return api_functions_t::e__aligned_recalloc_dbg;
  if(str_lower == "_aligned_free") return api_functions_t::e__aligned_free;
  if(str_lower == "_aligned_free_dbg") return api_functions_t::e__aligned_free_dbg;
  if(str_lower == "localalloc") return api_functions_t::e_localalloc;
  if(str_lower == "localfree") return api_functions_t::e_localfree;
  if(str_lower == "localrealloc") return api_functions_t::e_localrealloc;
  if(str_lower == "globalalloc") return api_functions_t::e_globalalloc;
  if(str_lower == "globalfree") return api_functions_t::e_globalfree;
  if(str_lower == "globalrealloc") return api_functions_t::e_globrealalloc;
  if(str_lower == "formatmessagea") return api_functions_t::e_FormatMessageA;
  if(str_lower == "formatmessagew") return api_functions_t::e_FormatMessageW;

  if(str_lower == "commandlinetoargvw") return api_functions_t::e_CommandLineToArgvW;

  if(str_lower == "convertsidtostringsidw") return api_functions_t::e_ConvertSidToStringSidW;
  if(str_lower == "convertsidtostringsida") return api_functions_t::e_ConvertSidToStringSidA;

  if(str_lower == "convertstringsidtosidw") return api_functions_t::e_ConvertStringSidToSidW;
  if(str_lower == "convertstringsidtosida") return api_functions_t::e_ConvertStringSidToSidA;

  if(str_lower == "cryptprotectdata") return api_functions_t::e_CryptProtectData;
  if(str_lower == "cryptunprotectdata") return api_functions_t::e_CryptUnprotectData;

  if(str_lower == "getnamedsecurityinfow") return api_functions_t::e_GetNamedSecurityInfoW;
  if(str_lower == "getnamedsecurityinfoa") return api_functions_t::e_GetNamedSecurityInfoA;

  if(str_lower == "setentriesinacla") return api_functions_t::e_SetEntriesInAclA;
  if(str_lower == "setentriesinaclw") return api_functions_t::e_SetEntriesInAclW;

  if(str_lower == "allocateandinitializesid") return api_functions_t::e_AllocateAndInitializeSid;
  if(str_lower == "freesid") return api_functions_t::e_FreeSid;
  if(str_lower == "getsecurityinfo") return api_functions_t::e_GetSecurityInfo;
  if(str_lower == "strdupa") return api_functions_t::e_StrDupA;
  if(str_lower == "strdupw") return api_functions_t::e_StrDupW;

  assert(false);

  // Default value if string doesn't match any enum
  return api_functions_t::e_unknown;
}

std::string api_family_to_string(const api_family_t& p_api_family)
{
  switch(p_api_family)
  {
  case  api_family_t::e_malloc:return "malloc";
  case  api_family_t::e_aligned_malloc:return "_aligned_malloc";
  case  api_family_t::e_heapapi:return "HeapAlloc";
  case  api_family_t::e_localalloc:return "LocalAlloc";
  case  api_family_t::e_globalalloc:return "GlobalAlloc";
  case  api_family_t::e_unknow:  // NOLINT(clang-diagnostic-covered-switch-default)
    [[fallthrough]];
  default: assert(false); return "unknow";
  }

}

