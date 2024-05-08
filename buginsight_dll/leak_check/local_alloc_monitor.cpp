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

//BugInsight, Copyright 2024 Jochen Baier <email@jochen-baier.de>


#include "local_alloc_monitor.hpp"

#include <memory_leak_map_with_callstack.hpp>
#include "memory_helper.hpp"
#include "run_time_options.hpp"

#include <Windows.h>
#include <sddl.h>
#include <aclapi.h>
#include <shlwapi.h>

extern memory_leak_repository_t* g_memory_leak_repository;
extern std::atomic_uint64_t g_monitored_api_calls;

HLOCAL buginsight_localfree(HLOCAL hMem)
{
  ++g_monitored_api_calls;

  assert(g_memory_leak_repository);

  if(hMem == nullptr)
  {
    return LocalFree_fptr(hMem);
  }

  const bool found_address=memory_helper::log_or_abort_if_memory_adddress_is_unknown(*g_memory_leak_repository, hMem, api_family_t::e_localalloc, api_functions_t::e_localfree);
  if(found_address)
  {
    g_memory_leak_repository->erase((uintptr_t)hMem);
  }

  return LocalFree(hMem);
}

HLOCAL buginsight_localalloc(UINT uFlags, SIZE_T uBytes)
{
  ++g_monitored_api_calls;

  HLOCAL p=LocalAlloc_fptr(uFlags, uBytes);
  if(!p)
  {
    return p;
  }

  g_memory_leak_repository->add((uintptr_t)p, uBytes, api_family_t::e_localalloc, api_functions_t::e_localalloc);
  return p;
}

HLOCAL buginsight_localrealloc(HLOCAL hMem, SIZE_T uBytes, UINT uFlags)
{
  ++g_monitored_api_calls;

  assert(g_memory_leak_repository);

  bool found_address=false;

  if(hMem)
  {
    found_address=memory_helper::log_or_abort_if_memory_adddress_is_unknown(*g_memory_leak_repository, hMem, api_family_t::e_localalloc, api_functions_t::e_localrealloc);
  }

  if(found_address)
  {
    g_memory_leak_repository->erase((uintptr_t)hMem);
  }

  void* p=::LocalReAlloc_fptr(hMem, uBytes, uFlags);
  if(!p)
  {
    return p;
  }

  g_memory_leak_repository->add((uintptr_t)p, uBytes, api_family_t::e_localalloc, api_functions_t::e_localrealloc);
  return p;
}

DWORD buginsight_formatmessagea(DWORD dwFlags, LPCVOID lpSource, DWORD  dwMessageId, DWORD  dwLanguageId, LPTSTR lpBuffer, DWORD nSize, va_list* Arguments)
{
  ++g_monitored_api_calls;

  DWORD res=FormatMessageA_fptr(dwFlags, lpSource, dwMessageId, dwLanguageId, lpBuffer, nSize, Arguments);
  if(!res)
  {
    return res;
  }

  if((dwFlags & FORMAT_MESSAGE_ALLOCATE_BUFFER) != FORMAT_MESSAGE_ALLOCATE_BUFFER)
  {
    return res;
  }

  const LPSTR* buffer_ptr=(LPSTR*)lpBuffer;


  const size_t length=strlen(*buffer_ptr);
  assert(length);

  g_memory_leak_repository->add((uintptr_t)*buffer_ptr, length + 1, api_family_t::e_localalloc, api_functions_t::e_FormatMessageA);
  return res;
}

DWORD buginsight_formatmessagew(DWORD dwFlags, LPCVOID lpSource, DWORD dwMessageId, DWORD dwLanguageId, LPWSTR lpBuffer, DWORD nSize, va_list* Arguments)
{
  ++g_monitored_api_calls;

  DWORD res=FormatMessageW_fptr(dwFlags, lpSource, dwMessageId, dwLanguageId, lpBuffer, nSize, Arguments);
  if(!res)
  {
    return res;
  }

  if((dwFlags & FORMAT_MESSAGE_ALLOCATE_BUFFER) != FORMAT_MESSAGE_ALLOCATE_BUFFER)
  {
    return res;
  }

  const LPWSTR* buffer_ptr=(LPWSTR*)lpBuffer;

  const size_t char_count=wcslen(*buffer_ptr);

  assert(char_count);


  g_memory_leak_repository->add((uintptr_t)*buffer_ptr, (char_count + 1) * sizeof(wchar_t), api_family_t::e_localalloc, api_functions_t::e_FormatMessageW);
  return res;
}

