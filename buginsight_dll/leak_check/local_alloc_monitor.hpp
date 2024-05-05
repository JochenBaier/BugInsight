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

#pragma once

#include <Windows.h>
#include <aclapi.h>

using LocalFree_t=HLOCAL(WINAPI*)(HLOCAL);
using LocalAlloc_t=HLOCAL(WINAPI*)(UINT, SIZE_T);
using LocalReAlloc_t=HLOCAL(WINAPI*)(HLOCAL, SIZE_T, UINT);
using FormatMessageA_t=DWORD(WINAPI*)(DWORD dwFlags, LPCVOID lpSource, DWORD  dwMessageId, DWORD  dwLanguageId, LPTSTR lpBuffer, DWORD nSize, va_list* Arguments);
using FormatMessageW_t=DWORD(WINAPI*)(DWORD dwFlags, LPCVOID lpSource, DWORD   dwMessageId, DWORD   dwLanguageId, LPWSTR  lpBuffer, DWORD   nSize, va_list* Arguments);
using CommandLineToArgvW_t=LPWSTR * (WINAPI*)(LPCWSTR, int);

using CryptProtectData_t=BOOL(WINAPI*) (DATA_BLOB*, LPCWSTR, DATA_BLOB*, PVOID, CRYPTPROTECT_PROMPTSTRUCT*, DWORD, DATA_BLOB*);
using CryptUnprotectData_t=BOOL(WINAPI*) (DATA_BLOB*, LPWSTR*, DATA_BLOB*, PVOID, CRYPTPROTECT_PROMPTSTRUCT*, DWORD, DATA_BLOB*);

using ConvertStringSidToSidW_t=BOOL(WINAPI*)(LPCWSTR, PSID*);
using ConvertStringSidToSidA_t=BOOL(WINAPI*)(LPCSTR, PSID*);

using ConvertSidToStringSidW_t=BOOL(WINAPI*) (PSID, LPWSTR*);
using ConvertSidToStringSidA_t=BOOL(WINAPI*) (PSID, LPSTR*);

using GetNamedSecurityInfoA_t=DWORD(WINAPI*) (LPCSTR, SE_OBJECT_TYPE, SECURITY_INFORMATION, PSID*, PSID*, PACL*, PACL*, PSECURITY_DESCRIPTOR*);
using GetNamedSecurityInfoW_t=DWORD(WINAPI*) (LPCWSTR, SE_OBJECT_TYPE, SECURITY_INFORMATION, PSID*, PSID*, PACL*, PACL*, PSECURITY_DESCRIPTOR*);

using SetEntriesInAclA_t=DWORD(WINAPI*) (ULONG, PEXPLICIT_ACCESS_A, PACL, PACL*);
using SetEntriesInAclW_t=DWORD(WINAPI*) (ULONG, PEXPLICIT_ACCESS_W, PACL, PACL*);

using AllocateAndInitializeSid_t=BOOL(WINAPI*) (PSID_IDENTIFIER_AUTHORITY pIdentifierAuthority, BYTE nSubAuthorityCount, DWORD  nSubAuthority0, DWORD  nSubAuthority1, DWORD  nSubAuthority2,
  DWORD  nSubAuthority3, DWORD  nSubAuthority4, DWORD  nSubAuthority5, DWORD  nSubAuthority6, DWORD  nSubAuthority7, PSID* pSid);
using FreeSid_t=PVOID(WINAPI*) (PSID);

using GetSecurityInfo_t=DWORD(WINAPI*) (HANDLE handle, SE_OBJECT_TYPE ObjectType, SECURITY_INFORMATION  SecurityInfo, PSID* ppsidOwner, PSID* ppsidGroup, PACL* ppDacl, PACL* ppSacl, PSECURITY_DESCRIPTOR* ppSecurityDescriptor);

using StrDupA_t=PSTR(WINAPI*) (PCSTR pszSrch);
using StrDupW_t=PWSTR(WINAPI*) (PCWSTR pszSrch);


inline LocalFree_t LocalFree_fptr{};
inline LocalAlloc_t LocalAlloc_fptr{};
inline LocalReAlloc_t LocalReAlloc_fptr{};

inline FormatMessageA_t FormatMessageA_fptr{};
inline FormatMessageW_t FormatMessageW_fptr{};

inline CommandLineToArgvW_t CommandLineToArgvW_fptr{};

inline CryptProtectData_t CryptProtectData_fptr{};
inline CryptUnprotectData_t CryptUnprotectData_fptr{};

inline ConvertStringSidToSidW_t ConvertStringSidToSidW_fptr{};
inline ConvertStringSidToSidA_t ConvertStringSidToSidA_fptr{};

inline ConvertSidToStringSidW_t ConvertSidToStringSidW_fptr{};
inline ConvertSidToStringSidA_t ConvertSidToStringSidA_fptr{};

inline GetNamedSecurityInfoA_t GetNamedSecurityInfoA_fptr{};
inline GetNamedSecurityInfoW_t GetNamedSecurityInfoW_fptr{};

