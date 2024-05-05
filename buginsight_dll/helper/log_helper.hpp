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

#include "attributes.hpp"
#include "fmt/format.h"
#include "leak_types.hpp"

class GraphCycles;
struct SynchLocksHeld;

struct lock_t;
struct statistics_t;
struct user_options_t;

namespace log_helper
{
void create_options_log_text(const user_options_t& p_runtime_options, fmt::memory_buffer& p_out_buffer);
TM_ATTRIBUTE_NOINLINE void build_deadlock_circle_log_text(const GraphCycles& p_deadlock_graph, void* mu, const uint64_t& mu_id, const uint64_t& other_node_id, fmt::memory_buffer& p_log_string);
void create_summary(statistics_t& p_statistics, const bool& p_found_deadlocks, const bool& p_other_errors_found, const bool& p_found_memory_leaks,
  const bool& p_leaks_suppressed, uint64_t p_api_calls, fmt::memory_buffer& p_summary);
TM_ATTRIBUTE_NOINLINE void build_delete_locked_lock_log_text(const std::thread::id& p_thread_id, const void* mu, fmt::memory_buffer& p_log_string);
TM_ATTRIBUTE_NOINLINE void build_release_locked_deleted_by_other_thread_log_text(const std::thread::id& p_thread_id, const void* mu, fmt::memory_buffer& p_log_string);
TM_ATTRIBUTE_NOINLINE void build_release_lock_which_thread_does_hold_log_text(const std::thread::id& p_thread_id, const void* mu, fmt::memory_buffer& p_log_string);
void build_watchdog_error(const std::thread::id& p_thread_id, const bool& p_in_lock, const uint64_t& p_hold_duration_sec, fmt::memory_buffer& p_log_string);
void build_watchdog_callstack(const GraphCycles& p_deadlock_graph, const lock_t& p_lock, fmt::memory_buffer& p_log_string);
TM_ATTRIBUTE_NOINLINE void build_max_locks_per_thread_reached_text(const int& p_lock_count, const std::thread::id& p_thread_id, const void* mu, fmt::memory_buffer& p_log_string);
void TM_ATTRIBUTE_NOINLINE build_unknown_memory_address_text(const std::thread::id& p_thread_id, api_functions_t p_api_function, const void* p_address, fmt::memory_buffer& p_log_string);
TM_ATTRIBUTE_NOINLINE void build_heap_check_failed_text(const std::thread::id& p_thread_id, const std::string& p_function_name, fmt::memory_buffer& p_log_string);
TM_ATTRIBUTE_NOINLINE void build_wrong_api_family_text(const std::thread::id& p_thread_id, api_functions_t p_api_function, api_family_t p_expected_api_family, api_family_t p_api_family, const void* p_address, fmt::memory_buffer& p_log_string);
void process_error(const fmt::memory_buffer& p_log_text, const user_options_t& p_runtime_options, bool p_is_deadlock);
}