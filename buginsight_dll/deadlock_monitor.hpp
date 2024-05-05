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

#include <cstdint>
#include <cassert>

#include "graphcycles.h"
#include "cs_mutex.hpp"
#include "thread_watchdog.hpp"

struct user_options_t;

class deadlock_monitor_t
{
public:
  deadlock_monitor_t(user_options_t& p_runtime_options);
  static void increase_ignore_counter(unsigned int p_lock_counter, unsigned int p_unlocks_counter);
  void dlc_deadlock_check_before_try_lock(void* mu, void** p_hold_locks, uint64_t& p_id);
  TM_ATTRIBUTE_NOINLINE void dlc_deadlock_check_before_lock(void* mu, void** p_hold_locks, uint64_t& p_graphid_handle);
  void dlc_deadlock_check_in_lock(void* mu, uint64_t p_id, void* p_hold_locks);
  void dlc_deadlock_check_after_lock(void* mu);
  void dlc_deadlock_delete_lock(void* mu);
  void buginsight_cnd_wait(const void* mu);
  void process_watchdog_error(const SynchLocksHeld& p_sync_locks, const uint64_t& p_hold_duration_sec);

  bool potential_deadlocks_found() const
  {
    return m_potential_deadlocks_found.load();
  }

  bool other_errors_found() const
  {
    return m_other_errors_found.load();
  }

private:
  void GetGraphId(void* mu, uint64_t& p_id) const
  {
    assert(mu);
    m_deadlock_graph.GetId(mu, p_id);
  }

  SynchLocksHeld& Synch_GetAllLocks(bool p_monitor_thread);

  user_options_t& m_runtime_options;
  GraphCycles m_deadlock_graph;
  cs_mutex_t m_deadlock_graph_mutex;
  thread_watchdog_t m_thread_watchdog;
  std::atomic_bool m_potential_deadlocks_found{false};
  std::atomic_bool m_other_errors_found{false};
};