LPWSTR* buginsight_CommandLineToArgvW(LPCWSTR lpCmdLine, int* pNumArgs)
{
  ++g_monitored_api_calls;

  LPWSTR* szArglist=CommandLineToArgvW(lpCmdLine, pNumArgs);
  if(!szArglist)
  {
    return szArglist;
  }

  const SIZE_T size=LocalSize(szArglist);

  g_memory_leak_repository->add((uintptr_t)szArglist, size, api_family_t::e_localalloc, api_functions_t::e_CommandLineToArgvW);

  return szArglist;
}

BOOL WINAPI buginsight_CryptProtectData(DATA_BLOB* pDataIn, LPCWSTR szDataDescr, DATA_BLOB* pOptionalEntropy, PVOID pvReserved, CRYPTPROTECT_PROMPTSTRUCT* pPromptStruct,
  DWORD dwFlags, DATA_BLOB* pDataOut)
{
  ++g_monitored_api_calls;

  const BOOL ok=CryptProtectData_fptr(pDataIn, szDataDescr, pOptionalEntropy, pvReserved, pPromptStruct, dwFlags, pDataOut);
  if(!ok)
  {
    return ok;
  }

  assert(pDataOut->pbData);
  const SIZE_T size=LocalSize(pDataOut->pbData);

  g_memory_leak_repository->add((uintptr_t)pDataOut->pbData, size, api_family_t::e_localalloc, api_functions_t::e_CryptProtectData);

  return ok;
}

BOOL WINAPI buginsight_CryptUnprotectData(DATA_BLOB* pDataIn, LPWSTR* ppszDataDescr, DATA_BLOB* pOptionalEntropy, PVOID pvReserved, CRYPTPROTECT_PROMPTSTRUCT* pPromptStruct,
  DWORD dwFlags, DATA_BLOB* pDataOut)
{
  ++g_monitored_api_calls;

  const BOOL ok=CryptUnprotectData_fptr(pDataIn, ppszDataDescr, pOptionalEntropy, pvReserved, pPromptStruct, dwFlags, pDataOut);
  if(!ok)
  {
    return ok;
  }

  if(ppszDataDescr)
  {
    const LPWSTR* ppszDataDescr_ptr=(LPWSTR*)ppszDataDescr;

    const SIZE_T size=LocalSize(*ppszDataDescr_ptr);
    g_memory_leak_repository->add((uintptr_t)*ppszDataDescr_ptr, size, api_family_t::e_localalloc, api_functions_t::e_CryptUnprotectData);
  }

  assert(pDataOut->pbData);
  const SIZE_T size=LocalSize(pDataOut->pbData);
  g_memory_leak_repository->add((uintptr_t)pDataOut->pbData, size, api_family_t::e_localalloc, api_functions_t::e_CryptUnprotectData);

  return ok;
}

BOOL WINAPI buginsight_ConvertSidToStringSidW(PSID Sid, LPWSTR* StringSid)
{
  ++g_monitored_api_calls;

  const BOOL ok=ConvertSidToStringSidW_fptr(Sid, StringSid);
  if(!ok)
  {
    return ok;
  }

  const SIZE_T size=LocalSize(*StringSid);

  g_memory_leak_repository->add((uintptr_t)*StringSid, size, api_family_t::e_localalloc, api_functions_t::e_ConvertSidToStringSidW);

  return ok;
}

BOOL WINAPI buginsight_ConvertSidToStringSidA(PSID Sid, LPSTR* StringSid)
{
  ++g_monitored_api_calls;

  const BOOL ok=ConvertSidToStringSidA_fptr(Sid, StringSid);
  if(!ok)
  {
    return ok;
  }

  const SIZE_T size=LocalSize(*StringSid);

  g_memory_leak_repository->add((uintptr_t)*StringSid, size, api_family_t::e_localalloc, api_functions_t::e_ConvertSidToStringSidA);

  return ok;
}

