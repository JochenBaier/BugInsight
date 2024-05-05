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


