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


#include "thread_history_helper.hpp"

#include <cassert>

#include "synch_locks_held.hpp"
#include "log_helper.hpp"
#include "fmt/std.h"
#include "shared_settings.hpp"

namespace thread_history_helper
{
bool update_lock_history_after_lock(const void* mu, const uint64_t& id, SynchLocksHeld& held_locks, const user_options_t& p_runtime_options)
{
  const int n=held_locks.n;
  int i=0;
  while(i != n && held_locks.locks[i].id != id)
  {
    i++;
  }

  if(i == n)
  {
    // The deadlock id may have been reassigned after ForgetDeadlockInfo,
    // but in that case mu should still be present.
    i=0;
    while(i != n && held_locks.locks[i].mu != mu)
    {
      i++;
    }
    if(i == n)
    {
      // mu missing means releasing unheld lock
      auto log_text=fmt::memory_buffer();
      log_helper::build_release_lock_which_thread_does_hold_log_text(std::this_thread::get_id(), mu, log_text);
      log_helper::process_error(log_text, p_runtime_options, false);
      return false;
    }

    auto log_text=fmt::memory_buffer();
    log_helper::build_release_locked_deleted_by_other_thread_log_text(std::this_thread::get_id(), mu, log_text);
    log_helper::process_error(log_text, p_runtime_options, false);
    return false;
  }

  if(held_locks.locks[i].count == 1) {
    held_locks.n=n - 1;
    held_locks.locks[i]=held_locks.locks[n - 1];

    auto& [p_mu, p_count, p_id]=held_locks.locks[n - 1];
    p_id=InvalidGraphId();
    p_mu=nullptr;
    p_count=0;

  } else {
    assert(held_locks.locks[i].count > 0);
    held_locks.locks[i].count--;
  }

  return true;
}

void update_lock_history_in_lock(void* mu, const uint64_t& id, SynchLocksHeld& held_locks)
{
  assert(mu);

  int n=held_locks.n;
  int i=0;
  while(i != n && held_locks.locks[i].id != id)
  {
    i++;
  }

  if(i == n)
  {
    if(n == static_cast<int>(held_locks.locks.size()))
    {
      assert(false);
      return;
    }

    // we have room for lock
    auto& [p_mu, p_count, p_id]=held_locks.locks[i];
    p_mu=mu;
    p_count=1;
    p_id=id;

    held_locks.n=n + 1;

  } else
  {
    assert(held_locks.locks[i].count > 0);
    held_locks.locks[i].count++;
  }
}

}