BOOL WINAPI buginsight_ConvertStringSidToSidW(LPCWSTR StringSid, PSID* Sid)
{
  ++g_monitored_api_calls;

  const BOOL ok=ConvertStringSidToSidW_fptr(StringSid, Sid);
  if(!ok)
  {
    return ok;
  }

  const SIZE_T size=LocalSize(*Sid);

  g_memory_leak_repository->add((uintptr_t)*Sid, size, api_family_t::e_localalloc, api_functions_t::e_ConvertStringSidToSidW);

  return ok;
}

BOOL WINAPI buginsight_ConvertStringSidToSidA(LPCSTR StringSid, PSID* Sid)
{
  ++g_monitored_api_calls;

  const BOOL ok=ConvertStringSidToSidA_fptr(StringSid, Sid);
  if(!ok)
  {
    return ok;
  }

  const SIZE_T size=LocalSize(*Sid);

  g_memory_leak_repository->add((uintptr_t)*Sid, size, api_family_t::e_localalloc, api_functions_t::e_ConvertStringSidToSidA);

  return ok;
}

DWORD WINAPI buginsight_GetNamedSecurityInfoW(LPCWSTR pObjectName, SE_OBJECT_TYPE ObjectType, SECURITY_INFORMATION  SecurityInfo, PSID* ppsidOwner, PSID* ppsidGroup, PACL* ppDacl, PACL* ppSacl,
  PSECURITY_DESCRIPTOR* ppSecurityDescriptor)
{
  ++g_monitored_api_calls;

  const DWORD res=GetNamedSecurityInfoW_fptr(pObjectName, ObjectType, SecurityInfo, ppsidOwner, ppsidGroup, ppDacl, ppSacl, ppSecurityDescriptor);
  if(res != ERROR_SUCCESS)
  {
    return res;
  }

  const SIZE_T size=LocalSize(*ppSecurityDescriptor);

  g_memory_leak_repository->add((uintptr_t)*ppSecurityDescriptor, size, api_family_t::e_localalloc, api_functions_t::e_GetNamedSecurityInfoW);

  return res;
}

DWORD WINAPI buginsight_GetNamedSecurityInfoA(LPCSTR pObjectName, SE_OBJECT_TYPE ObjectType, SECURITY_INFORMATION  SecurityInfo, PSID* ppsidOwner, PSID* ppsidGroup, PACL* ppDacl, PACL* ppSacl,
  PSECURITY_DESCRIPTOR* ppSecurityDescriptor)
{
  ++g_monitored_api_calls;

  const DWORD res=GetNamedSecurityInfoA_fptr(pObjectName, ObjectType, SecurityInfo, ppsidOwner, ppsidGroup, ppDacl, ppSacl, ppSecurityDescriptor);
  if(res != ERROR_SUCCESS)
  {
    return res;
  }

  const SIZE_T size=LocalSize(*ppSecurityDescriptor);

  g_memory_leak_repository->add((uintptr_t)*ppSecurityDescriptor, size, api_family_t::e_localalloc, api_functions_t::e_GetNamedSecurityInfoA);

  return res;
}

DWORD WINAPI buginsight_SetEntriesInAclA(ULONG cCountOfExplicitEntries, PEXPLICIT_ACCESS_A  pListOfExplicitEntries, PACL OldAcl, PACL* NewAcl)
{
  ++g_monitored_api_calls;

  const DWORD res=SetEntriesInAclA_fptr(cCountOfExplicitEntries, pListOfExplicitEntries, OldAcl, NewAcl);
  if(res != ERROR_SUCCESS)
  {
    return res;
  }

  const SIZE_T size=LocalSize(*NewAcl);

  g_memory_leak_repository->add((uintptr_t)*NewAcl, size, api_family_t::e_localalloc, api_functions_t::e_SetEntriesInAclA);

  return res;
}

