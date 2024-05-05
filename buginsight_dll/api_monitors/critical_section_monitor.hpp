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


#pragma once

#include <Windows.h>

using InitializeCriticalSection_t=void (WINAPI*)(LPCRITICAL_SECTION);
using InitializeCriticalSectionAndSpinCount_t=void (WINAPI*)(LPCRITICAL_SECTION, DWORD);
using EnterCriticalSection_t=void (WINAPI*)(LPCRITICAL_SECTION);
using LeaveCriticalSection_=void (WINAPI*)(LPCRITICAL_SECTION);
using TryEnterCriticalSection_t=bool (WINAPI*) (LPCRITICAL_SECTION);
using DeleteCriticalSection_t=void (WINAPI*)(LPCRITICAL_SECTION);

inline InitializeCriticalSection_t InitializeCriticalSection_fptr{};
inline InitializeCriticalSectionAndSpinCount_t InitializeCriticalSectionAndSpinCount_fptr{};
inline EnterCriticalSection_t EnterCriticalSection_fptr{};
inline LeaveCriticalSection_ LeaveCriticalSection_fptr{};
inline TryEnterCriticalSection_t TryEnterCriticalSection_fptr{};
inline DeleteCriticalSection_t DeleteCriticalSection_fptr{};

void buginsight_initialize_critical_section(void* lpCriticalSection);
void buginsight_initialize_critical_section_andspincount(void* lpCriticalSection, DWORD dwSpinCount);
void buginsight_enter_critical_section(void* lpCriticalSection);
void buginsight_leave_critical_section(void* lpCriticalSection);
bool buginsight_tryenter_critical_section(void* lpCriticalSection);
void buginsight_delete_critical_section(void* lpCriticalSection);

