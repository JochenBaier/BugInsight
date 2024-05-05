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


#include "options_parser.hpp"

#include <algorithm>
#include <string>
#include "fmt/std.h"

#include <boost/algorithm/string.hpp>

#include "shared_settings.hpp"
#include "command_line_parser.hpp"
#include "log.hpp"

bool parse_to_blacklist_info(const std::string& p_dll_name_with_wildcards, blacklist_info_t& p_blacklist_info);
bool isdigit(const std::string& str);
std::pair<std::string, std::string> partition(const std::string& input, char p_char);

bool parse_options(const configuration_list_t& p_configuration_list, user_options_t& p_runtime_options)
{
  const std::vector<std::string> g_know_keys{"log_to", "log_level", "log_to_debug_out", "on_error", "thread_watchdog",
    "thread_watchdog_max_duration_sec", "exit_code", "suppress_by_id", "ignore_modules", "config_file", "callstacks", "start_detached"};
  const std::vector<std::string> g_true_false{"true", "false"};

  for(const auto& [key, value] : p_configuration_list)
  {
    if(std::find(g_know_keys.begin(), g_know_keys.end(), key) == g_know_keys.end())
    {
      LOG(err, fmt::format("Option '{}' is not known", key));
      return false;
    }

    if(value.empty())
    {
      LOG(err, fmt::format("Value from key '{}' is empty", key));
      return false;
    }

    if(key == "log_to")
    {
      std::vector<std::string> log_options;
      boost::split(log_options, value, boost::is_any_of(","));
      if(log_options.empty())
      {
        LOG(err, fmt::format("Value {}' not a valid log options", value));
        return false;
      }

      p_runtime_options.m_log_to_stdout=false;

      for(auto& log_option : log_options)
      {
        if(log_option == "stdout")
        {
          p_runtime_options.m_log_to_stdout=true;

        } else
        {
          p_runtime_options.m_log_to_file=log_option;
        }
      }

    } else if(key == "log_level")
    {
      if(value == "trace")
      {
        p_runtime_options.m_log_level=spdlog::level::trace;
      } else if(value == "debug")
      {
        p_runtime_options.m_log_level=spdlog::level::debug;
      } else if(value == "info")
      {
        p_runtime_options.m_log_level=spdlog::level::info;
      } else if(value == "warn")
      {
        p_runtime_options.m_log_level=spdlog::level::warn;
      } else if(value == "error")
      {
        p_runtime_options.m_log_level=spdlog::level::err;
      } else if(value == "fatal")
      {
        p_runtime_options.m_log_level=spdlog::level::critical;
      } else if(value == "off")
      {
        p_runtime_options.m_log_level=spdlog::level::off;
      } else
      {
        LOG(err, fmt::format("Option '{}' requires value 'trace|debug|info|warn|error|fatal|off' but got '{}'", key, value));
        return false;
      }

    } else if(key == "on_error")
    {
      if(value == "continue")
      {
        p_runtime_options.m_on_error=on_error_option_t::_continue;
      } else if(value == "fail")
      {
        p_runtime_options.m_on_error=on_error_option_t::fail;
      } else if(value == "fail_on_exit")
      {
        p_runtime_options.m_on_error=on_error_option_t::fail_on_exit;
      } else
      {
        LOG(err, fmt::format("Option '{}' requires value 'continue|fail|fail_on_exit' but got '{}'", key, value));
        return false;
      }

    } else if(key == "log_to_debug_out")
    {
      if(value == "enabled")
      {
        p_runtime_options.m_log_to_debug_out=log_to_debug_out_options_t::enabled;
      } else if(value == "disabled")
      {
        p_runtime_options.m_log_to_debug_out=log_to_debug_out_options_t::disabled;
      } else if(value == "debugger")
      {
        p_runtime_options.m_log_to_debug_out=log_to_debug_out_options_t::enabeld_in_debugger;
      } else
      {
        LOG(err, fmt::format("Option '{}' requires value 'enabled|disabled|debugger' but got '{}'", key, value));
        return false;
      }

    } else if(key == "thread_watchdog")
    {
      if(std::find(g_true_false.begin(), g_true_false.end(), value) == g_true_false.end())
      {
        LOG(err, fmt::format("Option '{}' requires value true/false but got '{}'", key, value));
        return false;
      }
      p_runtime_options.m_thread_watchdog_enabled=(value == "true");
    } else if(key == "thread_watchdog_max_duration_sec")
    {
      if(!isdigit(value))
      {
        LOG(err, fmt::format("Value '{}' for option '{}' is not number", value, key));
        return false;
      }
      const int as_int_sec=std::stoi(value);
      const bool in_range=as_int_sec > 1 && as_int_sec <= 24 * 60 * 60;
      if(!in_range)
      {
        LOG(err, fmt::format("Option '{}' requires value 2..{} but got '{}'", key, 24 * 60 * 60, value));
        return false;
      }

      p_runtime_options.m_thread_watchdog_max_duration_sec=as_int_sec;
    } else if(key == "exit_code")
    {
      if(!isdigit(value))
      {
        LOG(err, fmt::format("Value '{}' for option '{}' is not number", value, key));
        return false;
      }
      const int as_int=std::stoi(value);
      const bool in_range=as_int >= 0 && as_int < 127;
      if(!in_range)
      {
        LOG(err, fmt::format("Option '{}' requires value 1..{} but got '{}'", key, 126, value));
        return false;
      }

      p_runtime_options.m_exit_code=as_int;
    } else if(key == "suppress_by_id")
    {
      std::vector<std::string> hex_values_as_string;
      boost::split(hex_values_as_string, value, boost::is_any_of("|"));

      std::vector<uint64_t> ids;
      for(auto& hex_string : hex_values_as_string)
      {
        const uint64_t hex_number=std::stoull(hex_string, nullptr, 16);
        ids.push_back(hex_number);
      }

      p_runtime_options.m_ids_to_suppress=std::move(ids);

    } else if(key == "ignore_modules")
    {
      std::vector<std::string> module_names;
      boost::split(module_names, value, boost::is_any_of(","));

      for(auto& module_name : module_names)
      {
        blacklist_info_t blacklist_info{};
        if(!parse_to_blacklist_info(module_name, blacklist_info))
        {
          return false;
        }

        if(blacklist_info.m_ignore_lock)
        {
          p_runtime_options.m_blacklist_modules_deadlocks.push_back(blacklist_info);
        }

        if(blacklist_info.m_ignore_memory_leak)
        {
          p_runtime_options.m_blacklist_modules_memory_leaks.push_back(blacklist_info);
        }

      }
    } else if(key == "config_file")
    {
      //not yet public p_runtime_options.m_used_ini_file=value;
    } else if(key == "callstacks")
    {
      if(std::find(g_true_false.begin(), g_true_false.end(), value) == g_true_false.end())
      {
        LOG(err, fmt::format("Option '{}' requires value true/false but got '{}'", key, value));
        return false;
      }
      p_runtime_options.m_collect_callstacks=(value == "true");
    } else if(key == "start_detached")
    {
      if(std::find(g_true_false.begin(), g_true_false.end(), value) == g_true_false.end())
      {
        LOG(err, fmt::format("Option '{}' requires value true/false but got '{}'", key, value));
        return false;
      }
      p_runtime_options.m_start_detached=(value == "true");
    }
  }

  return true;
}

