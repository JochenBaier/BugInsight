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
