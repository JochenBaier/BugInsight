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

using HeapAlloc_t=LPVOID(WINAPI*)(HANDLE, DWORD, SIZE_T);
using HeapReAlloc_t=LPVOID(WINAPI*)(HANDLE, DWORD, LPVOID, SIZE_T);
using HeapFree_t=BOOL(WINAPI*)(HANDLE, DWORD, LPVOID);

inline HeapAlloc_t HeapAlloc_fptr{};
inline HeapReAlloc_t HeapReAlloc_fptr{};
inline HeapFree_t HeapFree_fptr{};

LPVOID buginsight_heapalloc(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes);
LPVOID buginsight_heaprepalloc(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes);
BOOL buginsight_heapfree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem);