bool parse_to_blacklist_info(const std::string& p_dll_name_with_wildcards, blacklist_info_t& p_blacklist_info)
{
  bool ignore_memory_leak=false;
  bool ignore_lock=false;

  std::string module_name=boost::to_lower_copy(p_dll_name_with_wildcards);

  std::string::size_type bracket_open=p_dll_name_with_wildcards.find('{');
  if(bracket_open != std::string::npos)
  {
    std::string::size_type bracket_close=p_dll_name_with_wildcards.find('}');
    if(bracket_close == std::string::npos)
    {
      LOG(err, fmt::format("Option '{}' requires both '{{..}}' or no '{{}}', invalide value: '{}'", "ignore_modules", p_dll_name_with_wildcards));
      return false;
    }

    const std::string options=p_dll_name_with_wildcards.substr(bracket_open + 1, bracket_close - bracket_open - 1);
    if(options.empty())
    {
      LOG(err, fmt::format("Option '{}' '{{..}}' needs 'l' and/or 'm' inside brackets', invalide value: '{}'", "ignore_modules", p_dll_name_with_wildcards));
      return false;
    }

    if(options.find('l') != std::string::npos)
    {
      ignore_lock=true;
    }

    if(options.find('m') != std::string::npos)
    {
      ignore_memory_leak=true;
    }

    module_name=module_name.substr(0, bracket_open);

  } else
  {
    std::string::size_type bracket_close=p_dll_name_with_wildcards.find('}');
    if(bracket_close != std::string::npos)
    {
      LOG(err, fmt::format("Option '{}' requires both '{{..}}' or no '{{}}', invalide value: '{}'", "ignore_modules", p_dll_name_with_wildcards));
      return false;
    }

    ignore_memory_leak=true;
    ignore_lock=true;
  }

  std::string regexPattern;

  for(const char& c:module_name)
  {
    switch(c)
    {
    case '*': regexPattern+=".*"; break;
    case '?':
    case '.':
    case '\\':
    case '+':
    case '^':
    case '$':
    case '|':
    case '(':
    case ')':
    case '[':
    case ']':
    case '{':
    case '}':
      regexPattern+='\\';
      regexPattern+=c;
      break;
    default:
      regexPattern+=c;
      break;
    }
  }

  p_blacklist_info={regexPattern, module_name, ignore_memory_leak, ignore_lock};
  return true;
}

//from https://github.com/imageworks/pystring
bool isdigit(const std::string& str)
{
  std::string::size_type len=str.size();
  if(len == 0) return false;
  if(len == 1) return ::isdigit(str[0]);

  for(std::string::size_type i=0; i < len; ++i)
  {
    if(!::isdigit(str[i])) return false;
  }
  return true;
}

std::pair<std::string, std::string> partition(const std::string& input, char p_char)
{
  const size_t pos=input.find(p_char);
  if(pos != std::string::npos)
  {
    return {input.substr(0, pos), input.substr(pos + 1)};
  }

  return {};
}

