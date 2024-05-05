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

#include <thread>
#include <vector>

#include "cs_mutex.hpp"
#include "one_shot_timer.hpp"
#include "synch_locks_held.hpp"

struct user_options_t;
struct SynchLocksHeld;
class deadlock_monitor_t;

struct thread_info_t
{
  thread_info_t(const SynchLocksHeldPtr& p_sync_ptr, const std::thread::id& p_thread_id) :
    m_sync_ptr(p_sync_ptr),
    m_thread_id(p_thread_id)
  {
  }

  SynchLocksHeldPtr m_sync_ptr;
  std::thread::id m_thread_id;
  bool m_already_logged{false};
};

class thread_watchdog_t : public  one_shot_timer_callback_t
{
public:
  thread_watchdog_t(deadlock_monitor_t& p_deadlock_monitor, user_options_t& p_runtime_options);
  void add_thread(const std::thread::id& p_thread_id, SynchLocksHeldPtr& p_snch);
  void remove_thread(const std::thread::id& p_thread_id);
  void  one_shot_timer_callback() override;

private:
  deadlock_monitor_t& m_deadlock_monitor;
  user_options_t& m_runtime_options;
  cs_mutex_t m_thread_list_mutex;
  std::vector<thread_info_t> m_threads;
  one_shot_timer_t m_one_shot_timer;
  bool m_debugger_present;
};