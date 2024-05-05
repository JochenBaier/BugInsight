// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

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

//BugInsight, Copyright 2024 Jochen Baier <email@jochen-baier.de>

#include "log_helper.hpp"

#include <Windows.h>
#include <csignal>
#include <algorithm>
#include <cassert>

#include "fmt/std.h"

#include <boost/algorithm/string/join.hpp>
#include <boost/range/adaptors.hpp>

#include <synch_locks_held.hpp>
#include <graphcycles.h>
#include "log.hpp"
#include "run_time_options.hpp"
#include "stacktrace.h"
#include "shared_settings.hpp"
#include "xxhasher.hpp"

namespace log_helper
{
void create_options_log_text(const user_options_t& p_runtime_options, fmt::memory_buffer& p_out_buffer)
{
  fmt::format_to(std::back_inserter(p_out_buffer), "\n");
  fmt::format_to(std::back_inserter(p_out_buffer), "[Used BugInsight options]\n");

  std::string on_error_value;
  switch(p_runtime_options.m_on_error)
  {
  case on_error_option_t::fail: on_error_value="fail"; break;
  case on_error_option_t::fail_on_exit: on_error_value="fail on exit"; break;
  case on_error_option_t::_continue: on_error_value="continue"; break;
  }

  fmt::format_to(std::back_inserter(p_out_buffer), "  on error: {}\n", on_error_value);

  using namespace boost::adaptors;
  const std::string black_list_string_mem=boost::algorithm::join(p_runtime_options.m_blacklist_modules_memory_leaks | transformed(std::mem_fn(&blacklist_info_t::m_original_string)), ", ");
  fmt::format_to(std::back_inserter(p_out_buffer), "  ignore modules memory leak: '{}'\n", black_list_string_mem);

  const std::string black_list_string_leaks=boost::algorithm::join(p_runtime_options.m_blacklist_modules_deadlocks | transformed(std::mem_fn(&blacklist_info_t::m_original_string)), ", ");
  fmt::format_to(std::back_inserter(p_out_buffer), "  ignore modules deadlock: '{}'\n", black_list_string_leaks);

  fmt::format_to(std::back_inserter(p_out_buffer), "  collect Call Stacks: {}\n", p_runtime_options.m_collect_callstacks?"true":"false");
  fmt::format_to(std::back_inserter(p_out_buffer), "  start detached: {}\n", p_runtime_options.m_start_detached?"true":"false");

  std::string log_to_options;
  if(p_runtime_options.m_log_to_stdout)
  {
    log_to_options="stdout";
  }
  if(!p_runtime_options.m_log_to_file.empty())
  {
    log_to_options+=fmt::format("{}{}", log_to_options.empty()?"":", ", p_runtime_options.m_log_to_file);
  }
  fmt::format_to(std::back_inserter(p_out_buffer), "  log to: {}\n", log_to_options);

  auto level_as_string=spdlog::level::to_string_view(p_runtime_options.m_log_level);
  if(level_as_string == "critical")
  {
    level_as_string="fatal";
  }
  fmt::format_to(std::back_inserter(p_out_buffer), "  log level: {}\n", level_as_string);

  //Hidden fmt::format_to(std::back_inserter(p_out_buffer), "  config file: '{}'\n", !p_runtime_options.m_used_ini_file.empty()? p_runtime_options.m_used_ini_file.string() : "");
  fmt::format_to(std::back_inserter(p_out_buffer), "  thread_watchdog: {}\n", p_runtime_options.m_thread_watchdog_enabled?"true":"false");
  fmt::format_to(std::back_inserter(p_out_buffer), "  thread_watchdog_max_duration_sec: {}\n", p_runtime_options.m_thread_watchdog_max_duration_sec);

  std::string log_to_debug_out_value;
  switch(p_runtime_options.m_log_to_debug_out)
  {
  case log_to_debug_out_options_t::enabled: log_to_debug_out_value="enabled"; break;
  case log_to_debug_out_options_t::disabled: log_to_debug_out_value="disabled"; break;
  case log_to_debug_out_options_t::enabeld_in_debugger: log_to_debug_out_value="in debugger"; break;

  default: assert(false);  break;
  }
  fmt::format_to(std::back_inserter(p_out_buffer), "  log to debug out: {}\n", log_to_debug_out_value);

  //Hidden fmt::format_to(std::back_inserter(p_out_buffer), "  suppress_by_id: '{}'\n", fmt::join(p_runtime_options.m_ids_to_suppress, ", "));

  fmt::format_to(std::back_inserter(p_out_buffer), "  exit_code: {}\n", p_runtime_options.m_exit_code);
  fmt::format_to(std::back_inserter(p_out_buffer), "\n\n");
}

void create_summary(statistics_t& p_statistics, const bool& p_found_deadlocks, const bool& p_other_errors_found, const bool& p_found_memory_leaks,
  const bool& p_leaks_suppressed, uint64_t p_api_calls, fmt::memory_buffer& p_summary)
{
  boost::ignore_unused(p_statistics);
  boost::ignore_unused(p_leaks_suppressed);

  fmt::format_to(std::back_inserter(p_summary), "\n\n");
  fmt::format_to(std::back_inserter(p_summary), "==================\n");
  fmt::format_to(std::back_inserter(p_summary), "BugInsight summary:\n");
  fmt::format_to(std::back_inserter(p_summary), "==================\n");
  fmt::format_to(std::back_inserter(p_summary), "[{:7}] Monitored API calls:        {}{}\n", p_api_calls == 0?"Error":"Ok", p_api_calls, p_api_calls?"":", No API calls were monitored! Please check.");
  fmt::format_to(std::back_inserter(p_summary), "[{:7}] Potential deadlocks found:  {}\n", p_found_deadlocks?"Error":"Ok", p_found_deadlocks?"Yes":"No");
  fmt::format_to(std::back_inserter(p_summary), "[{:7}] Memory leaks found:         {}\n", p_found_memory_leaks?"Error":"Ok", p_found_memory_leaks?"Yes":"No");
  //not yet public fmt::format_to(std::back_inserter(p_summary), "[{:7}] Memory leaks suppressed:    {}\n", p_leaks_suppressed?"Warning":"Ok", p_leaks_suppressed?"Yes":"No");
  fmt::format_to(std::back_inserter(p_summary), "[{:7}] Errors found:               {}\n", p_other_errors_found?"Error":"Ok", p_other_errors_found?"Yes":"No");

#ifdef _DEBUG
  const bool critical_section_error=p_statistics.m_initial_critial_section_counter != p_statistics.m_delete_critial_section_counter;
  fmt::format_to(std::back_inserter(p_summary), "[{:7}] CriticalSection statistics: Initialized: {}, Deleted: {}\n", critical_section_error?"Error":"Ok",
    p_statistics.m_initial_critial_section_counter, p_statistics.m_delete_critial_section_counter);
#endif
  fmt::format_to(std::back_inserter(p_summary), "\n");
}

std::string construct_hold_locks_string(const std::array<lock_t, MAX_LOCKS_PER_THREAD>& hold_locks, const uint8_t& hold_locks_count)
{
  std::string hold_locks_info;
  for(size_t k=0; k < (std::min)(hold_locks.size(), static_cast<size_t>(hold_locks_count)); ++k)
  {
    hold_locks_info+=fmt::format("{}{}", k == 0?"":", ", hold_locks[k].mu);
  }

  return hold_locks_info;
}

TM_ATTRIBUTE_NOINLINE void build_deadlock_circle_log_text(const GraphCycles& p_deadlock_graph, void* mu, const uint64_t& mu_id, const uint64_t& other_node_id, fmt::memory_buffer& p_log_string)
{
  fmt::format_to(std::back_inserter(p_log_string), "\n\n\n==========================================================================================================\n");
  fmt::format_to(std::back_inserter(p_log_string), "[BugInsight] [error] Potential deadlock at lock: {}, thread ID: {}\n", mu, std::this_thread::get_id());

  if(!p_deadlock_graph.m_collect_callstacks)
  {
    fmt::format_to(std::back_inserter(p_log_string), "[BugInsight] [warning] Call Stack not available: Collecting Call Stacks is disabled! Use command line option '--callstacks=true' to enable.\n");
    return;
  }

  fmt::format_to(std::back_inserter(p_log_string), "Hint: Go through all Call Stacks and check which locks are called in inconsistent order\n\n");
  fmt::format_to(std::back_inserter(p_log_string), "Lock Call Stack:\n");
  save_current_callstack_to_string(p_log_string, 3);

  fmt::format_to(std::back_inserter(p_log_string), "\nA cycle in the historical lock ordering graph has been observed:\n");

  std::array<uint64_t, 10> path{};

  const size_t path_len=static_cast<size_t>(p_deadlock_graph.FindPath(mu_id, other_node_id, (int)path.size(), path.data()));

  for(size_t j=0; j != path_len && j != path.size(); j++)
  {
    const uint64_t& id=path[j];
    void* path_mu=static_cast<void*>(p_deadlock_graph.Ptr(id));
    if(path_mu == nullptr) continue;

    std::thread::id thread_id;
    uint8_t hold_locks_count=0;
    std::array<lock_t, MAX_LOCKS_PER_THREAD> hold_locks;
    p_deadlock_graph.extra_infos(id, thread_id, hold_locks_count, hold_locks);

    const std::string hold_locks_info=construct_hold_locks_string(hold_locks, hold_locks_count);

    std::optional<callstack_t> callstack;
    p_deadlock_graph.GetStackTrace(id, callstack);
    if(!callstack.has_value())
    {
      assert(false);
      fmt::format_to(std::back_inserter(p_log_string), "\n\nThread ID: {}\nHolding lock: {}\nWait for lock: {}\nCall Stack: (not available)\n", thread_id, hold_locks_info, path_mu);
      return;
    }

    fmt::format_to(std::back_inserter(p_log_string), "\n\nThread ID: {}\nHolding lock: {}\nWait for lock: {}\nCall Stack:\n", thread_id, hold_locks_info, path_mu);

    xxhasher_t callstack_hash_not_used;
    modules_static_runtime_t modules_static_runtime_not_used;
    printf_call_stack_symbol_infos(*callstack, p_log_string, callstack_hash_not_used, modules_static_runtime_not_used);

  }
}

TM_ATTRIBUTE_NOINLINE void build_delete_locked_lock_log_text(const std::thread::id& p_thread_id, const void* mu, fmt::memory_buffer& p_log_string)
{
  fmt::format_to(std::back_inserter(p_log_string), "\n\n\n==========================================================================================================\n");
  fmt::format_to(std::back_inserter(p_log_string), "BugInsight: [error] The thread with ID '{}' deletes the lock '{}' which the thread probably still holds.\n", p_thread_id, mu);

  fmt::format_to(std::back_inserter(p_log_string), "Call Stack:\n");
  save_current_callstack_to_string(p_log_string, 3);
}

TM_ATTRIBUTE_NOINLINE void build_release_lock_which_thread_does_hold_log_text(const std::thread::id& p_thread_id, const void* mu, fmt::memory_buffer& p_log_string)
{
  fmt::format_to(std::back_inserter(p_log_string), "\n\n\n==========================================================================================================\n");
  fmt::format_to(std::back_inserter(p_log_string), "BugInsight: [error] The thread with ID '{}' releasing the lock '{}' which is not hold by this thread.\n", p_thread_id, mu);

  fmt::format_to(std::back_inserter(p_log_string), "Call Stack:\n");
  save_current_callstack_to_string(p_log_string, 4);
}

TM_ATTRIBUTE_NOINLINE void build_release_locked_deleted_by_other_thread_log_text(const std::thread::id& p_thread_id, const void* mu, fmt::memory_buffer& p_log_string)
{
  fmt::format_to(std::back_inserter(p_log_string), "\n\n\n==========================================================================================================\n");
  fmt::format_to(std::back_inserter(p_log_string), "BugInsight: [error] The thread with ID '{}' releasing the lock '{}' which was probably already deleted by an other thread.\n", p_thread_id, mu);

  fmt::format_to(std::back_inserter(p_log_string), "Call Stack:\n");
  save_current_callstack_to_string(p_log_string, 3);
}

void build_watchdog_callstack(const GraphCycles& p_deadlock_graph, const lock_t& p_lock, fmt::memory_buffer& p_log_string)
{
  std::optional<callstack_t> callstack;
  p_deadlock_graph.GetStackTrace(p_lock.id, callstack);

  if(!callstack.has_value())
  {
    assert(false);
    fmt::format_to(std::back_inserter(p_log_string), "\n\nCall Stack of lock {}:\n", "not available");
    return;
  }

  fmt::format_to(std::back_inserter(p_log_string), "\n\nCall Stack of lock {}:\n", p_lock.mu);

  xxhasher_t callstack_hash_not_used;
  modules_static_runtime_t modules_static_runtime_not_used;
  printf_call_stack_symbol_infos(*callstack, p_log_string, callstack_hash_not_used, modules_static_runtime_not_used);
}

void build_watchdog_error(const std::thread::id& p_thread_id, const bool& p_in_lock, const uint64_t& p_hold_duration_sec, fmt::memory_buffer& p_log_string)
{
  fmt::format_to(std::back_inserter(p_log_string), "\n\n\n==========================================================================================================\n");

  if(p_in_lock)
  {
    fmt::format_to(std::back_inserter(p_log_string), "BugInsight: [error] The thread with ID '{}' holds a lock since {} seconds.\n", p_thread_id, p_hold_duration_sec);
  } else
  {
    fmt::format_to(std::back_inserter(p_log_string), "BugInsight: [error] The thread with ID '{}' waits to enter a lock since {} seconds.\n", p_thread_id, p_hold_duration_sec);
  }
}

TM_ATTRIBUTE_NOINLINE void build_max_locks_per_thread_reached_text(const int& p_lock_count, const std::thread::id& p_thread_id, const void* mu, fmt::memory_buffer& p_log_string)
{
  fmt::format_to(std::back_inserter(p_log_string), "\n\n\n==========================================================================================================\n");
  fmt::format_to(std::back_inserter(p_log_string), "BugInsight: [error] The thread with ID '{}' already holds {} locks. Max number of hold locks per thread reached. 'The lock '{}' will be ignored.", p_thread_id, p_lock_count, mu);

  fmt::format_to(std::back_inserter(p_log_string), "\nCallstack:\n");
  save_current_callstack_to_string(p_log_string, 3);
}

TM_ATTRIBUTE_NOINLINE void build_unknown_memory_address_text(const std::thread::id& p_thread_id, api_functions_t p_api_function, const void* p_address, fmt::memory_buffer& p_log_string)
{
  assert(p_api_function != api_functions_t::e_unknown);
  assert(p_address);
  fmt::format_to(std::back_inserter(p_log_string), "\n\n\n==========================================================================================================\n");
  fmt::format_to(std::back_inserter(p_log_string), "BugInsight: [error] The thread with ID '{}' calls '{}' with unknown memory address '{}'. Potential double free or memory corruption.\n",
    p_thread_id, api_function_to_string(p_api_function).c_str(), p_address);

  fmt::format_to(std::back_inserter(p_log_string), "Call Stack:\n");
  save_current_callstack_to_string(p_log_string, 3);
}

TM_ATTRIBUTE_NOINLINE void build_wrong_api_family_text(const std::thread::id& p_thread_id, api_functions_t p_api_function, api_family_t p_expected_api_family, api_family_t p_api_family, const void* p_address, fmt::memory_buffer& p_log_string)
{
  assert(p_api_function != api_functions_t::e_unknown);
  assert(p_address);
  fmt::format_to(std::back_inserter(p_log_string), "\n\n\n==========================================================================================================\n");
  fmt::format_to(std::back_inserter(p_log_string), "BugInsight: [error] The thread with ID '{}' calls '{}' (address '{}') with unexpected api type '{}', expected api type '{}'.\n",
    p_thread_id, api_function_to_string(p_api_function).c_str(), p_address, api_family_to_string(p_api_family), api_family_to_string(p_expected_api_family));

  fmt::format_to(std::back_inserter(p_log_string), "Call Stack:\n");
  save_current_callstack_to_string(p_log_string, 3);
}

TM_ATTRIBUTE_NOINLINE void build_heap_check_failed_text(const std::thread::id& p_thread_id, const std::string& p_function_name, fmt::memory_buffer& p_log_string)
{
  fmt::format_to(std::back_inserter(p_log_string), "\n\n\n==========================================================================================================\n");
  fmt::format_to(std::back_inserter(p_log_string), "BugInsight: [error] Heap check failed. Function '{}'. Thread ID '{}'.\n", p_function_name.c_str(), p_thread_id);

  fmt::format_to(std::back_inserter(p_log_string), "Call Stack:\n");
  save_current_callstack_to_string(p_log_string, 3);
}

void process_error(const fmt::memory_buffer& p_log_text, const user_options_t& p_runtime_options, const bool p_is_deadlock)
{
  LOG(info, fmt::to_string(p_log_text));

  if(p_runtime_options.m_on_error != on_error_option_t::fail)
  {
    return;
  }

  LOG(err, "Process will be terminated with exit code '{}' due to {}. Use command line option '--on_error=continue (or fail_on_exit)' to continue on error\n",
    p_runtime_options.m_exit_code, p_is_deadlock?"potential deadlock":"error");

  TerminateProcess(GetCurrentProcess(), p_runtime_options.m_exit_code);
}
}
