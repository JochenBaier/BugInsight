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

#include <cinttypes>

struct SynchLocksHeld;
struct user_options_t;

namespace thread_history_helper
{
bool update_lock_history_after_lock(const void* mu, const uint64_t& id, SynchLocksHeld& held_locks, const user_options_t& p_runtime_options);
void update_lock_history_in_lock(void* mu, const uint64_t& id, SynchLocksHeld& held_locks);
}
