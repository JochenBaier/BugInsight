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

