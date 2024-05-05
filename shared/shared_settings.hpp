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

#include <string>

#include <boost/serialization/vector.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/core/ignore_unused.hpp>

#include <spdlog/spdlog.h>

struct blacklist_info_t
{
  std::string m_regex_string;
  std::string m_original_string;

  bool m_ignore_memory_leak{false};
  bool m_ignore_lock{false};

  template <typename Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    boost::ignore_unused(version);

    ar& m_regex_string;
    ar& m_original_string;
    ar& m_ignore_memory_leak;
    ar& m_ignore_lock;
  }
};

enum class on_error_option_t
{
  _continue,
  fail,
  fail_on_exit
};

enum class log_to_debug_out_options_t
{
  disabled,
  enabled,
  enabeld_in_debugger
};

struct user_options_t
{
  std::string m_log_to_file;
  bool m_log_to_stdout{true};
  spdlog::level::level_enum m_log_level{spdlog::level::info};
  log_to_debug_out_options_t m_log_to_debug_out{log_to_debug_out_options_t::enabeld_in_debugger};
  on_error_option_t m_on_error{on_error_option_t::fail_on_exit};
  bool m_thread_watchdog_enabled{true};
  uint32_t m_thread_watchdog_max_duration_sec{30};
  int m_exit_code={66};
  std::vector<uint64_t> m_ids_to_suppress;
  std::vector<blacklist_info_t> m_blacklist_modules_deadlocks;
  std::vector<blacklist_info_t> m_blacklist_modules_memory_leaks;
  bool m_collect_callstacks{true};
  bool m_start_detached{false};

  template <typename Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    boost::ignore_unused(version);

    ar& m_log_to_file;
    ar& m_log_to_stdout;
    ar& m_log_level;
    ar& m_log_to_debug_out;
    ar& m_on_error;
    ar& m_thread_watchdog_enabled;
    ar& m_thread_watchdog_max_duration_sec;
    ar& m_exit_code;
    ar& m_ids_to_suppress;
    ar& m_blacklist_modules_deadlocks;
    ar& m_blacklist_modules_memory_leaks;
    ar& m_collect_callstacks;
    ar& m_start_detached;
  }

};

namespace shared_settings
{
void save_user_options_to_shared_memory(boost::interprocess::managed_windows_shared_memory& p_managed_shm, const uint32_t& p_process_id, const user_options_t& p_user_options);
void get_user_options_from_shared_memory(user_options_t& p_user_options);
}
