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

using GlobalAlloc_t=HLOCAL(WINAPI*)(UINT, SIZE_T);
using GlobalReAlloc_t=HLOCAL(WINAPI*)(HLOCAL, SIZE_T, UINT);
using FormatMessageA_t=DWORD(WINAPI*)(DWORD dwFlags, LPCVOID lpSource, DWORD dwMessageId, DWORD dwLanguageId, LPTSTR lpBuffer, DWORD nSize, va_list* Arguments);
using FormatMessageW_t=DWORD(WINAPI*)(DWORD dwFlags, LPCVOID lpSource, DWORD dwMessageId, DWORD dwLanguageId, LPWSTR  lpBuffer, DWORD nSize, va_list* Arguments);
using GlobalFree_t=HLOCAL(WINAPI*)(HLOCAL);

inline GlobalAlloc_t GlobalAlloc_fptr{};
inline GlobalReAlloc_t GlobalReAlloc_fptr{};
inline GlobalFree_t GlobalFree_fptr{};

HLOCAL buginsight_globalalloc(UINT  uFlags, SIZE_T uBytes);
HLOCAL buginsight_globalrealloc(HLOCAL hMem, SIZE_T uBytes, UINT uFlags);
HLOCAL buginsight_globalfree(HLOCAL hMem);