inline SetEntriesInAclA_t SetEntriesInAclA_fptr{};
inline SetEntriesInAclW_t SetEntriesInAclW_fptr{};

inline AllocateAndInitializeSid_t AllocateAndInitializeSid_fptr{};
inline FreeSid_t FreeSid_fptr{};
inline GetSecurityInfo_t GetSecurityInfo_fptr{};

inline StrDupA_t StrDupA_fptr{};
inline StrDupW_t StrDupW_fptr{};

HLOCAL buginsight_localfree(HLOCAL hMem);

HLOCAL buginsight_localalloc(UINT  uFlags, SIZE_T uBytes);
HLOCAL buginsight_localrealloc(HLOCAL hMem, SIZE_T uBytes, UINT uFlags);

DWORD buginsight_formatmessagea(DWORD dwFlags, LPCVOID lpSource, DWORD  dwMessageId, DWORD  dwLanguageId, LPTSTR lpBuffer, DWORD nSize, va_list* Arguments);
DWORD buginsight_formatmessagew(DWORD   dwFlags, LPCVOID lpSource, DWORD   dwMessageId, DWORD   dwLanguageId, LPWSTR  lpBuffer, DWORD   nSize, va_list* Arguments);

LPWSTR* buginsight_CommandLineToArgvW(LPCWSTR lpCmdLine, int* pNumArgs);

BOOL WINAPI buginsight_ConvertSidToStringSidW(PSID Sid, LPWSTR* StringSid);
BOOL WINAPI buginsight_ConvertSidToStringSidA(PSID Sid, LPSTR* StringSid);

BOOL WINAPI buginsight_ConvertStringSidToSidW(LPCWSTR StringSid, PSID* Sid);
BOOL WINAPI buginsight_ConvertStringSidToSidA(LPCSTR StringSid, PSID* Sid);


BOOL WINAPI buginsight_CryptProtectData(DATA_BLOB* pDataIn, LPCWSTR szDataDescr, DATA_BLOB* pOptionalEntropy, PVOID pvReserved, CRYPTPROTECT_PROMPTSTRUCT* pPromptStruct,
  DWORD dwFlags, DATA_BLOB* pDataOut);
BOOL WINAPI buginsight_CryptUnprotectData(DATA_BLOB* pDataIn, LPWSTR* ppszDataDescr, DATA_BLOB* pOptionalEntropy, PVOID pvReserved, CRYPTPROTECT_PROMPTSTRUCT* pPromptStruct,
  DWORD dwFlags, DATA_BLOB* pDataOut);


DWORD WINAPI buginsight_GetNamedSecurityInfoW(LPCWSTR pObjectName, SE_OBJECT_TYPE ObjectType, SECURITY_INFORMATION  SecurityInfo, PSID* ppsidOwner, PSID* ppsidGroup, PACL* ppDacl, PACL* ppSacl,
  PSECURITY_DESCRIPTOR* ppSecurityDescriptor);

DWORD WINAPI buginsight_GetNamedSecurityInfoA(LPCSTR pObjectName, SE_OBJECT_TYPE ObjectType, SECURITY_INFORMATION  SecurityInfo, PSID* ppsidOwner, PSID* ppsidGroup, PACL* ppDacl, PACL* ppSacl,
  PSECURITY_DESCRIPTOR* ppSecurityDescriptor);


DWORD WINAPI buginsight_SetEntriesInAclA(ULONG cCountOfExplicitEntries, PEXPLICIT_ACCESS_A  pListOfExplicitEntries, PACL OldAcl, PACL* NewAcl);
DWORD WINAPI buginsight_SetEntriesInAclW(ULONG cCountOfExplicitEntries, PEXPLICIT_ACCESS_W  pListOfExplicitEntries, PACL OldAcl, PACL* NewAcl);

BOOL buginsight_AllocateAndInitializeSid(PSID_IDENTIFIER_AUTHORITY pIdentifierAuthority, BYTE nSubAuthorityCount, DWORD  nSubAuthority0, DWORD  nSubAuthority1, DWORD  nSubAuthority2,
  DWORD  nSubAuthority3, DWORD  nSubAuthority4, DWORD  nSubAuthority5, DWORD  nSubAuthority6, DWORD  nSubAuthority7, PSID* pSid);
PVOID WINAPI buginsight_FreeSid(PSID pSid);

DWORD WINAPI buginsight_GetSecurityInfo(HANDLE handle, SE_OBJECT_TYPE ObjectType, SECURITY_INFORMATION  SecurityInfo, PSID* ppsidOwner, PSID* ppsidGroup, PACL* ppDacl, PACL* ppSacl, PSECURITY_DESCRIPTOR* ppSecurityDescriptor);


PSTR WINAPI buginsight_StrDupA(PCSTR pszSrch);
PWSTR WINAPI buginsight_StrDupW(PCWSTR pszSrch);




