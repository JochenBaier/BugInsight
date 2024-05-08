// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

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


#include "deadlock_monitor.hpp"

#include <atomic>
#include <cassert>
#include <memory>
#include <mutex>

#include "log_helper.hpp"
#include "synch_locks_held.hpp"
#include "thread_history_helper.hpp"
#include "timer.hpp"
#include "thread_watchdog.hpp"
#include "shared_settings.hpp"

thread_local SynchLocksHeldPtr g_sync_locks_held;
thread_local unsigned int g_ignore_counter_lock=0;
thread_local unsigned int g_ignore_counter_unlock=0;

struct on_thread_delete_t
{
  explicit on_thread_delete_t(thread_watchdog_t& p_thread_watchdog) :
    m_thread_watchdog(p_thread_watchdog)
  {
  }

  ~on_thread_delete_t()
  {
    if(!g_sync_locks_held)
    {
      assert(false);
      return;
    }

    assert(g_ignore_counter_lock == 0);
    assert(g_ignore_counter_unlock == 0);

    g_sync_locks_held.reset();

    m_thread_watchdog.remove_thread(std::this_thread::get_id());
  }

  thread_watchdog_t& m_thread_watchdog;  // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
};

deadlock_monitor_t::deadlock_monitor_t(user_options_t& p_runtime_options) :
  m_runtime_options(p_runtime_options),
  m_deadlock_graph(p_runtime_options.m_collect_callstacks),
  m_thread_watchdog(*this, p_runtime_options)
{
}

void deadlock_monitor_t::increase_ignore_counter(const unsigned int p_lock_counter, const unsigned int p_unlocks_counter)
{
  g_ignore_counter_lock+=p_lock_counter;
  g_ignore_counter_unlock+=p_unlocks_counter;
}

SynchLocksHeld& deadlock_monitor_t::Synch_GetAllLocks(bool p_monitor_thread)
{
  if(!g_sync_locks_held)
  {
    g_sync_locks_held=std::make_shared<SynchLocksHeld>();
  }

  if(p_monitor_thread && !g_sync_locks_held->m_thread_monitor_active)
  {
    thread_local on_thread_delete_t synch_locks_deleter(m_thread_watchdog);
    m_thread_watchdog.add_thread(std::this_thread::get_id(), g_sync_locks_held);
    g_sync_locks_held->m_thread_monitor_active=true;
  }

  return *g_sync_locks_held.get();
}

TM_ATTRIBUTE_NOINLINE void deadlock_monitor_t::dlc_deadlock_check_before_lock(void* mu, void** p_hold_locks, uint64_t& p_graphid_handle_out)
{
  assert(p_graphid_handle_out == 0);
  if(g_ignore_counter_lock > 0)
  {
    --g_ignore_counter_lock;
    return;
  }

  SynchLocksHeld& all_locks=Synch_GetAllLocks(true);
  if(all_locks.n >= MAX_LOCKS_PER_THREAD)
  {
    auto log_text=fmt::memory_buffer();
    log_helper::build_max_locks_per_thread_reached_text(all_locks.n, std::this_thread::get_id(), mu, log_text);
    log_helper::process_error(log_text, m_runtime_options, false);
    m_other_errors_found=true;
    return;
  }

  *p_hold_locks=&all_locks;

  std::lock_guard lock(m_deadlock_graph_mutex);

  m_deadlock_graph.GetId(mu, p_graphid_handle_out);

  m_deadlock_graph.UpdateStackTrace(p_graphid_handle_out, all_locks);

  if(all_locks.n == 0)
  {
    assert(all_locks.m_lock_start_time_point == 0);

    all_locks.m_lock_start_time_point=timer::get_ms_tick_counts();
    // There are no other locks held. It's ok, since if this Mutex is involved in a deadlock,
    // it can't always be the first lock acquired by a thread.
    return;
  }

  // For each other mutex already held by this thread:
  for(int i=0; i != all_locks.n; i++) {
    const uint64_t& other_node_id=all_locks.locks[i].id;
    const void* other=m_deadlock_graph.Ptr(other_node_id);
    if(other == nullptr)
    {
      // Ignore stale lock
      continue;
    }

    //recursive cs?
    if(other_node_id == p_graphid_handle_out)
    {
      assert(all_locks.locks[i].count > 0);
      return;
    }

    // Add the acquired-before edge to the graph.
    if(!m_deadlock_graph.InsertEdge(other_node_id, p_graphid_handle_out))
    {
      auto log_text=fmt::memory_buffer();
      log_helper::build_deadlock_circle_log_text(m_deadlock_graph, mu, p_graphid_handle_out, other_node_id, log_text);
      log_helper::process_error(log_text, m_runtime_options, true);

      m_potential_deadlocks_found=true;

      break;
    }
  }
}

