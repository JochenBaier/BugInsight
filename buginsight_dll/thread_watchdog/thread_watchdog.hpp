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