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


#include "critical_section_monitor.hpp"

#include <Windows.h>
#include <cassert>
#include <cstdint>

#include "deadlock_monitor.hpp"
#include "run_time_options.hpp"

extern deadlock_monitor_t* g_deadlock_monitor;
extern statistics_t g_statistics;
extern std::atomic_uint64_t g_monitored_api_calls;

void buginsight_initialize_critical_section(void* lpCriticalSection)
{
  ++g_monitored_api_calls;

  assert(lpCriticalSection);

  InitializeCriticalSection_fptr(static_cast<LPCRITICAL_SECTION>(lpCriticalSection));
  ++g_statistics.m_initial_critial_section_counter;
}

void buginsight_initialize_critical_section_andspincount(void* lpCriticalSection, DWORD dwSpinCount)
{
  ++g_monitored_api_calls;

  assert(lpCriticalSection);

  InitializeCriticalSectionAndSpinCount_fptr(static_cast<LPCRITICAL_SECTION>(lpCriticalSection), dwSpinCount);
  ++g_statistics.m_initial_critial_section_counter;
}

void buginsight_enter_critical_section(void* lpCriticalSection)
{
  ++g_monitored_api_calls;

  assert(lpCriticalSection);

  void* hold_locks=nullptr;
  assert(g_deadlock_monitor);

  uint64_t id=0;
  g_deadlock_monitor->dlc_deadlock_check_before_lock(lpCriticalSection, &hold_locks, id);
  if(id == 0)
  {
    EnterCriticalSection_fptr(static_cast<LPCRITICAL_SECTION>(lpCriticalSection));
    return;
  }

  EnterCriticalSection_fptr(static_cast<LPCRITICAL_SECTION>(lpCriticalSection));
  g_deadlock_monitor->dlc_deadlock_check_in_lock(lpCriticalSection, id, hold_locks);
}

void buginsight_leave_critical_section(void* lpCriticalSection)
{
  ++g_monitored_api_calls;

  assert(lpCriticalSection);

  g_deadlock_monitor->dlc_deadlock_check_after_lock(lpCriticalSection);
  LeaveCriticalSection_fptr(static_cast<LPCRITICAL_SECTION>(lpCriticalSection));
}

bool buginsight_tryenter_critical_section(void* lpCriticalSection)
{
  ++g_monitored_api_calls;

  void* hold_locks=nullptr;
  uint64_t id=0;
  g_deadlock_monitor->dlc_deadlock_check_before_try_lock(lpCriticalSection, &hold_locks, id);
  if(id == 0)
  {
    assert(false);
    return TryEnterCriticalSection_fptr(static_cast<LPCRITICAL_SECTION>(lpCriticalSection));
  }

  const bool ok=TryEnterCriticalSection_fptr(static_cast<LPCRITICAL_SECTION>(lpCriticalSection));
  if(!ok)
  {
    return ok;
  }

  g_deadlock_monitor->dlc_deadlock_check_in_lock(lpCriticalSection, id, hold_locks);
  return ok;
}

void buginsight_delete_critical_section(void* lpCriticalSection)
{
  ++g_monitored_api_calls;

  assert(lpCriticalSection);
  g_deadlock_monitor->dlc_deadlock_delete_lock(lpCriticalSection);
  DeleteCriticalSection_fptr(static_cast<LPCRITICAL_SECTION>(lpCriticalSection));
  ++g_statistics.m_delete_critial_section_counter;
}