DWORD WINAPI buginsight_SetEntriesInAclW(ULONG cCountOfExplicitEntries, PEXPLICIT_ACCESS_W  pListOfExplicitEntries, PACL OldAcl, PACL* NewAcl)
{
  ++g_monitored_api_calls;

  const DWORD res=SetEntriesInAclW_fptr(cCountOfExplicitEntries, pListOfExplicitEntries, OldAcl, NewAcl);
  if(res != ERROR_SUCCESS)
  {
    return res;
  }

  const SIZE_T size=LocalSize(*NewAcl);

  g_memory_leak_repository->add((uintptr_t)*NewAcl, size, api_family_t::e_localalloc, api_functions_t::e_SetEntriesInAclW);

  return res;
}

//should be moved to handle leaks...
BOOL buginsight_AllocateAndInitializeSid(PSID_IDENTIFIER_AUTHORITY pIdentifierAuthority, BYTE nSubAuthorityCount, DWORD  nSubAuthority0, DWORD  nSubAuthority1, DWORD  nSubAuthority2,
  DWORD  nSubAuthority3, DWORD  nSubAuthority4, DWORD  nSubAuthority5, DWORD  nSubAuthority6, DWORD  nSubAuthority7, PSID* pSid)
{
  ++g_monitored_api_calls;

  const BOOL ok=AllocateAndInitializeSid(pIdentifierAuthority, nSubAuthorityCount, nSubAuthority0, nSubAuthority1, nSubAuthority2,
    nSubAuthority3, nSubAuthority4, nSubAuthority5, nSubAuthority6, nSubAuthority7, pSid);
  if(!ok)
  {
    return ok;
  }

  const SIZE_T size=LocalSize(*pSid);

  g_memory_leak_repository->add((uintptr_t)*pSid, size, api_family_t::e_localalloc, api_functions_t::e_AllocateAndInitializeSid);

  return ok;
}

PVOID WINAPI buginsight_FreeSid(PSID pSid)
{
  ++g_monitored_api_calls;

  assert(g_memory_leak_repository);

  if(pSid == nullptr)
  {
    return FreeSid_fptr(pSid);
  }

  const bool found_address=memory_helper::log_or_abort_if_memory_adddress_is_unknown(*g_memory_leak_repository, pSid, api_family_t::e_localalloc, api_functions_t::e_FreeSid);
  if(found_address)
  {
    g_memory_leak_repository->erase((uintptr_t)pSid);
  }

  return FreeSid_fptr(pSid);

}

DWORD WINAPI buginsight_GetSecurityInfo(HANDLE handle, SE_OBJECT_TYPE ObjectType, SECURITY_INFORMATION  SecurityInfo, PSID* ppsidOwner, PSID* ppsidGroup, PACL* ppDacl, PACL* ppSacl, PSECURITY_DESCRIPTOR* ppSecurityDescriptor)
{
  const DWORD res=GetSecurityInfo_fptr(handle, ObjectType, SecurityInfo, ppsidOwner, ppsidGroup, ppDacl, ppSacl, ppSecurityDescriptor);
  if(res != ERROR_SUCCESS)
  {
    return res;
  }

  if(!ppSecurityDescriptor)
  {
    return res;
  }

  const SIZE_T size=LocalSize(*ppSecurityDescriptor);

  g_memory_leak_repository->add((uintptr_t)*ppSecurityDescriptor, size, api_family_t::e_localalloc, api_functions_t::e_GetSecurityInfo);

  return res;
}

PSTR WINAPI buginsight_StrDupA(PCSTR pszSrch)
{
  const PSTR res=StrDupA_fptr(pszSrch);
  if(!res)
  {
    return res;
  }

  const SIZE_T size=LocalSize(res);

  g_memory_leak_repository->add((uintptr_t)res, size, api_family_t::e_localalloc, api_functions_t::e_StrDupA);

  return res;
}

PWSTR WINAPI buginsight_StrDupW(PCWSTR pszSrch)
{
  const PWSTR res=StrDupW_fptr(pszSrch);
  if(!res)
  {
    return res;
  }

  const SIZE_T size=LocalSize(res);

  g_memory_leak_repository->add((uintptr_t)res, size, api_family_t::e_localalloc, api_functions_t::e_StrDupW);

  return res;
}

