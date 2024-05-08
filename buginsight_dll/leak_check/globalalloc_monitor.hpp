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

