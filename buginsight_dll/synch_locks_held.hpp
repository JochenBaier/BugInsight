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

#pragma once

#include <atomic>
#include <thread>
#include <array>
#include <chrono>
#include <memory>

// Return an invalid graph id that will never be assigned by GraphCycles.
inline uint64_t InvalidGraphId() {
	return 0;
}

struct lock_t
{
	void* mu{ nullptr };      // lock acquired
	int32_t count{ 0 };  // times acquired
	uint64_t id{0};     // deadlock_graph id of acquired lock
};

struct SynchLocksHeld
{
	SynchLocksHeld():
	id(std::this_thread::get_id())
	{
	}

	std::thread::id id;
	int n{ 0 };          // number of valid entries in locks[]
	std::array<lock_t, MAX_LOCKS_PER_THREAD> locks{};
	std::atomic_uint64_t m_lock_start_time_point{0};
	std::atomic_bool m_thread_monitor_active{false};
};

typedef std::shared_ptr<SynchLocksHeld> SynchLocksHeldPtr;