// Record a lock acquisition.  
void deadlock_monitor_t::dlc_deadlock_check_in_lock(void* mu, uint64_t p_id, void* p_hold_locks)
{
  assert(mu);
  assert(p_hold_locks);

  std::lock_guard lock(m_deadlock_graph_mutex);

  SynchLocksHeld* held_locks=static_cast<SynchLocksHeld*>(p_hold_locks);

  thread_history_helper::update_lock_history_in_lock(mu, p_id, *held_locks);
}

void deadlock_monitor_t::dlc_deadlock_check_after_lock(void* mu)
{
  assert(mu);
  if(g_ignore_counter_unlock > 0)
  {
    --g_ignore_counter_unlock;
    return;
  }

  {//scope
    std::lock_guard lock(m_deadlock_graph_mutex);

    uint64_t id=0;
    GetGraphId(mu, id);
    SynchLocksHeld& held_locks=Synch_GetAllLocks(false);

    const bool update_history_ok=thread_history_helper::update_lock_history_after_lock(mu, id, held_locks, m_runtime_options);
    if(!update_history_ok)
    {
      m_other_errors_found=true;
    }

    if(held_locks.n == 0)
    {
      if(update_history_ok)
      {
        assert(held_locks.m_lock_start_time_point != 0);
      }

      held_locks.m_lock_start_time_point=0;
    }
  }
}

void deadlock_monitor_t::dlc_deadlock_check_before_try_lock(void* mu, void** p_hold_locks, uint64_t& p_id)
{
  assert(mu);
  if(g_ignore_counter_lock > 0)
  {
    --g_ignore_counter_lock;
  }

  std::lock_guard lock(m_deadlock_graph_mutex);

  m_deadlock_graph.GetId(mu, p_id);

  SynchLocksHeld& all_locks=Synch_GetAllLocks(true);
  *p_hold_locks=&all_locks;

  if(all_locks.n == 0)
  {
    assert(all_locks.m_lock_start_time_point == 0);
    all_locks.m_lock_start_time_point=timer::get_ms_tick_counts();
  }
}

void deadlock_monitor_t::dlc_deadlock_delete_lock(void* mu)
{
  assert(mu);

  bool found_hold_lock=false;

  {//scope
    std::lock_guard lock(m_deadlock_graph_mutex);
    m_deadlock_graph.RemoveNode(mu);

    const SynchLocksHeld& all_locks=Synch_GetAllLocks(false);

    const size_t n=static_cast<size_t>(all_locks.n);
    for(size_t i=0; i < n; ++i)
    {
      if(all_locks.locks[i].mu == mu)
      {
        found_hold_lock=true;
        break;
      }
    }
  }

  if(!found_hold_lock)
  {
    return;
  }

  auto log_text=fmt::memory_buffer();
  log_helper::build_delete_locked_lock_log_text(std::this_thread::get_id(), mu, log_text);
  log_helper::process_error(log_text, m_runtime_options, false);
}

void deadlock_monitor_t::buginsight_cnd_wait(const void* mu)
{
  UNUSED(mu)
  assert(mu);
  SynchLocksHeld& all_locks=Synch_GetAllLocks(false);
  assert(all_locks.m_lock_start_time_point > 0);

  //'disable' thread watchdog for mutex used by std::condition_variable
  all_locks.m_lock_start_time_point=(std::numeric_limits<uint64_t>::max)();
}

void deadlock_monitor_t::process_watchdog_error(const SynchLocksHeld& p_sync_locks, const uint64_t& p_hold_duration_sec)
{
  m_other_errors_found=true;
  fmt::memory_buffer log_text;

  {//scope
    std::lock_guard lock(m_deadlock_graph_mutex);

    const bool call_stack_possible=p_sync_locks.n > 0;

    log_helper::build_watchdog_error(p_sync_locks.id, call_stack_possible, p_hold_duration_sec, log_text);
    if(call_stack_possible)
    {
      log_helper::build_watchdog_callstack(m_deadlock_graph, p_sync_locks.locks[0], log_text);
    }

  }

  log_helper::process_error(log_text, m_runtime_options, false);
}

