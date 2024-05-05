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


#include "thread_watchdog.hpp"

#include <cassert>
#include <algorithm>
#include <mutex>
#include <fmt/std.h>

#include "deadlock_monitor.hpp"
#include "Log.hpp"
#include "shared_settings.hpp"
#include "synch_locks_held.hpp"
#include "timer.hpp"

thread_watchdog_t::thread_watchdog_t(deadlock_monitor_t& p_deadlock_monitor, user_options_t& p_runtime_options) :
  m_deadlock_monitor(p_deadlock_monitor),
  m_runtime_options(p_runtime_options),
  m_one_shot_timer(*this),
#ifdef _WIN32
  m_debugger_present(IsDebuggerPresent())
#else
  m_debugger_present(false)
#endif
{
}

void thread_watchdog_t::add_thread(const std::thread::id& p_thread_id, SynchLocksHeldPtr& p_snch)
{
  if(!m_runtime_options.m_thread_watchdog_enabled)
  {
    return;
  }

  const std::lock_guard lock(m_thread_list_mutex);

  LOG(trace, "thread_watchdog_t::add_thread: ID: {}", p_thread_id);
  m_threads.emplace_back(p_snch, p_thread_id);

  if(m_threads.size() == 1)
  {
    LOG(trace, "thread_watchdog_t::add_thread: ID: {}, start timer", p_thread_id);
    m_one_shot_timer.start(m_runtime_options.m_thread_watchdog_max_duration_sec >= 10?5:1);
  }
}

void thread_watchdog_t::remove_thread(const std::thread::id& p_thread_id)
{
  if(!m_runtime_options.m_thread_watchdog_enabled)
  {
    return;
  }

  {//scope
    const std::lock_guard lock(m_thread_list_mutex);

    const auto found=std::find_if(m_threads.begin(), m_threads.end(), [&p_thread_id](const auto& o) { return o.m_thread_id == p_thread_id; });

    if(found == m_threads.end())
    {
      assert(false);
      return;
    }

    LOG(trace, "thread_watchdog_t::remove_thread: ID: {}", p_thread_id);

    //copy last element to deleted place
    *found=m_threads.back();
    m_threads.pop_back();
  }
}

void  thread_watchdog_t::one_shot_timer_callback()
{
  LOG(trace, "thread_watchdog_t::one_shot_timer_callback: thread count: {}", m_threads.size());

  SynchLocksHeldPtr found_long_locked;
  uint64_t elapsed_ms=0;

  {//scope
    const std::lock_guard lock(m_thread_list_mutex);

    const auto now=timer::get_ms_tick_counts();

    for(auto& thread : m_threads)
    {
      const SynchLocksHeldPtr& sync_locks=thread.m_sync_ptr;
      const uint64_t lock_start_time_point=sync_locks->m_lock_start_time_point.load();
      if(lock_start_time_point == 0 || (lock_start_time_point >= now))
      {
        thread.m_already_logged=false;
        continue;
      }

      if(thread.m_already_logged)
      {
        continue;
      }

      elapsed_ms=now - lock_start_time_point;

      //avoid abort during debugging...
      const uint64_t max_duration_sec=(m_runtime_options.m_thread_watchdog_max_duration_sec <= 120 && m_debugger_present)?120:m_runtime_options.m_thread_watchdog_max_duration_sec;

      if(elapsed_ms >= max_duration_sec * 1000)
      {
        LOG(debug, "thread_watchdog_t::one_shot_timer_callback: max duration '{}' sec reached", max_duration_sec);
        found_long_locked=sync_locks;
        thread.m_already_logged=true;
        break;
      }

    }
  }

  if(found_long_locked)
  {
    m_deadlock_monitor.process_watchdog_error(*found_long_locked, elapsed_ms / 1000);
  }

  {//scope
    const std::lock_guard lock(m_thread_list_mutex);

    if(!m_threads.empty())
    {
      m_one_shot_timer.start(m_runtime_options.m_thread_watchdog_max_duration_sec >= 10?5:1);
    }
  }
}
