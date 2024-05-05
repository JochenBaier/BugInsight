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

using HeapAlloc_t=LPVOID(WINAPI*)(HANDLE, DWORD, SIZE_T);
using HeapReAlloc_t=LPVOID(WINAPI*)(HANDLE, DWORD, LPVOID, SIZE_T);
using HeapFree_t=BOOL(WINAPI*)(HANDLE, DWORD, LPVOID);

inline HeapAlloc_t HeapAlloc_fptr{};
inline HeapReAlloc_t HeapReAlloc_fptr{};
inline HeapFree_t HeapFree_fptr{};

LPVOID buginsight_heapalloc(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes);
LPVOID buginsight_heaprepalloc(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes);
BOOL buginsight_heapfree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem);

