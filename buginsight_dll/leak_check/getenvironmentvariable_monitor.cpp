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


#include "getenvironmentvariable_monitor.hpp"

#include <Windows.h>
#include <werapi.h>

#include <boost/algorithm/string.hpp>

#include "memory_leak_map_with_callstack.hpp"
#include <loader.hpp>
#include "deadlock_monitor.hpp"

extern user_options_t g_runtime_options;
extern memory_leak_repository_t* g_memory_leak_repository;
extern std::atomic_uint64_t g_monitored_api_calls;

DWORD buginsight_getenvironmentvariablea(LPCTSTR lpName, LPTSTR lpBuffer, DWORD nSize)
{
  ++g_monitored_api_calls;

  static std::string buginsight_env=fmt::format("{}/", boost::to_lower_copy(std::string(APP_NAME_PRETTY)));
  static std::string set_increase_lock_ignore_counter=fmt::format("{}set/lock++", buginsight_env);
  static std::string set_increase_unlock_ignore_counter=fmt::format("{}set/unlock++", buginsight_env);

  static std::string get_buginsight_enabled=fmt::format("{}get/enabled", buginsight_env);
  static std::string get_internal_allocated_bytes=fmt::format("{}get/mem/", buginsight_env);
  static std::string get_internal_rest_all_allocated=fmt::format("{}set/mem/reset_all", buginsight_env);

  if(lpName == nullptr || !boost::starts_with(lpName, buginsight_env.c_str()))
  {
    return GetEnvironmentVariableA_fptr(lpName, lpBuffer, nSize);
  }

  if(boost::equals(lpName, set_increase_lock_ignore_counter.c_str()))
  {
    deadlock_monitor_t::increase_ignore_counter(1, 0);
    return 1;
  }

  if(boost::equals(lpName, set_increase_unlock_ignore_counter.c_str()))
  {
    deadlock_monitor_t::increase_ignore_counter(0, 1);
    return 1;
  }

  if(boost::equals(lpName, get_buginsight_enabled.c_str()))
  {
    deadlock_monitor_t::increase_ignore_counter(0, 1);
    return 1;
  }

  if(boost::starts_with(lpName, get_internal_allocated_bytes.c_str()))
  {
    assert(g_memory_leak_repository);

    assert(nSize == sizeof(size_t));

    const std::string env_string=lpName;

    std::string function_name=env_string.substr(get_internal_allocated_bytes.size());
    assert(!function_name.empty());

    const size_t size=g_memory_leak_repository->buginsight_internal_get_allocated_bytes(function_name);

    *(size_t*)lpBuffer=size;

    return 1;
  }

  if(boost::equals(lpName, get_internal_rest_all_allocated.c_str()))
  {
    assert(g_memory_leak_repository);
    g_memory_leak_repository->internal_reset_all_allocations();
    return 1;
  }

  return 0;
}

